#include <gtest/gtest.h>
#include "functions.h"

using namespace my_container;

TEST(ArrayTest, Constructor_Default) {
    Array<int, 5> arr;
    EXPECT_EQ(arr.size(), 5);
    EXPECT_EQ(arr.empty(), false);
}

TEST(ArrayTest, Constructor_InitializeList) {
    Array<int, 3> arr = {1, 2, 3};
    ASSERT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(ArrayTest, At) {
    Array<int, 2> arr;
    arr[0] = 10;
    arr[1] = 20;
    EXPECT_EQ(arr.at(0), 10);
    EXPECT_THROW(arr.at(2), std::out_of_range);
}

TEST(ArrayTest, Front_Back) {
    Array<int, 3> arr = {5, 10, 15};
    EXPECT_EQ(arr.front(), 5);
    EXPECT_EQ(arr.back(), 15);
    EXPECT_EQ(*arr.data(), 5);
}

TEST(ArrayTest, Iterator_Test) {
    Array<int, 3> arr = {1, 2, 3};
    int sum = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) sum += *it;
    EXPECT_EQ(sum, 6);
}

TEST(ArrayTest, Fill_And_Swap) {
    Array<int, 4> arr1;
    arr1.fill(5);
    Array<int, 4> arr2 = {0, 0, 0, 0};
    arr1.swap(arr2);
    EXPECT_EQ(arr2[0], 5);
    EXPECT_EQ(arr1[0], 0);
}

TEST(ArrayTest, Equality_Operators) {
    Array<int, 2> arr1 = {1, 2};
    Array<int, 2> arr2 = {1, 2};
    Array<int, 2> arr3 = {3, 4};
    EXPECT_EQ(arr1 == arr2, true);
    EXPECT_EQ(arr1 != arr3, true);
}

TEST(ArrayTest, More_Less_Operators) {
    Array<int, 3> arr1 = {1, 2, 3};
    Array<int, 3> arr2 = {1, 2, 4};
    Array<int, 3> arr3 = {1, 2, 3};
    EXPECT_EQ(arr1 < arr2, true);
    EXPECT_EQ(arr2 > arr1, true);
    EXPECT_EQ(arr1 <= arr3, true);
    EXPECT_EQ(arr3 >= arr1, true);
}

TEST(ArrayTest, Three_Way_Comparison) {
    Array<int, 3> arr1 = {1, 2, 3};
    Array<int, 3> arr2 = {1, 2, 4};
    EXPECT_EQ((arr1 <=> arr2) < 0, true);
}

TEST(ArrayTest, Output_Operator) {
    Array<int, 3> arr = {10, 20, 30};
    std::stringstream ss;
    ss << arr;
    EXPECT_EQ(ss.str(), "10 20 30 ");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}