#pragma once
#include <gmock/gmock.h>
#include "IDatabase.h"
#include "IDummyDataGenerator.h"
#include "DummyDataService.h"
#include "DummyDataGenerator.h"

// ---------------------------------------------------------------
// Mock classes
// ---------------------------------------------------------------
class MockDatabase : public IDatabase {
public:
    MOCK_METHOD(bool,      Connect,  (const std::string&),               (override));
    MOCK_METHOD(void,      Disconnect, (),                               (override));
    MOCK_METHOD(bool,      Insert,   (const std::string&, const Record&),(override));
    MOCK_METHOD(RecordSet, QueryAll, (const std::string&),               (override));
    MOCK_METHOD(int,       Count,    (const std::string&),               (override));
};

class MockDummyDataGenerator : public IDummyDataGenerator {
public:
    MOCK_METHOD(RecordSet, Generate, (int), (override));
};

// ---------------------------------------------------------------
// DummyDataService tests
// ---------------------------------------------------------------
TEST(DummyDataServiceTest, InitializeCallsConnect) {
    MockDatabase db;
    MockDummyDataGenerator gen;
    EXPECT_CALL(db, Connect("test://localhost")).WillOnce(testing::Return(true));

    DummyDataService svc(&db, &gen);
    EXPECT_TRUE(svc.Initialize("test://localhost"));
}

TEST(DummyDataServiceTest, InitializeReturnsFalseOnFailure) {
    MockDatabase db;
    MockDummyDataGenerator gen;
    EXPECT_CALL(db, Connect(testing::_)).WillOnce(testing::Return(false));

    DummyDataService svc(&db, &gen);
    EXPECT_FALSE(svc.Initialize("bad://conn"));
}

TEST(DummyDataServiceTest, GenerateAndStoreInsertsAllRecords) {
    MockDatabase db;
    MockDummyDataGenerator gen;

    RecordSet fakeRecords = {
        {{"id","1"},{"name","Alice Smith"},{"email","alice.smith@gmail.com"},{"age","30"},{"phone","010-1234-5678"}},
        {{"id","2"},{"name","Bob Jones"},  {"email","bob.jones@yahoo.com"},  {"age","25"},{"phone","010-9876-5432"}},
    };

    EXPECT_CALL(db,  Connect(testing::_)).WillOnce(testing::Return(true));
    EXPECT_CALL(gen, Generate(2)).WillOnce(testing::Return(fakeRecords));
    EXPECT_CALL(db,  Insert("users", testing::_))
        .Times(2).WillRepeatedly(testing::Return(true));

    DummyDataService svc(&db, &gen);
    svc.Initialize("test://localhost");
    EXPECT_EQ(svc.GenerateAndStore("users", 2), 2);
}

TEST(DummyDataServiceTest, GenerateAndStoreCountsOnlySuccessfulInserts) {
    MockDatabase db;
    MockDummyDataGenerator gen;

    RecordSet fakeRecords = {
        {{"id","1"},{"name","A"}},
        {{"id","2"},{"name","B"}},
        {{"id","3"},{"name","C"}},
    };

    EXPECT_CALL(db,  Connect(testing::_)).WillOnce(testing::Return(true));
    EXPECT_CALL(gen, Generate(3)).WillOnce(testing::Return(fakeRecords));
    EXPECT_CALL(db,  Insert("users", testing::_))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false))   // second insert fails
        .WillOnce(testing::Return(true));

    DummyDataService svc(&db, &gen);
    svc.Initialize("test://localhost");
    EXPECT_EQ(svc.GenerateAndStore("users", 3), 2);
}

TEST(DummyDataServiceTest, FetchAllDelegatesToDatabase) {
    MockDatabase db;
    MockDummyDataGenerator gen;

    RecordSet expected = { {{"id","1"},{"name","Alice Smith"}} };
    EXPECT_CALL(db, Connect(testing::_)).WillOnce(testing::Return(true));
    EXPECT_CALL(db, QueryAll("users")).WillOnce(testing::Return(expected));

    DummyDataService svc(&db, &gen);
    svc.Initialize("test://localhost");
    RecordSet result = svc.FetchAll("users");
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0]["name"], "Alice Smith");
}

TEST(DummyDataServiceTest, ShutdownCallsDisconnect) {
    MockDatabase db;
    MockDummyDataGenerator gen;
    EXPECT_CALL(db, Connect(testing::_)).WillOnce(testing::Return(true));
    EXPECT_CALL(db, Disconnect()).Times(1);

    DummyDataService svc(&db, &gen);
    svc.Initialize("test://localhost");
    svc.Shutdown();
}

// ---------------------------------------------------------------
// DummyDataGenerator tests (real implementation)
// ---------------------------------------------------------------
TEST(DummyDataGeneratorTest, GeneratesExactCount) {
    DummyDataGenerator gen;
    EXPECT_EQ(gen.Generate(0).size(), 0u);
    EXPECT_EQ(gen.Generate(5).size(), 5u);
    EXPECT_EQ(gen.Generate(100).size(), 100u);
}

TEST(DummyDataGeneratorTest, EachRecordHasRequiredFields) {
    DummyDataGenerator gen;
    for (const auto& r : gen.Generate(10)) {
        EXPECT_FALSE(r.at("id").empty());
        EXPECT_FALSE(r.at("name").empty());
        EXPECT_FALSE(r.at("email").empty());
        EXPECT_FALSE(r.at("age").empty());
        EXPECT_FALSE(r.at("phone").empty());
    }
}

TEST(DummyDataGeneratorTest, EmailContainsAtSymbol) {
    DummyDataGenerator gen;
    for (const auto& r : gen.Generate(20)) {
        EXPECT_NE(r.at("email").find('@'), std::string::npos)
            << "email = " << r.at("email");
    }
}

TEST(DummyDataGeneratorTest, AgeIsInValidRange) {
    DummyDataGenerator gen;
    for (const auto& r : gen.Generate(50)) {
        int age = std::stoi(r.at("age"));
        EXPECT_GE(age, 18);
        EXPECT_LE(age, 65);
    }
}

TEST(DummyDataGeneratorTest, PhoneMatchesKoreanFormat) {
    DummyDataGenerator gen;
    for (const auto& r : gen.Generate(20)) {
        const std::string& phone = r.at("phone");
        // format: 010-XXXX-XXXX
        EXPECT_EQ(phone.substr(0, 4), "010-") << "phone = " << phone;
        EXPECT_EQ(phone.size(), 13u)           << "phone = " << phone;
    }
}

TEST(DummyDataGeneratorTest, IdsAreSequential) {
    DummyDataGenerator gen;
    RecordSet records = gen.Generate(5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(records[i]["id"], std::to_string(i + 1));
    }
}
