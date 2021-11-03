#include <catch.hpp>

#include <sstream>
#include <random>
#include <iostream>

#include "../parser.h"
#include "../error.h"

auto ReadFull(const std::string& str) {
    std::stringstream ss{str};
    Tokenizer tokenizer{&ss};

    auto obj = Read(&tokenizer);
    REQUIRE(tokenizer.IsEnd());
    return obj;
}

TEST_CASE("Read number") {
    auto node = ReadFull("5");
    REQUIRE(Is<Number>(node));
    REQUIRE(As<Number>(node)->GetValue() == 5);

    node = ReadFull("-5");
    REQUIRE(Is<Number>(node));
    REQUIRE(As<Number>(node)->GetValue() == -5);
}

std::string RandomSymbol(std::default_random_engine* rng) {
    std::uniform_int_distribution<char> symbol('a', 'z');
    std::string s;
    for (int i = 0; i < 5; ++i) {
        s.push_back(symbol(*rng));
    }
    return s;
}

TEST_CASE("Read symbol") {
    SECTION("Plus") {
        auto node = ReadFull("+");
        REQUIRE(Is<Symbol>(node));
        REQUIRE(As<Symbol>(node)->GetName() == "+");
    }

    SECTION("Random symbol") {
        std::default_random_engine rng{42};
        for (int i = 0; i < 10; ++i) {
            auto name = RandomSymbol(&rng);
            auto node = ReadFull(name);
            REQUIRE(Is<Symbol>(node));
            REQUIRE(As<Symbol>(node)->GetName() == name);
        }
    }
}

TEST_CASE("Lists") {
    SECTION("Empty list") {
        auto null = ReadFull("()");
        REQUIRE(!null);
    }

    SECTION("Pair") {
        auto pair = ReadFull("(1 . 2)");
        REQUIRE(Is<Cell>(pair));

        auto first = As<Cell>(pair)->GetFirst();
        REQUIRE(Is<Number>(first));
        REQUIRE(As<Number>(first)->GetValue() == 1);

        auto second = As<Cell>(pair)->GetSecond();
        REQUIRE(Is<Number>(second));
        REQUIRE(As<Number>(second)->GetValue() == 2);
    }

    SECTION("Simple list") {
        auto list = ReadFull("(1 2)");
        REQUIRE(Is<Cell>(list));

        auto first = As<Cell>(list)->GetFirst();
        REQUIRE(Is<Number>(first));
        REQUIRE(As<Number>(first)->GetValue() == 1);

        list = As<Cell>(list)->GetSecond();
        auto second = As<Cell>(list)->GetFirst();
        REQUIRE(Is<Number>(second));
        REQUIRE(As<Number>(second)->GetValue() == 2);

        REQUIRE(!As<Cell>(list)->GetSecond());
    }

    SECTION("List with operator") {
        auto list = ReadFull("(+ 1 2)");
        REQUIRE(Is<Cell>(list));

        auto first = As<Cell>(list)->GetFirst();
        REQUIRE(Is<Symbol>(first));
        REQUIRE(As<Symbol>(first)->GetName() == "+");

        list = As<Cell>(list)->GetSecond();
        auto second = As<Cell>(list)->GetFirst();
        REQUIRE(Is<Number>(second));
        REQUIRE(As<Number>(second)->GetValue() == 1);

        list = As<Cell>(list)->GetSecond();
        second = As<Cell>(list)->GetFirst();
        REQUIRE(Is<Number>(second));
        REQUIRE(As<Number>(second)->GetValue() == 2);

        REQUIRE(!As<Cell>(list)->GetSecond());
    }

    SECTION("List with funny end") {
        auto list = ReadFull("(1 2 . 3)");

        REQUIRE(Is<Cell>(list));

        auto first = As<Cell>(list)->GetFirst();
        REQUIRE(Is<Number>(first));
        REQUIRE(As<Number>(first)->GetValue() == 1);

        list = As<Cell>(list)->GetSecond();
        auto second = As<Cell>(list)->GetFirst();
        REQUIRE(Is<Number>(second));
        REQUIRE(As<Number>(second)->GetValue() == 2);

        auto last = As<Cell>(list)->GetSecond();
        REQUIRE(Is<Number>(last));
        REQUIRE(As<Number>(last)->GetValue() == 3);
    }

    SECTION("Complex lists") {
        ReadFull("(1 . ())");
        ReadFull("(1 2 . ())");
        ReadFull("(1 . (2 . ()))");
        ReadFull("(1 2 (3 4) (()))");
        ReadFull("(+ 1 2 (- 3 4))");
    }

    SECTION("Invalid lists") {
        REQUIRE_THROWS_AS(ReadFull(""), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("("), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1"), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1 ."), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("( ."), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1 . ()"), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1 . )"), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1 . 2 3)"), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1))"), SyntaxError);
    }
}

constexpr uint32_t kShotsCount = 100000;
constexpr uint32_t kSeed = 16;
constexpr uint32_t kSeqCount = 21;

TEST_CASE("Fuzzzzzzing") {
    std::array<std::string, kSeqCount> sequences = {"(", "(",  "(", "(",  ")", ")",      ")",
                                                    ")", "-",  "+", "#t", "",  "symbol", "0",
                                                    "1", "-2", ".", ".",  ".", ".",      "'"};

    std::stringstream ss;
    std::mt19937 gen(kSeed);
    std::uniform_int_distribution<uint32_t> dist(1, kSeqCount);

    for (uint32_t i = 0; i < kShotsCount; ++i) {
        std::shuffle(sequences.begin(), sequences.end(), gen);

        uint32_t len = dist(gen);

        for (uint32_t j = 0; j < len; ++j) {
            ss << sequences[j] << " ";
        }

        try {
            // std::cerr << ss.str() << std::endl;  // uncomment to debug, random is deterministic
            ReadFull(ss.str());
        } catch (const SyntaxError&) {
        }

        ss.str("");
    }
}
