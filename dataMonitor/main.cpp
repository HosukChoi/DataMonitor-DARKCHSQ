#include <iostream>
#include <stdexcept>
#include "DummyDataGenerator.h"
#include "JsonWriter.h"

int main() {
    try {
        DummyDataGenerator generator;
        auto records = generator.generate(100);

        JsonWriter writer;
        writer.write(records, "output");

        int ok = 0, warn = 0, error = 0;
        for (const auto& r : records) {
            if (r.status == "OK")    ++ok;
            else if (r.status == "WARN")  ++warn;
            else if (r.status == "ERROR") ++error;
        }

        std::cout << "Generated " << records.size() << " records\n";
        std::cout << "  OK: " << ok << "  WARN: " << warn << "  ERROR: " << error << "\n";
        std::cout << "Output written to output/\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
