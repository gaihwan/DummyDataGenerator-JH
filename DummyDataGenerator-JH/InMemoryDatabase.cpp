#include "InMemoryDatabase.h"
#include <iostream>

bool InMemoryDatabase::Connect(const std::string& connectionString) {
    std::cout << "[DB] Connected  : " << connectionString << std::endl;
    connected_ = true;
    return true;
}

void InMemoryDatabase::Disconnect() {
    std::cout << "[DB] Disconnected." << std::endl;
    connected_ = false;
    tables_.clear();
}

bool InMemoryDatabase::Insert(const std::string& tableName, const Record& record) {
    if (!connected_) return false;
    tables_[tableName].push_back(record);
    return true;
}

RecordSet InMemoryDatabase::QueryAll(const std::string& tableName) {
    if (!connected_) return {};
    auto it = tables_.find(tableName);
    return (it != tables_.end()) ? it->second : RecordSet{};
}

int InMemoryDatabase::Count(const std::string& tableName) {
    auto it = tables_.find(tableName);
    return (it != tables_.end()) ? static_cast<int>(it->second.size()) : 0;
}
