
#include <iostream>
using namespace std;

int factorial(int n) {
    return factorial(n-1) * n;
}

int main(){

    cout << "Beginning execution." << "\n";
    cout << factorial(10) << "\n";

}