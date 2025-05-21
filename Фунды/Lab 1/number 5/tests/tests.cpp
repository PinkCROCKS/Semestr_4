#include <gtest/gtest.h>
#include <stdexcept>
#include "vector.hpp"

using namespace my_container;

class VectorTest : public ::testing::Test {
protected:
    Vector<int> empty_vec;
    Vector<int> vec_int;
    Vector<std::string> vec_str;
    void SetUp() override {
        vec_int = {1, 2, 3};
        vec_str = {"a", "b", "c"};
    }

};

// Тесты конструкторов
TEST_F(VectorTest, Constructors) {
    // Конструктор по умолчанию
    EXPECT_EQ(empty_vec.size(), 0);
    EXPECT_EQ(empty_vec.capacity(), 2);

    // Конструктор из initializer_list
    EXPECT_EQ(vec_int.size(), 3);
    EXPECT_EQ(vec_int[0], 1);
    EXPECT_EQ(vec_int[2], 3);

    // Конструктор копирования
    Vector<int> copy(vec_int);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy[1], 2);
}

// Тесты операторов присваивания
TEST_F(VectorTest, AssignmentOperators) {
    Vector<int> copy;
    copy = vec_int;
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy[2], 3);
}

// Тесты операторов сравнения
TEST_F(VectorTest, ComparisonOperators) {
    Vector<int> v1 = {1, 2, 3};
    Vector<int> v2 = {1, 2, 4};
    Vector<int> v3 = {1, 2};

    EXPECT_EQ(v1, vec_int);
    EXPECT_NE(v1, v2);
    EXPECT_LT(v3, v1);
    EXPECT_LE(v3, v1);
    EXPECT_GT(v2, v1);
    EXPECT_GE(v2, v1);
}

// Тесты методов доступа
TEST_F(VectorTest, AccessMethods) {
    EXPECT_EQ(vec_int[0], 1);

    // at()
    EXPECT_EQ(vec_str.at(1), "b");
    EXPECT_THROW(vec_str.at(5), std::out_of_range);

    // front/back
    EXPECT_EQ(vec_int.front(), 1);
    EXPECT_EQ(vec_int.back(), 3);
}

// Тесты управления памятью
TEST_F(VectorTest, MemoryManagement) {
    // reserve
    empty_vec.reserve(10);
    EXPECT_GE(empty_vec.capacity(), 10);

    vec_int.shrink_to_fit();
    EXPECT_EQ(vec_int.capacity(), vec_int.size());

    // max_size
    EXPECT_GE(vec_int.max_size(), vec_int.size());
}

// Тесты модификации контейнера
TEST_F(VectorTest, ModificationMethods) {
    // push_back/pop_back
    vec_int.push_back(4);
    EXPECT_EQ(vec_int.size(), 4);
    vec_int.pop_back();
    EXPECT_EQ(vec_int.size(), 3);

    // insert
    vec_int.insert(1, 5);
    EXPECT_EQ(vec_int[1], 5);
    EXPECT_EQ(vec_int.size(), 4);

    // erase
    vec_int.erase(1);
    EXPECT_EQ(vec_int[1], 2);
    EXPECT_EQ(vec_int.size(), 3);

    // clear
    vec_int.clear();
    EXPECT_TRUE(vec_int.empty());
}

// Тесты исключений
TEST_F(VectorTest, Exceptions) {
    // at() с неверным индексом
    EXPECT_THROW(empty_vec.at(0), std::out_of_range);

    // erase неверного индекса
    EXPECT_THROW(empty_vec.erase(0), std::range_error);

    // front/back пустого вектора
    EXPECT_THROW(empty_vec.front(), std::out_of_range);
    EXPECT_THROW(empty_vec.back(), std::out_of_range);
}

// Тесты swap
TEST_F(VectorTest, Swap) {
    Vector<int> v1 = {1, 2};
    Vector<int> v2 = {3, 4, 5};

    v1.swap(v2);

    EXPECT_EQ(v1.size(), 3);
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v1[2], 5);
    EXPECT_EQ(v2[1], 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}