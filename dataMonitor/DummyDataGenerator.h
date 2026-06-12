#pragma once
#include <string>
#include <vector>
#include <random>

struct SensorRecord {
    std::string id;
    std::string timestamp;
    double value;
    std::string tag;
    std::string status;
};

class DummyDataGenerator {
public:
    DummyDataGenerator();
    std::vector<SensorRecord> generate(int count = 100);

private:
    std::mt19937 rng_;

    std::string generateUUID();
    std::string generateTimestamp(int offsetMinutes);
    double generateValue(const std::string& tag);
    std::string determineStatus(const std::string& tag, double value);
};
