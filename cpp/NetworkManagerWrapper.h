#ifndef NETWORK_MANAGER_WRAPPER_H
#define NETWORK_MANAGER_WRAPPER_H

#include <NetworkManager.h>
#include <string>
#include <memory>
#include <nm-client.h>
#include <nm-device.h>
#include <nm-connection.h>
#include <glib-object.h>
#include <spdlog/spdlog.h>

class NetworkManagerWrapper {
public:
public:
    NetworkManagerWrapper(const std::string& iface_name, const std::string& ssid, const std::string& password = "",
                          const std::string& ip_address = "", const std::string& gateway = "", const std::string& dns = "",
                          const std::string& connection_name = "MyConnection", std::shared_ptr<spdlog::logger> logger = nullptr);
    ~NetworkManagerWrapper();

    bool connectToAccessPointWithStaticIP();
    bool connectToAccessPointWithAutoIP();
    bool scanForSSID();

private:
    std::string iface_name_;
    std::string ssid_;
    std::string password_;
    std::string ip_address_;
    std::string gateway_;
    std::string dns_;
    std::string connection_name_;
    NMClient* client_;
    std::shared_ptr<spdlog::logger> logger_;

    void configureConnection(NMConnection* connection, NMDevice* device, bool auto_ip);
    bool activateConnection(NMConnection* connection, NMDevice* device);
    bool isSSIDConnected();


    static void print_ip_address(NMClient* client, const char* iface_name);
    static void printConnectionDetails(NMConnection* connection);
    static void delete_connection_callback(GObject *source_object, GAsyncResult *res, gpointer user_data);
    static bool isValidIPAddress(const std::string& ip_address);


    struct CallbackData {
        GMainLoop* loop;
        bool* success_ptr;
        std::string iface_name;  // Add this line
    };

};

#endif // NETWORK_MANAGER_WRAPPER_H
