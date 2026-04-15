# SPDX-License-Identifier: MIT

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(led)
add_subdirectory(main)
add_subdirectory(settings)
add_subdirectory(shell)
add_subdirectory(wifi)