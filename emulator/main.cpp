#include <iostream>
#include "other.cpp"

extern void doSomeStuff();



int main() {
    
    std::cout << "Hello world" << std::endl;
    doSomeStuff();
    return 0;
}