#include <iostream>
using namespace std;

class MyClass {
public:
    MyClass() {
        cout << "Constructor called" << endl;
    }
    ~MyClass() {
        cout << "Destructor called" << endl;
    }
};

int main() {
    MyClass obj; // Constructor is called here
    // Destructor will be called automatically when 'obj' goes out of scope
    return 0;
}