
#include <iostream>
using namespace std;

int factorial(int n) {
    return factorial(n-1) * n;
}

int main(){
    cout << factorial(1000000) << "\n";
}