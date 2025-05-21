#include <iostream>

#include "Stack.hpp"

int main() {
    Stack<int> first {1, 2, 3, 4, 5};
    first.push(10);
    std::cout << first.top();
    return 0;
}
