/* SPDX-License-Identifier: MIT */

#include "wifi.h"

#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/wifi_credentials.h>
#include <zephyr/net/wifi_mgmt.h>

LOG_MODULE_DECLARE(wifi_ble_scanner, LOG_LEVEL_INF);

#define WIFI_EVENT_MASK (NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT)

static struct net_mgmt_event_callback wifi_event_cb;

static void wifi_count_stored_ssid(void* cb_arg, const char* ssid, size_t ssid_len) {
  size_t* count = cb_arg;

  ARG_UNUSED(ssid);
  ARG_UNUSED(ssid_len);

  (*count)++;
}

static void wifi_handle_connect_result(const struct net_mgmt_event_callback* cb) {
  const struct wifi_status* status = cb->info;

  if (status == NULL) {
    LOG_WRN("Wi-Fi connect result received without status information.");
    return;
  }

  if (status->status != 0) {
    LOG_WRN("Wi-Fi connection failed: %d", status->status);
    return;
  }

  LOG_INF("Wi-Fi connected.");
}

static void wifi_handle_disconnect_result(const struct net_mgmt_event_callback* cb) {
  const struct wifi_status* status = cb->info;

  if (status == NULL) {
    LOG_WRN("Wi-Fi disconnect result received without status information.");
    return;
  }

  if (status->status != 0) {
    LOG_WRN("Wi-Fi disconnect completed with status: %d", status->status);
    return;
  }

  LOG_INF("Wi-Fi disconnected.");
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
  ARG_UNUSED(wifi_iface);

  net_mgmt_init_event_callback(&wifi_event_cb, wifi_event_handler, WIFI_EVENT_MASK);
  net_mgmt_add_event_callback(&wifi_event_cb);
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