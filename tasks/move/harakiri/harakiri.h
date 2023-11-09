#pragma once

#include <string>
#include <utility>

class OneTimeCallback {
public:
    virtual ~OneTimeCallback() = default;

    std::string operator()() const&& {
        return Run();
    }
    OneTimeCallback& operator=(const OneTimeCallback&) = delete;
    OneTimeCallback& operator=(OneTimeCallback&&) = delete;

private:
    virtual std::string Run() const = 0;
};

class AwesomeCallback : public OneTimeCallback {
public:
    explicit AwesomeCallback(std::string str) : str_(std::move(str)) {
    }

private:
    std::string Run() const override {
        auto ans = str_ + "awesomeness";
        delete this;
        return ans;
    }

    std::string str_;
};
