#include "DummyDataGenerator.h"
#include <algorithm>
#include <cctype>
#include <random>
#include <sstream>
#include <vector>

namespace {

std::mt19937& Rng() {
    static std::random_device rd;
    static std::mt19937 rng(rd());
    return rng;
}

const std::vector<std::string> kFirstNames = {
    "Alice", "Bob", "Charlie", "Diana", "Eve", "Frank",
    "Grace", "Hank", "Iris",   "Jack",  "Karen", "Leo",
    "Maria", "Nathan", "Olivia", "Peter", "Quinn", "Rachel",
    "Steve", "Tina"
};

const std::vector<std::string> kLastNames = {
    "Smith", "Johnson", "Williams", "Brown",  "Jones",   "Garcia",
    "Miller", "Davis",  "Wilson",   "Taylor", "Anderson","Thomas",
    "Jackson", "White", "Harris",   "Martin"
};

const std::vector<std::string> kDomains = {
    "gmail.com", "yahoo.com", "hotmail.com", "outlook.com", "example.com"
};

template<typename T>
const T& Pick(const std::vector<T>& v) {
    std::uniform_int_distribution<size_t> dist(0, v.size() - 1);
    return v[dist(Rng())];
}

} // namespace

std::string DummyDataGenerator::RandomName() {
    return Pick(kFirstNames) + " " + Pick(kLastNames);
}

std::string DummyDataGenerator::RandomEmail(const std::string& name) {
    std::string local = name;
    std::transform(local.begin(), local.end(), local.begin(), ::tolower);
    std::replace(local.begin(), local.end(), ' ', '.');
    return local + "@" + Pick(kDomains);
}

int DummyDataGenerator::RandomAge() {
    std::uniform_int_distribution<int> dist(18, 65);
    return dist(Rng());
}

std::string DummyDataGenerator::RandomPhone() {
    std::uniform_int_distribution<int> d(0, 9);
    std::ostringstream oss;
    oss << "010-";
    for (int i = 0; i < 4; ++i) oss << d(Rng());
    oss << "-";
    for (int i = 0; i < 4; ++i) oss << d(Rng());
    return oss.str();
}

RecordSet DummyDataGenerator::Generate(int count) {
    RecordSet result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
        std::string name = RandomName();
        Record rec;
        rec["id"]    = std::to_string(i + 1);
        rec["name"]  = name;
        rec["email"] = RandomEmail(name);
        rec["age"]   = std::to_string(RandomAge());
        rec["phone"] = RandomPhone();
        result.push_back(std::move(rec));
    }
    return result;
}
