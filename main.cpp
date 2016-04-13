#include <iostream>
#include "basic.h"
using namespace std;

int main( int argc, const char* argv[] ) {
    Vector3 a(3, 2, 1);
    a.dump();
    a.getNormal().dump();
    Vector3 b(2, 1, 0);
    (a * (b * a)).dump();
    return 0;
}