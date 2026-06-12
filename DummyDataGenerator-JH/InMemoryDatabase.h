#pragma once
#include "IDatabase.h"
#include <map>
#include <string>

class InMemoryDatabase : public IDatabase {
public:
    bool      Connect(const std::string& connectionString) override;
    void      Disconnect()                                 override;
    bool      Insert(const std::string& tableName,
                     const Record& record)                 override;
    RecordSet QueryAll(const std::string& tableName)       override;
    int       Count(const std::string& tableName)          override;

private:
    bool connected_ = false;
    std::map<std::string, RecordSet> tables_;
};
