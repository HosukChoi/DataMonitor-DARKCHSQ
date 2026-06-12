#pragma once
#include <string>
#include <vector>
#include "DummyDataGenerator.h"

class JsonWriter {
public:
    void write(const std::vector<SensorRecord>& records, const std::string& outputDir = "output");

private:
    std::string buildJson(const std::vector<SensorRecord>& records);
    std::string currentTimestampISO();
    std::string generateFilename();
};
