#include <iostream>
#include <memory>
using namespace std;

struct A {
    void print() {
        cout << "Object of A" << endl;
    }
};

int main() {

    unique_ptr<A> ptr = make_unique<A>();

    ptr->print();
    
    cout << "Address: " << ptr.get();

    return 0;
}