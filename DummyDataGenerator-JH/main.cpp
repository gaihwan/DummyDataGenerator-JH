#include <iostream>
#include <string>

// ================================================================
//  DEBUG : gmock/gtest 테스트 실행
// ================================================================
#ifdef _DEBUG

#include <gmock/gmock.h>
#include "Tests.h"

int main(int argc, char** argv) {
    std::cout << "================================================\n";
    std::cout << "  DummyDataGenerator PoC  [DEBUG / TEST MODE]\n";
    std::cout << "================================================\n\n";
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

// ================================================================
//  RELEASE : CLI UI
// ================================================================
#else

#include <iomanip>
#include "DummyDataGenerator.h"
#include "InMemoryDatabase.h"
#include "DummyDataService.h"

static void PrintHeader() {
    std::cout
        << std::left
        << std::setw(5)  << "ID"
        << std::setw(22) << "Name"
        << std::setw(35) << "Email"
        << std::setw(6)  << "Age"
        << "Phone"
        << "\n"
        << std::string(85, '-') << "\n";
}

static void PrintRecord(const Record& r) {
    std::cout
        << std::left
        << std::setw(5)  << r.at("id")
        << std::setw(22) << r.at("name")
        << std::setw(35) << r.at("email")
        << std::setw(6)  << r.at("age")
        << r.at("phone")
        << "\n";
}

int main() {
    std::cout << "================================================\n";
    std::cout << "  DummyDataGenerator PoC  [RELEASE / CLI MODE]\n";
    std::cout << "================================================\n";

    InMemoryDatabase   db;
    DummyDataGenerator gen;
    DummyDataService   svc(&db, &gen);

    const std::string TABLE = "users";
    svc.Initialize("memory://localhost/testdb");

    while (true) {
        std::cout << "\n--- Menu ---\n";
        std::cout << " 1. Generate & store dummy data\n";
        std::cout << " 2. View all stored records\n";
        std::cout << " 3. Clear all records\n";
        std::cout << " 4. Exit\n";
        std::cout << "Select > ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "[!] Invalid input.\n";
            continue;
        }

        if (choice == 1) {
            int count = 0;
            std::cout << "How many records to generate? > ";
            if (!(std::cin >> count) || count <= 0) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "[!] Enter a positive integer.\n";
                continue;
            }
            int inserted = svc.GenerateAndStore(TABLE, count);
            std::cout << "[+] " << inserted << " record(s) stored to '"
                      << TABLE << "'.\n";

        } else if (choice == 2) {
            RecordSet records = svc.FetchAll(TABLE);
            if (records.empty()) {
                std::cout << "[!] No records found. Generate some first.\n";
            } else {
                std::cout << "\n";
                PrintHeader();
                for (const auto& r : records)
                    PrintRecord(r);
                std::cout << "\nTotal: " << records.size() << " record(s).\n";
            }

        } else if (choice == 3) {
            // 재연결로 초기화
            svc.Shutdown();
            svc.Initialize("memory://localhost/testdb");
            std::cout << "[+] All records cleared.\n";

        } else if (choice == 4) {
            break;

        } else {
            std::cout << "[!] Invalid option.\n";
        }
    }

    svc.Shutdown();
    std::cout << "\nGoodbye!\n";
    return 0;
}

#endif
