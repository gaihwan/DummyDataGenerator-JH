#include "DummyDataService.h"

DummyDataService::DummyDataService(IDatabase* db, IDummyDataGenerator* generator)
    : db_(db), generator_(generator) {}

bool DummyDataService::Initialize(const std::string& connectionString) {
    return db_->Connect(connectionString);
}

int DummyDataService::GenerateAndStore(const std::string& tableName, int count) {
    RecordSet records = generator_->Generate(count);
    int inserted = 0;
    for (const auto& record : records) {
        if (db_->Insert(tableName, record))
            ++inserted;
    }
    return inserted;
}

RecordSet DummyDataService::FetchAll(const std::string& tableName) {
    return db_->QueryAll(tableName);
}

void DummyDataService::Shutdown() {
    db_->Disconnect();
}
