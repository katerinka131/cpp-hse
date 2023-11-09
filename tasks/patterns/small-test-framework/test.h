#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>

class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() {
    }
};

class Substr {
public:
    explicit Substr(const std::string s) : sub_str_(s) {
    }

    bool operator()(const std::string test_name) {
        return test_name.find(sub_str_) != std::string::npos;
    }

private:
    std::string sub_str_;
};

class FullMatch {
public:
    explicit FullMatch(const std::string s) : match_str_(s) {
    }

    bool operator()(const std::string test_name) {
        return match_str_ == test_name;
    }

private:
    std::string match_str_;
};

class TestRegistry {
public:
    static inline TestRegistry& Instance() {
        static TestRegistry instance;
        return instance;
    }

    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        register_t_[class_name] = std::bind(&std::make_unique<TestClass>);
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        if (register_t_.find(class_name) != register_t_.end()) {
            return register_t_.find(class_name)->second();
        } else {
            throw std::out_of_range("Test class not registered");
        }
    }

    void RunTest(const std::string& test_name) {
        auto test = CreateTest(test_name);
        test->SetUp();
        try {
            test->Run();
        } catch (...) {
            test->TearDown();
            throw;
        }
        test->TearDown();
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> tests;
        const auto al = [](const std::string&) { return true; };
        tests = ShowTests(al);
        std::sort(tests.begin(), tests.end());
        return tests;
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> tests;
        for (auto& pair : register_t_) {
            if (callback(pair.first)) {
                tests.push_back(pair.first);
            }
        }
        return tests;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        std::vector<std::string> tests;
        tests = ShowTests(callback);
        for (auto& test : tests) {
            RunTest(test);
        }
    }
    void Clear() {
        register_t_.clear();
    }

private:
    std::map<std::string, std::function<std::unique_ptr<AbstractTest>()>> register_t_;
};
