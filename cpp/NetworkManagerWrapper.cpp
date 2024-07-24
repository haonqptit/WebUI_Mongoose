#include "NetworkManagerWrapper.h"
#include <stdexcept>
#include <regex>
#include <thread>
#include <chrono>
#include <nm-active-connection.h>
#include <nm-setting-connection.h>
#include <nm-setting-wireless.h>
#include <nm-setting-wireless-security.h>
#include <nm-setting-ip4-config.h>
#include <nm-setting-ip6-config.h>
#include <nm-access-point.h>
#include <nm-device-wifi.h>
#include <glib-object.h>
#include <glib.h>

NetworkManagerWrapper::NetworkManagerWrapper(const std::string& iface_name, const std::string& ssid, const std::string& password,
                                             const std::string& ip_address, const std::string& gateway, const std::string& dns,
                                             const std::string& connection_name, std::shared_ptr<spdlog::logger> logger)
    : iface_name_(iface_name), ssid_(ssid), password_(password), ip_address_(ip_address), gateway_(gateway), dns_(dns), connection_name_(connection_name), logger_(logger) {
    GError* error = nullptr;
    client_ = nm_client_new(nullptr, &error);
    if (error) {
        throw std::runtime_error("Error creating NMClient: " + std::string(error->message));
    }
    if (logger_) {
        logger_->info("NMClient created successfully.");
    }
}

NetworkManagerWrapper::~NetworkManagerWrapper() {
    if (client_) {
        g_object_unref(client_);
        if (logger_) {
            logger_->info("NMClient unref successfully.");
        }
    }
}

bool NetworkManagerWrapper::scanForSSID() {
    const int max_retries = 5;
    const int sleep_duration_ms = 1500;  // 2 seconds

    for (int i = 0; i < max_retries; ++i) {
        NMDevice* device = nm_client_get_device_by_iface(client_, iface_name_.c_str());
        if (!device) {
            if (logger_) {
                logger_->error("Device not found: {}", iface_name_);
            }
            return false;
        }

        if (!NM_IS_DEVICE_WIFI(device)) {
            if (logger_) {
                logger_->error("Device is not a WiFi device: {}", iface_name_);
            }
            return false;
        }

        NMDeviceWifi* wifi_device = NM_DEVICE_WIFI(device);
        const GPtrArray* access_points = nm_device_wifi_get_access_points(wifi_device);
        if (!access_points) {
            if (logger_) {
                logger_->error("No access points found.");
            }
            return false;
        }

        if (logger_) {
            logger_->info("Scanning for SSIDs...");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration_ms));
        for (guint j = 0; j < access_points->len; j++) {
             NMAccessPoint* ap = NM_ACCESS_POINT(g_ptr_array_index(access_points, j));
             GBytes* ssid_bytes = nm_access_point_get_ssid(ap);
             if (ssid_bytes) {
                 gsize ssid_length;
                 const gchar* ssid_data = (const gchar*)g_bytes_get_data(ssid_bytes, &ssid_length);
                 std::string ssid(ssid_data, ssid_length);
                 if (logger_) {
                     logger_->info("Found SSID: {}", ssid);
                 }
                 if (ssid == ssid_) {
                 //    g_bytes_unref(ssid_bytes); // Unreference the GBytes object
                     return true;
                 }
              //   g_bytes_unref(ssid_bytes); // Unreference the GBytes object
             }
         }


    }

    if (logger_) {
        logger_->error("SSID not found: {}", ssid_);
    }
    return false;
}

bool NetworkManagerWrapper::connectToAccessPointWithStaticIP() {
    if (!scanForSSID()) {
        return false;
    }

    if (!isValidIPAddress(ip_address_)) {
        if (logger_) {
            logger_->error("Invalid IP address format: {}", ip_address_);
        }
        return false;
    }
    if (!isValidIPAddress(gateway_)) {
        if (logger_) {
            logger_->error("Invalid gateway format: {}", gateway_);
        }
        return false;
    }
    if (!isValidIPAddress(dns_)) {
        if (logger_) {
            logger_->error("Invalid DNS format: {}", dns_);
        }
        return false;
    }

    NMDevice* device = nm_client_get_device_by_iface(client_, iface_name_.c_str());
    if (!device) {
        if (logger_) {
            logger_->error("Device not found: {}", iface_name_);
        }
        return false;
    }

    NMConnection* connection = nm_simple_connection_new();
    configureConnection(connection, device, false);

    NMRemoteConnection* existing_connection = nm_client_get_connection_by_id(client_, connection_name_.c_str());
    if (existing_connection) {
        nm_remote_connection_delete_async(existing_connection, nullptr, delete_connection_callback, nullptr);
        g_main_context_iteration(nullptr, TRUE);  // Block until the operation completes
    }

    if (logger_) {
        logger_->info("Connection details before activation:");
        printConnectionDetails(connection);
    }

    if (!activateConnection(connection, device)) {
        return false;
    }

    return isSSIDConnected();
}

bool NetworkManagerWrapper::connectToAccessPointWithAutoIP() {
    if (!scanForSSID()) {
        return false;
    }

    NMDevice* device = nm_client_get_device_by_iface(client_, iface_name_.c_str());
    if (!device) {
        if (logger_) {
            logger_->error("Device not found: {}", iface_name_);
        }
        return false;
    }

    NMConnection* connection = nm_simple_connection_new();
    configureConnection(connection, device, true);

    NMRemoteConnection* existing_connection = nm_client_get_connection_by_id(client_, connection_name_.c_str());
    if (existing_connection) {
        nm_remote_connection_delete_async(existing_connection, nullptr, delete_connection_callback, nullptr);
        g_main_context_iteration(nullptr, TRUE);  // Block until the operation completes
    }

    if (logger_) {
        logger_->info("Connection details before activation:");
        printConnectionDetails(connection);
    }

    if (!activateConnection(connection, device)) {
        if (logger_) {
            logger_->error("Failed to activate connection.");
        }
        return false;
    }

    // Validate SSID after attempting connection
    if (!isSSIDConnected()) {
        if (logger_) {
            logger_->error("Failed to connect to the specified SSID: {}", ssid_);
        }
        return false;
    }

    return true;
}

void NetworkManagerWrapper::delete_connection_callback(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GError *error = nullptr;
    gboolean result = nm_remote_connection_delete_finish(NM_REMOTE_CONNECTION(source_object), res, &error);
    if (!result) {
        spdlog::error("Error deleting existing connection: {}", error->message);
        g_clear_error(&error);
    } else {
        spdlog::info("Existing connection deleted successfully.");
    }
}

void NetworkManagerWrapper::configureConnection(NMConnection* connection, NMDevice* device, bool auto_ip) {
    NMSettingConnection* s_con = (NMSettingConnection*)nm_setting_connection_new();
    g_object_set(G_OBJECT(s_con), NM_SETTING_CONNECTION_ID, connection_name_.c_str(), NULL);
    g_object_set(G_OBJECT(s_con), NM_SETTING_CONNECTION_AUTOCONNECT, TRUE, NULL);
    g_object_set(G_OBJECT(s_con), NM_SETTING_CONNECTION_INTERFACE_NAME, iface_name_.c_str(), NULL);
    g_object_set(G_OBJECT(s_con), NM_SETTING_CONNECTION_TYPE, NM_SETTING_WIRELESS_SETTING_NAME, NULL);
    nm_connection_add_setting(connection, NM_SETTING(s_con));
    if (logger_) {
        logger_->info("Connection settings configured.");
    }

    NMSettingWireless* s_wifi = (NMSettingWireless*)nm_setting_wireless_new();
    GBytes* ssid_bytes = g_bytes_new(ssid_.c_str(), ssid_.length());
    g_object_set(G_OBJECT(s_wifi), NM_SETTING_WIRELESS_SSID, ssid_bytes, NULL);
    g_object_set(G_OBJECT(s_wifi), NM_SETTING_WIRELESS_MODE, NM_SETTING_WIRELESS_MODE_INFRA, NULL);
    g_bytes_unref(ssid_bytes);
    nm_connection_add_setting(connection, NM_SETTING(s_wifi));
    if (logger_) {
        logger_->info("Wireless settings configured.");
    }

    if (!password_.empty()) {
        NMSettingWirelessSecurity* s_wsec = (NMSettingWirelessSecurity*)nm_setting_wireless_security_new();
        g_object_set(G_OBJECT(s_wsec), NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, "wpa-psk", NULL);
        g_object_set(G_OBJECT(s_wsec), NM_SETTING_WIRELESS_SECURITY_PSK, password_.c_str(), NULL);
        nm_connection_add_setting(connection, NM_SETTING(s_wsec));
        if (logger_) {
            logger_->info("Wireless security settings configured.");
        }
    }

    NMSettingIPConfig* s_ip4 = (NMSettingIPConfig*)nm_setting_ip4_config_new();
    if (auto_ip) {
        g_object_set(G_OBJECT(s_ip4), NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP4_CONFIG_METHOD_AUTO, NULL);
    } else {
        g_object_set(G_OBJECT(s_ip4), NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP4_CONFIG_METHOD_MANUAL, NULL);
        GError* error = nullptr;
        nm_setting_ip_config_add_address(s_ip4, nm_ip_address_new(AF_INET, ip_address_.c_str(), 24, &error));
        if (error) {
            throw std::runtime_error("Error creating IP address: " + std::string(error->message));
        }
        nm_setting_ip_config_add_dns(s_ip4, dns_.c_str());
        g_object_set(G_OBJECT(s_ip4), NM_SETTING_IP_CONFIG_GATEWAY, gateway_.c_str(), NULL);
    }
    nm_connection_add_setting(connection, NM_SETTING(s_ip4));
    if (logger_) {
        logger_->info("IPv4 settings configured.");
    }

    NMSettingIPConfig* s_ip6 = (NMSettingIPConfig*)nm_setting_ip6_config_new();
    g_object_set(G_OBJECT(s_ip6), NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP6_CONFIG_METHOD_IGNORE, NULL);
    nm_connection_add_setting(connection, NM_SETTING(s_ip6));
    if (logger_) {
        logger_->info("IPv6 settings configured.");
    }
}

bool NetworkManagerWrapper::activateConnection(NMConnection* connection, NMDevice* device) {
    bool success = false;
    GMainLoop* loop = g_main_loop_new(nullptr, FALSE);

    CallbackData cb_data = { loop, &success, iface_name_ };

    if (logger_) {
        logger_->info("Main loop created.");
    }

    nm_client_add_and_activate_connection_async(client_, connection, device, nullptr, nullptr,
        [](GObject* source_object, GAsyncResult* res, gpointer user_data) {
            if (auto logger = spdlog::get("nm_logger")) {
                logger->info("Callback entered.");
            }
            GError* error = nullptr;
            NMActiveConnection* active_connection = nm_client_add_and_activate_connection_finish(NM_CLIENT(source_object), res, &error);
            CallbackData* cb_data = static_cast<CallbackData*>(user_data);
            if (error) {
                if (auto logger = spdlog::get("nm_logger")) {
                    logger->error("Error activating connection: {}", error->message);
                }
                g_clear_error(&error);
                *(cb_data->success_ptr) = false;
            } else {
                if (auto logger = spdlog::get("nm_logger")) {
                    logger->info("Connection activated successfully.");
                }
                *(cb_data->success_ptr) = true;
                g_object_unref(active_connection);
            }
            if (auto logger = spdlog::get("nm_logger")) {
                logger->info("Quitting main loop...");
            }
            if (cb_data->loop) {
                g_main_loop_quit(cb_data->loop);
            } else {
                if (auto logger = spdlog::get("nm_logger")) {
                    logger->error("Error: loop is nullptr.");
                }
            }
        }, &cb_data);

    std::this_thread::sleep_for(std::chrono::seconds(5)); // Added delay to ensure connection process completes
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    if (logger_) {
        logger_->info("Main loop unref done.");
    }

    // Validate if the SSID is actually connected
    if (success) {
        success = isSSIDConnected();
    }

    return success;
}

void NetworkManagerWrapper::print_ip_address(NMClient* client, const char* iface_name) {
    NMDevice* device = nm_client_get_device_by_iface(client, iface_name);
    if (!device) {
        spdlog::error("Device not found: {}", iface_name);
        return;
    }

    NMIPConfig* ip_config = nm_device_get_ip4_config(device);
    if (!ip_config) {
        spdlog::error("No IP4 config found for device: {}", iface_name);
        return;
    }

    const GPtrArray* addresses = nm_ip_config_get_addresses(ip_config);
    if (!addresses) {
        spdlog::error("No IP addresses found for device: {}", iface_name);
        return;
    }

    for (guint i = 0; i < addresses->len; i++) {
        NMIPAddress* addr = (NMIPAddress*)g_ptr_array_index(addresses, i);
        spdlog::info("IP address: {}/{}", nm_ip_address_get_address(addr), nm_ip_address_get_prefix(addr));
    }
}

void NetworkManagerWrapper::printConnectionDetails(NMConnection* connection) {
    guint length;
    NMSetting** settings = nm_connection_get_settings(connection, &length);
    for (guint i = 0; i < length; i++) {
        const char* name = nm_setting_get_name(settings[i]);
        spdlog::info("Setting: {}", name);
    }
}

bool NetworkManagerWrapper::isValidIPAddress(const std::string& ip_address) {
    const std::regex pattern("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                             "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                             "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                             "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return std::regex_match(ip_address, pattern);
}

bool NetworkManagerWrapper::isSSIDConnected() {
    const int max_retries = 10;
    const int sleep_duration_ms = 100;  // 1 second

    for (int i = 0; i < max_retries; ++i) {
        const GPtrArray* active_connections = nm_client_get_active_connections(client_);
        for (guint j = 0; j < active_connections->len; j++) {
            NMActiveConnection* active_connection = NM_ACTIVE_CONNECTION(g_ptr_array_index(active_connections, j));
            NMRemoteConnection* remote_connection = nm_active_connection_get_connection(active_connection);
            NMConnection* connection = NM_CONNECTION(remote_connection);
            NMSettingWireless* s_wifi = (NMSettingWireless*)nm_connection_get_setting(connection, NM_TYPE_SETTING_WIRELESS);
            if (s_wifi) {
                GBytes* ssid_bytes = nm_setting_wireless_get_ssid(s_wifi);
                if (ssid_bytes) {
                    gsize ssid_length;
                    const gchar* ssid_data = (const gchar*)g_bytes_get_data(ssid_bytes, &ssid_length);
                    std::string current_ssid(ssid_data, ssid_length);
                    if (current_ssid == ssid_) {
                        if (logger_) {
                            logger_->info("Successfully connected to SSID: {}", current_ssid);
                        }
                        return true;
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration_ms));
    }
    if (logger_) {
        logger_->error("Failed to connect to SSID: {}", ssid_);
    }
    return false;
}
