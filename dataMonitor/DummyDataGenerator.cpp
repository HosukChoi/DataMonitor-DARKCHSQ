#include "DummyDataGenerator.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <format>

static const std::vector<std::string> SENSOR_TAGS = {
    "TEMP_SENSOR_01", "TEMP_SENSOR_02", "TEMP_SENSOR_03", "TEMP_SENSOR_04", "TEMP_SENSOR_05",
    "HUMIDITY_SENSOR_01", "HUMIDITY_SENSOR_02",
    "FLOW_SENSOR_01", "FLOW_SENSOR_02",
    "PRESSURE_SENSOR_01", "PRESSURE_SENSOR_02", "PRESSURE_SENSOR_03"
};

// Status thresholds per sensor type
struct Threshold { double warn; double error; };
static Threshold getThreshold(const std::string& tag) {
    if (tag.starts_with("TEMP"))     return { 85.0, 100.0 };
    if (tag.starts_with("HUMIDITY")) return { 80.0,  90.0 };
    if (tag.starts_with("FLOW"))     return { 75.0,  90.0 };
    if (tag.starts_with("PRESSURE")) return { 200.0, 220.0 };
    return { 80.0, 100.0 };
}

// Value range per sensor type
struct Range { double min; double max; };
static Range getRange(const std::string& tag) {
    if (tag.starts_with("TEMP"))     return { -10.0, 120.0 };
    if (tag.starts_with("HUMIDITY")) return {   0.0, 100.0 };
    if (tag.starts_with("FLOW"))     return {   0.0, 100.0 };
    if (tag.starts_with("PRESSURE")) return {  10.0, 250.0 };
    return { 0.0, 100.0 };
}

DummyDataGenerator::DummyDataGenerator() : rng_(std::random_device{}()) {}

std::vector<SensorRecord> DummyDataGenerator::generate(int count) {
    std::uniform_int_distribution<int> tagPicker(0, static_cast<int>(SENSOR_TAGS.size()) - 1);

    std::vector<SensorRecord> records;
    records.reserve(count);

    // Space records evenly over the past 24 hours (in minutes)
    const int totalMinutes = 24 * 60;
    const int intervalMinutes = totalMinutes / count;

    for (int i = 0; i < count; ++i) {
        SensorRecord rec;
        rec.id = generateUUID();
        rec.timestamp = generateTimestamp(-(totalMinutes - i * intervalMinutes));
        rec.tag = SENSOR_TAGS[tagPicker(rng_)];
        rec.value = generateValue(rec.tag);
        rec.status = determineStatus(rec.tag, rec.value);
        records.push_back(std::move(rec));
    }
    return records;
}

std::string DummyDataGenerator::generateUUID() {
    std::uniform_int_distribution<uint32_t> dis32(0, 0xFFFFFFFF);
    std::uniform_int_distribution<uint16_t> dis16(0, 0xFFFF);

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    oss << std::setw(8) << dis32(rng_) << "-";
    oss << std::setw(4) << dis16(rng_) << "-";
    oss << std::setw(4) << ((dis16(rng_) & 0x0FFF) | 0x4000) << "-";
    oss << std::setw(4) << ((dis16(rng_) & 0x3FFF) | 0x8000) << "-";
    oss << std::setw(8) << dis32(rng_);
    oss << std::setw(4) << dis16(rng_);
    return oss.str();
}

std::string DummyDataGenerator::generateTimestamp(int offsetMinutes) {
    using namespace std::chrono;
    auto now = system_clock::now() + minutes(offsetMinutes);
    auto tt = system_clock::to_time_t(now);
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &tt);
#else
    gmtime_r(&tt, &utc);
#endif
    return std::format("{:04d}-{:02d}-{:02d}T{:02d}:{:02d}:{:02d}Z",
        utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
        utc.tm_hour, utc.tm_min, utc.tm_sec);
}

double DummyDataGenerator::generateValue(const std::string& tag) {
    auto [minV, maxV] = getRange(tag);
    std::uniform_real_distribution<double> dist(minV, maxV);
    double raw = dist(rng_);
    // Round to 2 decimal places
    return std::round(raw * 100.0) / 100.0;
}

std::string DummyDataGenerator::determineStatus(const std::string& tag, double value) {
    auto [warn, error] = getThreshold(tag);
    if (value >= error) return "ERROR";
    if (value >= warn)  return "WARN";
    return "OK";
}
