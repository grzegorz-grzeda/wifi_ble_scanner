/* SPDX-License-Identifier: MIT */

#include "wifi.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/wifi_credentials.h>
#include <zephyr/net/wifi_mgmt.h>

LOG_MODULE_DECLARE(wifi_ble_scanner, LOG_LEVEL_INF);

#define WIFI_EVENT_MASK (NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT)
#define WIFI_AUTOCONNECT_INITIAL_RETRY_SECONDS 2U
#define WIFI_AUTOCONNECT_MAX_RETRY_SECONDS 60U

static struct net_mgmt_event_callback wifi_event_cb;
static struct net_if* wifi_autoconnect_iface;
static struct k_work_delayable wifi_autoconnect_retry_work;
static bool wifi_autoconnect_active;
static bool wifi_connect_in_progress;
static uint32_t wifi_retry_attempt;

static int wifi_request_stored_connect(struct net_if* wifi_iface);
static void wifi_schedule_autoconnect_retry(const char* reason, int status);

static uint32_t wifi_retry_delay_seconds(uint32_t retry_attempt) {
  uint32_t delay_seconds = WIFI_AUTOCONNECT_INITIAL_RETRY_SECONDS;

  while (retry_attempt > 1U && delay_seconds < WIFI_AUTOCONNECT_MAX_RETRY_SECONDS) {
    delay_seconds *= 2U;
    retry_attempt--;
  }

  if (delay_seconds > WIFI_AUTOCONNECT_MAX_RETRY_SECONDS) {
    delay_seconds = WIFI_AUTOCONNECT_MAX_RETRY_SECONDS;
  }

  return delay_seconds;
}

static void wifi_autoconnect_retry_work_handler(struct k_work* work) {
  int ret;

  ARG_UNUSED(work);

  if (!wifi_autoconnect_active || wifi_autoconnect_iface == NULL || wifi_connect_in_progress) {
    return;
  }

  ret = wifi_request_stored_connect(wifi_autoconnect_iface);
  if (ret != 0) {
    wifi_schedule_autoconnect_retry("retry submission failed", ret);
  }
}

static void wifi_count_stored_ssid(void* cb_arg, const char* ssid, size_t ssid_len) {
  size_t* count = cb_arg;

  ARG_UNUSED(ssid);
  ARG_UNUSED(ssid_len);

  (*count)++;
}

static void wifi_handle_connect_result(const struct net_mgmt_event_callback* cb) {
  const struct wifi_status* status = cb->info;

  wifi_connect_in_progress = false;

  if (status == NULL) {
    LOG_WRN("Wi-Fi connect result received without status information.");
    if (wifi_autoconnect_active) {
      wifi_schedule_autoconnect_retry("missing connect status", -EIO);
    }
    return;
  }

  if (status->status != 0) {
    LOG_WRN("Wi-Fi connection failed: %d", status->status);
    if (wifi_autoconnect_active) {
      wifi_schedule_autoconnect_retry("connect failure", status->status);
    }
    return;
  }

  wifi_autoconnect_active = false;
  wifi_retry_attempt = 0U;
  (void)k_work_cancel_delayable(&wifi_autoconnect_retry_work);
  LOG_INF("Wi-Fi connected.");
}

static void wifi_handle_disconnect_result(const struct net_mgmt_event_callback* cb) {
  const struct wifi_status* status = cb->info;

  /* Some failure paths report only a disconnect event, so release the
   * in-progress gate before deciding whether to schedule a retry.
   */
  wifi_connect_in_progress = false;

  if (status == NULL) {
    LOG_WRN("Wi-Fi disconnect result received without status information.");
    if (wifi_autoconnect_active) {
      wifi_schedule_autoconnect_retry("missing disconnect status", -EIO);
    }
    return;
  }

  if (status->status != 0) {
    LOG_WRN("Wi-Fi disconnect completed with status: %d", status->status);
    if (wifi_autoconnect_active) {
      wifi_schedule_autoconnect_retry("disconnect status", status->status);
    }
    return;
  }

  LOG_INF("Wi-Fi disconnected.");
  if (wifi_autoconnect_active) {
    wifi_schedule_autoconnect_retry("disconnect before successful connect", 0);
  }
}

static int wifi_request_stored_connect(struct net_if* wifi_iface) {
  int ret;

  wifi_connect_in_progress = true;
  ret = net_mgmt(NET_REQUEST_WIFI_CONNECT_STORED, wifi_iface, NULL, 0);
  if (ret != 0) {
    wifi_connect_in_progress = false;
  }

  return ret;
}

static void wifi_schedule_autoconnect_retry(const char* reason, int status) {
  uint32_t retry_number;
  uint32_t delay_seconds;

  if (!wifi_autoconnect_active || wifi_autoconnect_iface == NULL || wifi_connect_in_progress) {
    return;
  }

  retry_number = wifi_retry_attempt + 1U;
  delay_seconds = wifi_retry_delay_seconds(retry_number);
  wifi_retry_attempt = retry_number;

  LOG_WRN("Stored Wi-Fi auto-connect %s (%d). Retry %u in %u second(s).", reason, status, retry_number, delay_seconds);

  (void)k_work_reschedule(&wifi_autoconnect_retry_work, K_SECONDS(delay_seconds));
}

static void wifi_event_handler(struct net_mgmt_event_callback* cb, uint64_t mgmt_event, struct net_if* iface) {
  ARG_UNUSED(iface);

  switch (mgmt_event) {
    case NET_EVENT_WIFI_CONNECT_RESULT:
      wifi_handle_connect_result(cb);
      break;
    case NET_EVENT_WIFI_DISCONNECT_RESULT:
      wifi_handle_disconnect_result(cb);
      break;
    default:
      break;
  }
}

void wifi_ble_scanner_wifi_init(struct net_if* wifi_iface) {
  wifi_autoconnect_iface = wifi_iface;
  wifi_autoconnect_active = false;
  wifi_connect_in_progress = false;
  wifi_retry_attempt = 0U;

  net_mgmt_init_event_callback(&wifi_event_cb, wifi_event_handler, WIFI_EVENT_MASK);
  net_mgmt_add_event_callback(&wifi_event_cb);
  k_work_init_delayable(&wifi_autoconnect_retry_work, wifi_autoconnect_retry_work_handler);
}

void wifi_ble_scanner_wifi_log_stored_credentials(void) {
  size_t stored_networks = 0;

  wifi_credentials_for_each_ssid(wifi_count_stored_ssid, &stored_networks);

  if (stored_networks == 0U) {
    LOG_INF("No stored Wi-Fi credentials. Use 'wifi cred add ...' from the UART shell.");
    return;
  }

  LOG_INF("Found %u stored Wi-Fi credential(s). Use 'wifi cred list' or 'wifi cred auto_connect'.",
          (unsigned int)stored_networks);
}

int wifi_ble_scanner_wifi_request_autoconnect(struct net_if* wifi_iface) {
  return net_mgmt(NET_REQUEST_WIFI_CONNECT_STORED, wifi_iface, NULL, 0);
}

int wifi_ble_scanner_wifi_start_autoconnect(struct net_if* wifi_iface) {
  int ret;

  if (wifi_iface == NULL) {
    return -EINVAL;
  }

  wifi_autoconnect_iface = wifi_iface;
  wifi_autoconnect_active = true;
  wifi_connect_in_progress = false;
  wifi_retry_attempt = 0U;

  ret = wifi_request_stored_connect(wifi_iface);
  if (ret != 0) {
    wifi_schedule_autoconnect_retry("initial request failed", ret);
  }

  return 0;
}