#include "Stack.hpp"
#include <gtest/gtest.h>

TEST(StackTest, Constructors) {
    Stack<int> test_stack1;
    EXPECT_TRUE(test_stack1.empty());
    EXPECT_EQ(test_stack1.size(), 0);

    Stack<int> test_stack2{1, 2, 3};
    EXPECT_EQ(test_stack2.size(), 3);
    EXPECT_EQ(test_stack2.top(), 1);

    Stack<int> test_stack3(test_stack2);
    EXPECT_EQ(test_stack3, test_stack2);
    EXPECT_EQ(test_stack3.top(), 1);

    Stack<int> test_stack4(std::move(test_stack2));
    EXPECT_EQ(test_stack4.size(), 3);
    EXPECT_TRUE(test_stack2.empty());  
}

TEST(StackTest, AssignmentOperators) {
    Stack<int> test_stack1{1, 2, 3};
    Stack<int> test_stack2;

    test_stack2 = test_stack1;
    EXPECT_EQ(test_stack2, test_stack1);
    EXPECT_EQ(test_stack2.top(), 1);

    Stack<int> test_stack3;
    test_stack3 = std::move(test_stack1);
    EXPECT_EQ(test_stack3.size(), 3);
    EXPECT_TRUE(test_stack1.empty());  
}

TEST(StackTest, BasicOperations) {
    Stack<int> test_stack{1, 2};
    test_stack.push(10);
    EXPECT_EQ(test_stack.top(), 10);
    test_stack.push(20);
    EXPECT_EQ(test_stack.top(), 20);
    EXPECT_FALSE(test_stack.empty());
    test_stack.pop();
    EXPECT_EQ(test_stack.top(), 10);
    test_stack.pop();
    test_stack.pop();
    EXPECT_EQ(test_stack.size(), 1);
    test_stack.pop();
    EXPECT_TRUE(test_stack.empty());
    EXPECT_THROW(test_stack.pop(), std::out_of_range);
}

TEST(StackTest, Comparisons) {
    Stack<int> test_stack1{1, 2, 3};
    Stack<int> test_stack2{1, 2, 3};
    Stack<int> test_stack3{4, 5};

    EXPECT_TRUE(test_stack1 == test_stack2);
    EXPECT_TRUE(test_stack1 != test_stack3);

    EXPECT_TRUE(test_stack1 < test_stack3);
    EXPECT_TRUE(test_stack3 > test_stack1);
    EXPECT_TRUE(test_stack1 <= test_stack2);
    EXPECT_TRUE(test_stack3 >= test_stack1);

    EXPECT_EQ(test_stack1 <=> test_stack2, std::strong_ordering::equal);
    EXPECT_EQ(test_stack1 <=> test_stack3, std::strong_ordering::less);
}

TEST(StackTest, Swap) {
    Stack<int> test_stack1{1, 2};
    Stack<int> test_stack2{3, 4, 5};

    test_stack1.swap(test_stack2);
    EXPECT_EQ(test_stack1.size(), 3);
    EXPECT_EQ(test_stack2.size(), 2);
    EXPECT_EQ(test_stack1.top(), 3);
    EXPECT_EQ(test_stack2.top(), 1);
}

TEST(StackTest, EdgeCases) {
    Stack<int> test_stack1{42};
    EXPECT_EQ(test_stack1.top(), 42);
    test_stack1.pop();
    EXPECT_TRUE(test_stack1.empty());
    Stack<int> test_stack2;
    Stack<int> test_stack3(std::move(test_stack2));
    EXPECT_TRUE(test_stack3.empty());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}