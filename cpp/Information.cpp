#include "Information.h"

void Information::validateNotEmpty(const std::string& value, const std::string& fieldName) const {
    if (value.empty()) {
        throw std::invalid_argument(fieldName + " cannot be empty");
    }
}

void Information::validatePath(const std::string& path, const std::string& fieldName) const {
    if (path.empty() || path[0] != '/') {
        throw std::invalid_argument(fieldName + " must be a valid absolute path");
    }
}

std::string Information::getHardwareVersion() const {
    return hardware_version;
}

void Information::setHardwareVersion(const std::string& version) {
    validateNotEmpty(version, "Hardware version");
    hardware_version = version;
}

std::string Information::getSoftwareVersion() const {
    return software_version;
}

void Information::setSoftwareVersion(const std::string& version) {
    validateNotEmpty(version, "Software version");
    software_version = version;
}

std::string Information::getFpgaVersion() const {
    return fpga_version;
}

void Information::setFpgaVersion(const std::string& version) {
    validateNotEmpty(version, "FPGA version");
    fpga_version = version;
}

std::string Information::getModel() const {
    return model;
}

void Information::setModel(const std::string& model) {
    validateNotEmpty(model, "Model");
    this->model = model;
}

std::string Information::getSerialNumber() const {
    return serial_number;
}

void Information::setSerialNumber(const std::string& serialNumber) {
    validateNotEmpty(serialNumber, "Serial number");
    serial_number = serialNumber;
}

std::string Information::getMacAddress() const {
    return macAddress;
}

void Information::setMacAddress(const std::string& macAddress) {
    validateNotEmpty(macAddress, "MAC address");
    this->macAddress = macAddress;
}

std::string Information::getHomePath() const {
    return home;
}

void Information::setHomePath(const std::string& path) {
    validatePath(path, "Home path");
    home = path;
}

std::string Information::getLoginPath() const {
    return login;
}

void Information::setLoginPath(const std::string& path) {
    validatePath(path, "Login path");
    login = path;
}

std::string Information::getSettingsPath() const {
    return settings;
}

void Information::setSettingsPath(const std::string& path) {
    validatePath(path, "Settings path");
    settings = path;
}

std::string Information::getJsonPath() const {
    return json;
}

void Information::setJsonPath(const std::string& path) {
    validatePath(path, "JSON path");
    json = path;
}

std::string Information::getLogfile1Path() const {
    return logfile1;
}

void Information::setLogfile1Path(const std::string& path) {
    validatePath(path, "Logfile1 path");
    logfile1 = path;
}

std::string Information::getLogfile2Path() const {
    return logfile2;
}

void Information::setLogfile2Path(const std::string& path) {
    validatePath(path, "Logfile2 path");
    logfile2 = path;
}

std::string Information::getLogfile3Path() const {
    return logfile3;
}

void Information::setLogfile3Path(const std::string& path) {
    validatePath(path, "Logfile3 path");
    logfile3 = path;
}
