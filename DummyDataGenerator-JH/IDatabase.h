#pragma once
#include "DataRecord.h"
#include <string>

class IDatabase {
public:
    virtual ~IDatabase() = default;

    virtual bool      Connect(const std::string& connectionString) = 0;
    virtual void      Disconnect()                                 = 0;
    virtual bool      Insert(const std::string& tableName,
                             const Record& record)                 = 0;
    virtual RecordSet QueryAll(const std::string& tableName)       = 0;
    virtual int       Count(const std::string& tableName)          = 0;
};
