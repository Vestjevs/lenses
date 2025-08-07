#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#include "uart_cmd.h"

static const char *TAG = "app";

// Kconfig selections
#ifndef CONFIG_NINA_UART_PORT
#define CONFIG_NINA_UART_PORT 1
#endif
#ifndef CONFIG_NINA_UART_TX_PIN
#define CONFIG_NINA_UART_TX_PIN 17
#endif
#ifndef CONFIG_NINA_UART_RX_PIN
#define CONFIG_NINA_UART_RX_PIN 16
#endif
#ifndef CONFIG_NINA_UART_BAUD
#define CONFIG_NINA_UART_BAUD 115200
#endif

#define DEFAULT_AP_CHANNEL 1
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64

static esp_netif_t *g_ap_netif = NULL;
static bool g_ap_running = false;
static char g_current_ssid[MAX_SSID_LEN + 1] = {0};
static int g_current_channel = DEFAULT_AP_CHANNEL;
static wifi_auth_mode_t g_current_auth = WIFI_AUTH_OPEN;

static void send_ok(void) {
    uart_cmd_write_line("OK");
}

static void send_err(const char *code, const char *msg) {
    char buf[256];
    snprintf(buf, sizeof(buf), "ERR %s %s", code ? code : "GENERIC", msg ? msg : "");
    uart_cmd_write_line(buf);
}

static void send_status(void) {
    // IP address of SoftAP
    esp_netif_ip_info_t ip_info = {0};
    char ip_str[16] = "0.0.0.0";
    if (g_ap_netif && esp_netif_get_ip_info(g_ap_netif, &ip_info) == ESP_OK) {
        snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d",
                 esp_ip4_addr1(&ip_info.ip), esp_ip4_addr2(&ip_info.ip),
                 esp_ip4_addr3(&ip_info.ip), esp_ip4_addr4(&ip_info.ip));
    }

    // stations connected
    wifi_sta_list_t sta_list = {0};
    esp_err_t r = esp_wifi_ap_get_sta_list(&sta_list);
    int num_clients = (r == ESP_OK) ? (int)sta_list.num : 0;

    char buf[256];
    snprintf(buf, sizeof(buf),
             "STATUS state=%s ssid=\"%s\" ip=%s ch=%d clients=%d",
             g_ap_running ? "RUNNING" : "STOPPED",
             g_current_ssid,
             ip_str,
             g_current_channel,
             num_clients);
    uart_cmd_write_line(buf);
}

static esp_err_t stop_ap(void) {
    if (!g_ap_running) {
        return ESP_OK;
    }
    ESP_LOGI(TAG, "Stopping SoftAP");
    ESP_ERROR_CHECK(esp_wifi_stop());
    g_ap_running = false;
    return ESP_OK;
}

static esp_err_t start_ap(const char *ssid, const char *password) {
    if (!ssid || ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }
    size_t ssid_len = strlen(ssid);
    size_t pass_len = password ? strlen(password) : 0;

    if (ssid_len == 0 || ssid_len > MAX_SSID_LEN) return ESP_ERR_INVALID_ARG;
    if (pass_len > 0 && (pass_len < 8 || pass_len > MAX_PASS_LEN)) return ESP_ERR_INVALID_ARG;

    wifi_config_t cfg = {0};
    memcpy(cfg.ap.ssid, ssid, ssid_len);
    cfg.ap.ssid_len = ssid_len;
    if (pass_len > 0) {
        memcpy(cfg.ap.password, password, pass_len);
        cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
    } else {
        cfg.ap.password[0] = '\0';
        cfg.ap.authmode = WIFI_AUTH_OPEN;
    }
    cfg.ap.channel = DEFAULT_AP_CHANNEL;
    cfg.ap.max_connection = 8;
    cfg.ap.ssid_hidden = 0;
    cfg.ap.beacon_interval = 100;

    ESP_LOGI(TAG, "Configuring SoftAP: ssid='%s' ch=%d auth=%s", ssid, cfg.ap.channel,
             (cfg.ap.authmode == WIFI_AUTH_OPEN) ? "OPEN" : "WPA2_PSK");

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    strncpy(g_current_ssid, ssid, sizeof(g_current_ssid) - 1);
    g_current_ssid[sizeof(g_current_ssid) - 1] = '\0';
    g_current_channel = cfg.ap.channel;
    g_current_auth = cfg.ap.authmode;
    g_ap_running = true;

    return ESP_OK;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "AP started");
                break;
            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(TAG, "AP stopped");
                g_ap_running = false;
                break;
            case WIFI_EVENT_AP_STACONNECTED: {
                wifi_event_ap_staconnected_t *e = (wifi_event_ap_staconnected_t *)event_data;
                ESP_LOGI(TAG, "Station " MACSTR " joined, AID=%d", MAC2STR(e->mac), e->aid);
                break;
            }
            case WIFI_EVENT_AP_STADISCONNECTED: {
                wifi_event_ap_stadisconnected_t *e = (wifi_event_ap_stadisconnected_t *)event_data;
                ESP_LOGI(TAG, "Station " MACSTR " left, AID=%d", MAC2STR(e->mac), e->aid);
                break;
            }
            default:
                break;
        }
    }
}

static void init_wifi(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create default AP netif
    g_ap_netif = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
}

static bool parse_quoted_pair(const char *s, char *out_ssid, size_t ssid_cap, char *out_pwd, size_t pwd_cap) {
    // Expect: 'AP SET "<ssid>" "<pwd>"'
    const char *p = strchr(s, '"');
    if (!p) return false;
    const char *q = strchr(p + 1, '"');
    if (!q) return false;
    size_t ssid_len = (size_t)(q - (p + 1));
    if (ssid_len >= ssid_cap) return false;
    memcpy(out_ssid, p + 1, ssid_len);
    out_ssid[ssid_len] = '\0';

    const char *p2 = strchr(q + 1, '"');
    if (!p2) return false;
    const char *q2 = strchr(p2 + 1, '"');
    if (!q2) return false;
    size_t pwd_len = (size_t)(q2 - (p2 + 1));
    if (pwd_len >= pwd_cap) return false;
    memcpy(out_pwd, p2 + 1, pwd_len);
    out_pwd[pwd_len] = '\0';

    return true;
}

static void handle_line(const char *line) {
    ESP_LOGI(TAG, "UART RX: %s", line);

    if (strcasecmp(line, "PING") == 0) {
        uart_cmd_write_line("PONG");
        return;
    }

    if (strncasecmp(line, "AP STATUS", 9) == 0) {
        send_status();
        return;
    }

    if (strncasecmp(line, "AP STOP", 7) == 0) {
        if (stop_ap() == ESP_OK) send_ok();
        else send_err("STOP", "failed");
        return;
    }

    if (strncasecmp(line, "AP SET", 6) == 0) {
        char ssid[MAX_SSID_LEN + 1];
        char pwd[MAX_PASS_LEN + 1];
        if (!parse_quoted_pair(line, ssid, sizeof(ssid), pwd, sizeof(pwd))) {
            send_err("ARGS", "expected AP SET \"ssid\" \"pwd\"");
            return;
        }
        if (stop_ap() != ESP_OK) {
            send_err("STOP", "failed to stop AP");
            return;
        }
        esp_err_t r = start_ap(ssid, pwd);
        if (r == ESP_OK) send_ok();
        else send_err("START", "invalid ssid/pwd or wifi error");
        return;
    }

    send_err("CMD", "unknown command");
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    init_wifi();

    uart_cmd_init((uart_port_t)CONFIG_NINA_UART_PORT,
                  CONFIG_NINA_UART_TX_PIN,
                  CONFIG_NINA_UART_RX_PIN,
                  CONFIG_NINA_UART_BAUD,
                  handle_line);

    // Announce readiness
    uart_cmd_write_line("READY NINA-AP v1");

    // Optionally, start with a default AP for testing (disabled by default)
    // start_ap("NINA-AP", "12345678");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}