#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <stdexcept>

class Configuration {
public:
    std::string getUsername() const;
    void setUsername(const std::string& username);

    std::string getPassword() const;
    void setPassword(const std::string& password);

    std::string getStaIpAddress() const;
    void setStaIpAddress(const std::string& ipAddress);

    std::string getStaLoggingLevel() const;
    void setStaLoggingLevel(const std::string& loggingLevel);

    std::string getStaWirelessMode() const;
    void setStaWirelessMode(const std::string& wirelessMode);

    std::string getStaWirelessSsid() const;
    void setStaWirelessSsid(const std::string& wirelessSsid);

    std::string getStaWirelessPassPhrase() const;
    void setStaWirelessPassPhrase(const std::string& wirelessPassPhrase);

    std::string getStaStatus() const;
    void setStaStatus(const std::string& status);

    std::string getApIpAddress() const;
    void setApIpAddress(const std::string& ipAddress);

    std::string getApLoggingLevel() const;
    void setApLoggingLevel(const std::string& loggingLevel);

    std::string getApWirelessMode() const;
    void setApWirelessMode(const std::string& wirelessMode);

    std::string getApWirelessSsid() const;
    void setApWirelessSsid(const std::string& wirelessSsid);

    std::string getApWirelessPassPhrase() const;
    void setApWirelessPassPhrase(const std::string& wirelessPassPhrase);

    std::string getApStatus() const;
    void setApStatus(const std::string& status);

    std::string getStaAuto() const;
    void setStaAuto(const std::string& mode);

    std::string getGateway() const;
    void setGateway(const std::string& gateway);

    std::string getDns() const;
    void setDns(const std::string& dns);
private:
    std::string username;
    std::string password;

    std::string sta_ip_address;
    std::string sta_logging_level;
    std::string sta_wireless_mode;
    std::string sta_wireless_ssid;
    std::string sta_wireless_pass_phrase;
    std::string sta_status;
    std::string sta_auto;
    std::string gateway;
    std::string dns;

    std::string ap_ip_address;
    std::string ap_logging_level;
    std::string ap_wireless_mode;
    std::string ap_wireless_ssid;
    std::string ap_wireless_pass_phrase;
    std::string ap_status;


    void validateNotEmpty(const std::string& value, const std::string& fieldName) const;
    void validateIpAddress(const std::string& ipAddress, const std::string& fieldName) const;
};

#endif // CONFIGURATION_H
