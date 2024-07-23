#ifndef INFORMATION_H
#define INFORMATION_H

#include <string>
#include <stdexcept>

class Information {
public:
    std::string getHardwareVersion() const;
    void setHardwareVersion(const std::string& version);

    std::string getSoftwareVersion() const;
    void setSoftwareVersion(const std::string& version);

    std::string getFpgaVersion() const;
    void setFpgaVersion(const std::string& version);

    std::string getModel() const;
    void setModel(const std::string& model);

    std::string getSerialNumber() const;
    void setSerialNumber(const std::string& serialNumber);

    std::string getMacAddress() const;
    void setMacAddress(const std::string& macAddress);

    std::string getHomePath() const;
    void setHomePath(const std::string& path);

    std::string getLoginPath() const;
    void setLoginPath(const std::string& path);

    std::string getSettingsPath() const;
    void setSettingsPath(const std::string& path);

    std::string getJsonPath() const;
    void setJsonPath(const std::string& path);

    std::string getLogfile1Path() const;
    void setLogfile1Path(const std::string& path);

    std::string getLogfile2Path() const;
    void setLogfile2Path(const std::string& path);

    std::string getLogfile3Path() const;
    void setLogfile3Path(const std::string& path);

private:
    std::string hardware_version;
    std::string software_version;
    std::string fpga_version;
    std::string model;
    std::string serial_number;
    std::string macAddress;

    std::string home;
    std::string login;
    std::string settings;
    std::string json;
    std::string logfile1;
    std::string logfile2;
    std::string logfile3;

    void validateNotEmpty(const std::string& value, const std::string& fieldName) const;
    void validatePath(const std::string& path, const std::string& fieldName) const;
};

#endif // INFORMATION_H
