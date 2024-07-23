#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <json-c/json.h>
#include <string>
#include <fstream>
#include <iostream>
#include "Information.h"
#include "Configuration.h"


class JSONParser {
public:
    JSONParser();
    ~JSONParser();

    Information parseInformation(const std::string& filename);
    Configuration parseConfiguration(const std::string& filename);

    void updateInformation(const Information& info, const std::string& filename);
    void updateConfiguration(const Configuration& config, const std::string& filename);

private:
    struct json_object *loadFile(const std::string& filename);
    std::string getStringValue(struct json_object* parsed_json, const std::string& key);
    std::string getNestedStringValue(struct json_object* parsed_json, const std::string& objectKey, const std::string& key);
    void setNestedStringValue(struct json_object* parsed_json, const std::string& objectKey, const std::string& key, const std::string& value);
};

#endif 
