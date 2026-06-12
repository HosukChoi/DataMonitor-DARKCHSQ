#include "JsonWriter.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <format>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <stdexcept>

namespace fs = std::filesystem;

static std::string escapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else           out += c;
    }
    return out;
}

void JsonWriter::write(const std::vector<SensorRecord>& records, const std::string& outputDir) {
    fs::create_directories(outputDir);
    std::string path = outputDir + "/" + generateFilename();
    std::ofstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open output file: " + path);
    file << buildJson(records);
    file.close();
}

std::string JsonWriter::buildJson(const std::vector<SensorRecord>& records) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    oss << "{\n";
    oss << "  \"metadata\": {\n";
    oss << "    \"generated_at\": \"" << currentTimestampISO() << "\",\n";
    oss << "    \"record_count\": " << records.size() << ",\n";
    oss << "    \"schema_version\": \"1.0\"\n";
    oss << "  },\n";
    oss << "  \"records\": [\n";

    for (size_t i = 0; i < records.size(); ++i) {
        const auto& r = records[i];
        oss << "    {\n";
        oss << "      \"id\": \""        << escapeJson(r.id)        << "\",\n";
        oss << "      \"timestamp\": \"" << escapeJson(r.timestamp) << "\",\n";
        oss << "      \"value\": "       << r.value                 << ",\n";
        oss << "      \"tag\": \""       << escapeJson(r.tag)       << "\",\n";
        oss << "      \"status\": \""    << escapeJson(r.status)    << "\"\n";
        oss << "    }";
        if (i + 1 < records.size()) oss << ",";
        oss << "\n";
    }

    oss << "  ]\n";
    oss << "}\n";
    return oss.str();
}

std::string JsonWriter::currentTimestampISO() {
    using namespace std::chrono;
    auto now = system_clock::now();
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

std::string JsonWriter::generateFilename() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto tt = system_clock::to_time_t(now);
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &tt);
#else
    gmtime_r(&tt, &utc);
#endif
    return std::format("dummy_{:04d}{:02d}{:02d}_{:02d}{:02d}{:02d}.json",
        utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
        utc.tm_hour, utc.tm_min, utc.tm_sec);
}
