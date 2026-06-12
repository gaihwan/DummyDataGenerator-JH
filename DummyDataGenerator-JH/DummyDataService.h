#pragma once
#include "IDatabase.h"
#include "IDummyDataGenerator.h"
#include <string>

class DummyDataService {
public:
    DummyDataService(IDatabase* db, IDummyDataGenerator* generator);

    bool      Initialize(const std::string& connectionString);
    int       GenerateAndStore(const std::string& tableName, int count);
    RecordSet FetchAll(const std::string& tableName);
    void      Shutdown();

private:
    IDatabase*           db_;
    IDummyDataGenerator* generator_;
};
