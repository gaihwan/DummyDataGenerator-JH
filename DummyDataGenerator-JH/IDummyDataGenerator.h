#pragma once
#include "DataRecord.h"

class IDummyDataGenerator {
public:
    virtual ~IDummyDataGenerator() = default;
    virtual RecordSet Generate(int count) = 0;
};
