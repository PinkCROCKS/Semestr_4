#include <gtest/gtest.h>

#include "../include/functions.h"

namespace my_container {
    class DequeTest : public ::testing::Test {
    protected:
        Deque<int> empty_deque;
        Deque<int> int_deque{1, 2, 3, 4, 5};
        Deque<std::string> str_deque{"a", "bb", "ccc"};
    };

    TEST_F(DequeTest, DefaultConstructor) {
        EXPECT_TRUE(empty_deque.empty());
        EXPECT_EQ(empty_deque.size(), 0);
    }

    TEST_F(DequeTest, InitializerListConstructor) {
        Deque<int> d{10, 20, 30};
        EXPECT_EQ(d.size(), 3);
        EXPECT_EQ(d[0], 10);
        EXPECT_EQ(d[2], 30);
    }

    TEST_F(DequeTest, CopyConstructor) {
        Deque<int> copy(int_deque);
        EXPECT_EQ(copy.size(), 5);
        EXPECT_EQ(copy[4], 5);
    }

    TEST_F(DequeTest, MoveConstructor) {
        Deque<int> moved(std::move(int_deque));
        EXPECT_EQ(moved.size(), 5);
        EXPECT_TRUE(int_deque.empty());
    }

    TEST_F(DequeTest, CopyAssignment) {
        Deque<int> copy;
        copy = int_deque;
        EXPECT_EQ(copy.back(), 5);
    }

    TEST_F(DequeTest, MoveAssignment) {
        Deque<int> moved;
        moved = std::move(int_deque);
        EXPECT_EQ(moved.front(), 1);
        EXPECT_TRUE(int_deque.empty());
    }

    TEST_F(DequeTest, AtMethod) {
        EXPECT_EQ(int_deque.at(2), 3);
        EXPECT_THROW(empty_deque.at(0), std::out_of_range);
    }

    TEST_F(DequeTest, OperatorSubscript) {
        int_deque[2] = 30;
        EXPECT_EQ(int_deque[2], 30);
    }

    TEST_F(DequeTest, FrontBackMethods) {
        EXPECT_EQ(int_deque.front(), 1);
        EXPECT_EQ(int_deque.back(), 5);
    }

    TEST_F(DequeTest, Iterators) {
        auto it = int_deque.begin();
        EXPECT_EQ(*it, 1);
        EXPECT_EQ(*(++it), 2);

        auto rit = int_deque.rbegin();
        EXPECT_EQ(*rit, 5);
        EXPECT_EQ(*(++rit), 4);
    }

    TEST_F(DequeTest, PushPopOperations) {
        int_deque.push_back(6);
        EXPECT_EQ(int_deque.back(), 6);

        int_deque.push_front(0);
        EXPECT_EQ(int_deque.front(), 0);

        int_deque.pop_back();
        EXPECT_EQ(int_deque.back(), 5);

        int_deque.pop_front();
        EXPECT_EQ(int_deque.front(), 1);
    }

    TEST_F(DequeTest, InsertErase) {
        auto it = int_deque.insert(++int_deque.begin(), 9);
        EXPECT_EQ(int_deque[1], 9);

        int_deque.erase(it);
        EXPECT_EQ(int_deque[2], 3);
    }

    TEST_F(DequeTest, Resize) {
        int_deque.resize(3);
        EXPECT_EQ(int_deque.size(), 3);

        int_deque.resize(5, 10);
        EXPECT_EQ(int_deque[4], 10);
    }

    TEST_F(DequeTest, Comparisons) {
        Deque<int> d1{1, 2, 3};
        Deque<int> d2{1, 2, 4};

        EXPECT_TRUE(d1 < d2);
        EXPECT_FALSE(d1 == d2);
        EXPECT_TRUE(d1 <=> d2 == std::strong_ordering::less);
    }

    TEST_F(DequeTest, Destructor) {
        auto *deque = new Deque<int>{1, 2, 3};
        delete deque;
    }
}