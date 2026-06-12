#pragma once
#include "IDummyDataGenerator.h"
#include <string>

class DummyDataGenerator : public IDummyDataGenerator {
public:
    RecordSet Generate(int count) override;

private:
    static std::string RandomName();
    static std::string RandomEmail(const std::string& name);
    static int         RandomAge();
    static std::string RandomPhone();
};
