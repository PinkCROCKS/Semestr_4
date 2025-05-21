#include <gtest/gtest.h>

//#include "../include/bigint.hpp"
#include "../src/functions.cpp"

TEST(BigIntTests, Constructors) {
    BigInt a{0};
    EXPECT_EQ(a, BigInt(0));
    BigInt b("1234567890");
    EXPECT_EQ(b, BigInt(1234567890));
    BigInt c("-987654321");
    EXPECT_EQ(c, BigInt(-987654321));
    BigInt r("");
    EXPECT_EQ(r, BigInt(0));
    BigInt d("000123");
    EXPECT_EQ(d, BigInt(123));
    BigInt e("-000456");
    EXPECT_EQ(e, BigInt(-456));
    EXPECT_THROW(BigInt("12a34"), std::invalid_argument);
    EXPECT_THROW(BigInt("--123"), std::invalid_argument);
}


TEST(BigIntTests, AssignmentOperators) {
    BigInt a(123);
    BigInt b = a;
    EXPECT_EQ(a, b);

    BigInt c(-456);
    b = std::move(c);
    EXPECT_EQ(b, BigInt(-456));
    EXPECT_TRUE(c.empty());
}

TEST(BigIntTests, ArithmeticOperations) {
    BigInt a(123);
    BigInt b(456);
    BigInt u(10000000000000000);
    BigInt temp(99999);
    temp += a;
    EXPECT_EQ(BigInt(100122), temp);
    EXPECT_EQ(a + b, BigInt(579));
    BigInt minus(-1);
    BigInt y(12);
    y -= minus;
    EXPECT_EQ(y, BigInt(13));
    minus -= y;
    EXPECT_EQ(minus, BigInt(-14));
    EXPECT_EQ(y -= BigInt(13), BigInt(0));
    BigInt c("-100000000");
    EXPECT_EQ(c + a, BigInt(-99999877));
    EXPECT_EQ(b - a, BigInt(333));
    EXPECT_EQ(a - BigInt(50), BigInt(73));
    a += BigInt(10000000000000000);
    u += BigInt(123);
    EXPECT_EQ(a, BigInt(10000000000000123));
    EXPECT_EQ(u, BigInt(10000000000000123));

    BigInt d(100000);
    EXPECT_EQ(d * BigInt(2), BigInt(200000));

    BigInt e(1000);
    EXPECT_EQ(e / BigInt(3), BigInt(333));
    EXPECT_EQ(e / BigInt(100002), BigInt(0));
    EXPECT_THROW(e / BigInt(0), std::invalid_argument);
}

TEST(BigIntTests, ComparisonOperators) {
    BigInt a(100);
    BigInt b(200);
    BigInt c(-100);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(c < a);
    EXPECT_TRUE(BigInt(0) > c);
    EXPECT_TRUE(a == BigInt(100));
    EXPECT_TRUE(b != a);
}

TEST(BigIntTests, EdgeCases) {
    // Максимальное значение
    BigInt maxVal("99999999999999999999");
    EXPECT_EQ(maxVal.size(), 4); // Для base=100000

    // Ноль
    BigInt zero;
    EXPECT_FALSE(zero.is_negative());

    // Смена знака
    BigInt a(123);
    BigInt b = -a;
    EXPECT_EQ(b, BigInt(-123));
}

TEST(BigIntTests, SpecialOperations) {
    BigInt a(1000);
    BigInt b(3);
    BigInt quotient = a / b;
    BigInt remainder = a - (quotient * b);
    EXPECT_EQ(remainder, BigInt(1));
    BigInt c("1000000000");
    BigInt d("2000000000");
    EXPECT_EQ(c * d, BigInt("2000000000000000000"));
}

TEST(BigIntTests, StringConversion) {
    BigInt a("12345678901234567890");
    std::stringstream ss;
    ss << a;
    EXPECT_EQ(ss.str(), "12345678901234567890");
    BigInt b("-12");
    std::stringstream sps;
    sps << b;
    EXPECT_EQ(sps.str(), "-12");
}

TEST(BigIntTests, FromStringTest){
    BigInt a{1234};
    BigInt c{-1234};
    BigInt b;
    std::stringstream ss;
    std::stringstream ss2;
    ss << "1234";
    ss >> b;
    ss2 << "-1234";
    EXPECT_EQ(a, b);
    ss2 >> b;
    EXPECT_EQ(c, b);
}

TEST(BigIntTests, AbsAndSign) {
    BigInt a(-123);
    EXPECT_EQ(a.abs(), BigInt(123));
    EXPECT_TRUE(a.is_negative());

    BigInt b(456);
    EXPECT_FALSE(b.abs().is_negative());
}

TEST(BigIntTests, ZeroHandling) {
    BigInt a(0);
    BigInt b(0);
    EXPECT_EQ(a + b, BigInt(0));
    EXPECT_EQ(a * b, BigInt(0));
    EXPECT_THROW(a / b, std::invalid_argument);
}

TEST(ModExpTest, TestingFunction){
    BigInt first("100");
    BigInt b = mod_exp(first, BigInt(2), BigInt(1));
    BigInt c = mod_exp(first, BigInt(2), BigInt(3));
    BigInt d = mod_exp(-first, BigInt(2), BigInt(6));
    EXPECT_EQ(b, BigInt(0));
    EXPECT_EQ(c, BigInt(1));
    EXPECT_EQ(d, BigInt(4));
    EXPECT_THROW(mod_exp(first, BigInt(-7), BigInt(1)), std::invalid_argument);
}

TEST(FftTest, TestComparing){
    BigInt a("19238120480129840129301230912830912804981290849012840981209481204");
    BigInt b("12049312809412049122390503280923804832084032809483209480932840923");
    EXPECT_EQ(BigInt::multiply_fft(a, b), a * b);
}

TEST(KaratsubaTest, TestComparing){
    BigInt a("19238120480129840129301230912830912804981290849012840981209481204");
    BigInt b("12049312809412049122390503280923804832084032809483209480932840923");
    EXPECT_EQ(BigInt::karatsuba_multiply(a, b), a * b);
}

int main(){
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}