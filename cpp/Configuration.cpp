#include "Configuration.h"
void Configuration::validateNotEmpty(const std::string& value, const std::string& fieldName) const {
    if (value.empty()) {
        throw std::invalid_argument(fieldName + " cannot be empty");
    }
}

void Configuration::validateIpAddress(const std::string& ipAddress, const std::string& fieldName) const {
    if (ipAddress.empty()) {
        throw std::invalid_argument(fieldName + " cannot be empty");
    }
    // Additional IP address validation can be added here
}

std::string Configuration::getUsername() const {
    return username;
}

void Configuration::setUsername(const std::string& username) {
    validateNotEmpty(username, "Username");
    this->username = username;
}

std::string Configuration::getPassword() const {
    return password;
}

void Configuration::setPassword(const std::string& password) {
    validateNotEmpty(password, "Password");
    this->password = password;
}

std::string Configuration::getStaIpAddress() const {
    return sta_ip_address;
}

void Configuration::setStaIpAddress(const std::string& ipAddress) {
    validateIpAddress(ipAddress, "STA IP address");
    sta_ip_address = ipAddress;
}

std::string Configuration::getStaLoggingLevel() const {
    return sta_logging_level;
}

void Configuration::setStaLoggingLevel(const std::string& loggingLevel) {
    validateNotEmpty(loggingLevel, "STA logging level");
    sta_logging_level = loggingLevel;
}

std::string Configuration::getStaWirelessMode() const {
    return sta_wireless_mode;
}

void Configuration::setStaWirelessMode(const std::string& wirelessMode) {
    validateNotEmpty(wirelessMode, "STA wireless mode");
    sta_wireless_mode = wirelessMode;
}

std::string Configuration::getStaWirelessSsid() const {
    return sta_wireless_ssid;
}

void Configuration::setStaWirelessSsid(const std::string& wirelessSsid) {
    validateNotEmpty(wirelessSsid, "STA wireless SSID");
    sta_wireless_ssid = wirelessSsid;
}

std::string Configuration::getStaWirelessPassPhrase() const {
    return sta_wireless_pass_phrase;
}

void Configuration::setStaWirelessPassPhrase(const std::string& wirelessPassPhrase) {
    validateNotEmpty(wirelessPassPhrase, "STA wireless pass phrase");
    sta_wireless_pass_phrase = wirelessPassPhrase;
}

std::string Configuration::getStaStatus() const {
    return sta_status;
}

void Configuration::setStaStatus(const std::string& status) {
    validateNotEmpty(status, "STA status");
    sta_status = status;
}

std::string Configuration::getApIpAddress() const {
    return ap_ip_address;
}

void Configuration::setApIpAddress(const std::string& ipAddress) {
    validateIpAddress(ipAddress, "AP IP address");
    ap_ip_address = ipAddress;
}

std::string Configuration::getApLoggingLevel() const {
    return ap_logging_level;
}

void Configuration::setApLoggingLevel(const std::string& loggingLevel) {
    validateNotEmpty(loggingLevel, "AP logging level");
    ap_logging_level = loggingLevel;
}

std::string Configuration::getApWirelessMode() const {
    return ap_wireless_mode;
}

void Configuration::setApWirelessMode(const std::string& wirelessMode) {
    validateNotEmpty(wirelessMode, "AP wireless mode");
    ap_wireless_mode = wirelessMode;
}

std::string Configuration::getApWirelessSsid() const {
    return ap_wireless_ssid;
}

void Configuration::setApWirelessSsid(const std::string& wirelessSsid) {
    validateNotEmpty(wirelessSsid, "AP wireless SSID");
    ap_wireless_ssid = wirelessSsid;
}

std::string Configuration::getApWirelessPassPhrase() const {
    return ap_wireless_pass_phrase;
}

void Configuration::setApWirelessPassPhrase(const std::string& wirelessPassPhrase) {
    validateNotEmpty(wirelessPassPhrase, "AP wireless pass phrase");
    ap_wireless_pass_phrase = wirelessPassPhrase;
}

std::string Configuration::getApStatus() const {
    return ap_status;
}

void Configuration::setApStatus(const std::string& status) {
    validateNotEmpty(status, "AP status");
    ap_status = status;
}

std::string Configuration::getStaAuto() const {
    return sta_auto;
}

void Configuration::setStaAuto(const std::string& mode) {
    sta_auto = mode;
}

std::string Configuration::getDns() const {
    return dns;
}

void Configuration::setDns(const std::string& dns_) {
    dns = dns_;
}

std::string Configuration::getGateway() const {
    return gateway;
}

void Configuration::setGateway(const std::string& gateway_) {
	gateway = gateway_;
}

