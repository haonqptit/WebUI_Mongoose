#include "JSONParser.h"

JSONParser::JSONParser() {}

JSONParser::~JSONParser() {}


struct json_object* JSONParser::loadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    struct json_object* parsed_json = json_tokener_parse(buffer.c_str());
    if (parsed_json == nullptr) {
        throw std::runtime_error("Failed to parse JSON");
    }

    return parsed_json;
}

std::string JSONParser::getStringValue(struct json_object* parsed_json, const std::string& key) {
    struct json_object *value;
    if (json_object_object_get_ex(parsed_json, key.c_str(), &value)) {
        return json_object_get_string(value);
    }
    return "";
}

std::string JSONParser::getNestedStringValue(struct json_object* parsed_json, const std::string& objectKey, const std::string& key) {
    struct json_object *nested_object;
    if (json_object_object_get_ex(parsed_json, objectKey.c_str(), &nested_object)) {
        struct json_object *value;
        if (json_object_object_get_ex(nested_object, key.c_str(), &value)) {
            return json_object_get_string(value);
        }
    }
    return "";
}

void JSONParser::setNestedStringValue(struct json_object* parsed_json, const std::string& objectKey, const std::string& key, const std::string& value) {
    struct json_object *nested_object;
    if (json_object_object_get_ex(parsed_json, objectKey.c_str(), &nested_object)) {
        struct json_object *new_value = json_object_new_string(value.c_str());
        json_object_object_add(nested_object, key.c_str(), new_value);
    }
}

Information JSONParser::parseInformation(const std::string& filename) {
    Information info;
    struct json_object* parsed_json = loadFile(filename);

    info.setHardwareVersion(getNestedStringValue(parsed_json, "information", "hardware_version"));
    info.setSoftwareVersion(getNestedStringValue(parsed_json, "information", "software_version"));
    info.setFpgaVersion(getNestedStringValue(parsed_json, "information", "fpga_version"));
    info.setModel(getNestedStringValue(parsed_json, "information", "model"));
    info.setSerialNumber(getNestedStringValue(parsed_json, "information", "serial_number"));
    info.setMacAddress(getNestedStringValue(parsed_json, "information", "macAddress"));

    info.setHomePath(getNestedStringValue(parsed_json, "pathFile", "home"));
    info.setLoginPath(getNestedStringValue(parsed_json, "pathFile", "login"));
    info.setSettingsPath(getNestedStringValue(parsed_json, "pathFile", "settings"));
    info.setJsonPath(getNestedStringValue(parsed_json, "pathFile", "json"));
    info.setLogfile1Path(getNestedStringValue(parsed_json, "pathFile", "logfile1"));
    info.setLogfile2Path(getNestedStringValue(parsed_json, "pathFile", "logfile2"));
    info.setLogfile3Path(getNestedStringValue(parsed_json, "pathFile", "logfile3"));

    json_object_put(parsed_json);
    return info;
}

Configuration JSONParser::parseConfiguration(const std::string& filename) {
    Configuration config;
    struct json_object* parsed_json = loadFile(filename);

    config.setUsername(getNestedStringValue(parsed_json, "account_information", "username"));
    config.setPassword(getNestedStringValue(parsed_json, "account_information", "password"));

    config.setStaIpAddress(getNestedStringValue(parsed_json, "settingsSTA", "ip-address"));
    config.setStaLoggingLevel(getNestedStringValue(parsed_json, "settingsSTA", "logging-level"));
    config.setStaWirelessMode(getNestedStringValue(parsed_json, "settingsSTA", "wireless-mode"));
    config.setStaWirelessSsid(getNestedStringValue(parsed_json, "settingsSTA", "wireless-SSID"));
    config.setStaWirelessPassPhrase(getNestedStringValue(parsed_json, "settingsSTA", "wireless-Pass-Phrase"));
    config.setStaStatus(getNestedStringValue(parsed_json, "settingsSTA", "status"));
    config.setGateway(getNestedStringValue(parsed_json, "settingsSTA", "gateway"));
    config.setDns(getNestedStringValue(parsed_json, "settingsSTA", "dns"));

    config.setApIpAddress(getNestedStringValue(parsed_json, "settingsAP", "ip-address"));
    config.setApLoggingLevel(getNestedStringValue(parsed_json, "settingsAP", "logging-level"));
    config.setApWirelessMode(getNestedStringValue(parsed_json, "settingsAP", "wireless-mode"));
    config.setApWirelessSsid(getNestedStringValue(parsed_json, "settingsAP", "wireless-SSID"));
    config.setApWirelessPassPhrase(getNestedStringValue(parsed_json, "settingsAP", "wireless-Pass-Phrase"));
    config.setApStatus(getNestedStringValue(parsed_json, "settingsAP", "status"));

    json_object_put(parsed_json);
    return config;
}

void JSONParser::updateInformation(const Information& info, const std::string& filename) {
    struct json_object* parsed_json = loadFile(filename);

    setNestedStringValue(parsed_json, "information", "hardware_version", info.getHardwareVersion());
    setNestedStringValue(parsed_json, "information", "software_version", info.getSoftwareVersion());
    setNestedStringValue(parsed_json, "information", "fpga_version", info.getFpgaVersion());
    setNestedStringValue(parsed_json, "information", "model", info.getModel());
    setNestedStringValue(parsed_json, "information", "serial_number", info.getSerialNumber());
    setNestedStringValue(parsed_json, "information", "macAddress", info.getMacAddress());

    setNestedStringValue(parsed_json, "pathFile", "home", info.getHomePath());
    setNestedStringValue(parsed_json, "pathFile", "login", info.getLoginPath());
    setNestedStringValue(parsed_json, "pathFile", "settings", info.getSettingsPath());
    setNestedStringValue(parsed_json, "pathFile", "json", info.getJsonPath());
    setNestedStringValue(parsed_json, "pathFile", "logfile1", info.getLogfile1Path());
    setNestedStringValue(parsed_json, "pathFile", "logfile2", info.getLogfile2Path());
    setNestedStringValue(parsed_json, "pathFile", "logfile3", info.getLogfile3Path());

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    file << json_object_to_json_string_ext(parsed_json, JSON_C_TO_STRING_PRETTY);
    file.close();

    json_object_put(parsed_json);
}

void JSONParser::updateConfiguration(const Configuration& config, const std::string& filename) {
    struct json_object* parsed_json = loadFile(filename);

    setNestedStringValue(parsed_json, "account_information", "username", config.getUsername());
    setNestedStringValue(parsed_json, "account_information", "password", config.getPassword());

    setNestedStringValue(parsed_json, "settingsSTA", "ip-address", config.getStaIpAddress());
    setNestedStringValue(parsed_json, "settingsSTA", "auto", config.getStaAuto());
    setNestedStringValue(parsed_json, "settingsSTA", "logging-level", config.getStaLoggingLevel());
    setNestedStringValue(parsed_json, "settingsSTA", "wireless-mode", config.getStaWirelessMode());
    setNestedStringValue(parsed_json, "settingsSTA", "wireless-SSID", config.getStaWirelessSsid());
    setNestedStringValue(parsed_json, "settingsSTA", "wireless-Pass-Phrase", config.getStaWirelessPassPhrase());
    setNestedStringValue(parsed_json, "settingsSTA", "status", config.getStaStatus());

    setNestedStringValue(parsed_json, "settingsAP", "ip-address", config.getApIpAddress());
    setNestedStringValue(parsed_json, "settingsAP", "logging-level", config.getApLoggingLevel());
    setNestedStringValue(parsed_json, "settingsAP", "wireless-mode", config.getApWirelessMode());
    setNestedStringValue(parsed_json, "settingsAP", "wireless-SSID", config.getApWirelessSsid());
    setNestedStringValue(parsed_json, "settingsAP", "wireless-Pass-Phrase", config.getApWirelessPassPhrase());
    setNestedStringValue(parsed_json, "settingsAP", "status", config.getApStatus());

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    file << json_object_to_json_string_ext(parsed_json, JSON_C_TO_STRING_PRETTY);
    file.close();

    json_object_put(parsed_json);
}
