#include <gtest/gtest.h>

#include "../include/functions.h"

namespace my_container {

    class ListTest : public ::testing::Test {
    protected:
        List<int> empty_int_list;
        List<int> int_list = {1, 2, 3, 4};
        List<std::string> str_list = {"one", "two", "three"};
    };

    template <typename T>
    void test_list_content(const List<T> &list, const std::vector<T> &expected) {
        ASSERT_EQ(list.size(), expected.size());
        auto it = list.begin();
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(*it, expected[i]);
            ++it;
        }
    }

    TEST_F(ListTest, DefaultConstructor) {
        EXPECT_TRUE(empty_int_list.empty());
        EXPECT_EQ(empty_int_list.size(), 0);
    }

    TEST_F(ListTest, InitializerListConstructor) {
        test_list_content(int_list, {1, 2, 3, 4});
        test_list_content(str_list, {"one", "two", "three"});
    }

    TEST_F(ListTest, CopyConstructor) {
        List<int> copy(int_list);
        test_list_content(copy, {1, 2, 3, 4});
        EXPECT_EQ(int_list.size(), copy.size());
    }

    TEST_F(ListTest, MoveConstructor) {
        List<int> moved(std::move(int_list));
        test_list_content(moved, {1, 2, 3, 4});
        EXPECT_TRUE(int_list.empty());
    }

    TEST_F(ListTest, AssignmentOperator) {
        List<int> copy;
        copy = int_list;
        test_list_content(copy, {1, 2, 3, 4});
    }

    TEST_F(ListTest, ComparisonOperators) {
        List<int> list1 = {1, 2, 3};
        List<int> list2 = {1, 2, 3};
        List<int> list3 = {1, 2};

        EXPECT_EQ(list1, list2);
        EXPECT_NE(list1, list3);
        EXPECT_GT(list1, list3);
        EXPECT_LT(list3, list1);
    }

    TEST_F(ListTest, Iterators) {
        auto it = int_list.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        auto rit = int_list.rbegin();
        EXPECT_EQ(*rit, 4);
    }

    TEST_F(ListTest, SizeMethods) {
        EXPECT_EQ(int_list.size(), 4);
        EXPECT_EQ(int_list.max_size(), 145674);
        EXPECT_FALSE(int_list.empty());
    }

    TEST_F(ListTest, PushPopMethods) {
        int_list.push_back(5);
        test_list_content(int_list, {1, 2, 3, 4, 5});

        int_list.pop_back();
        test_list_content(int_list, {1, 2, 3, 4});

        int_list.pop_front();
        test_list_content(int_list, {2, 3, 4});
    }

    TEST_F(ListTest, InsertErase) {
        auto it = int_list.begin();
        ++it;
        int_list.insert(it, 99);
        test_list_content(int_list, {1, 99, 2, 3, 4});

        int_list.erase(it);
        test_list_content(int_list, {1, 99, 3, 4});
    }

    TEST_F(ListTest, Exceptions) {
        EXPECT_THROW(empty_int_list.pop_back(), std::out_of_range);
        EXPECT_THROW(empty_int_list.pop_front(), std::out_of_range);
    }

    TEST_F(ListTest, IteratorOperations) {
        auto it1 = int_list.begin();
        auto it2 = int_list.begin();
        EXPECT_EQ(it1, it2);

        ++it1;
        EXPECT_NE(it1, it2);
    }

    TEST_F(ListTest, SwapMethod) {
        List<int> list1 = {1, 2, 3};
        List<int> list2 = {4, 5};

        list1.swap(list2);

        test_list_content(list1, {4, 5});
        test_list_content(list2, {1, 2, 3});
        EXPECT_EQ(list1.size(), 2);
        EXPECT_EQ(list2.size(), 3);

        list1.swap(list1);
        test_list_content(list1, {4, 5});
    }

    TEST_F(ListTest, FrontBack) {
        List<int> list1 = {1, 2, 3};
        EXPECT_EQ(list1.front(), 1);
        EXPECT_EQ(list1.back(), 3);
    }

}