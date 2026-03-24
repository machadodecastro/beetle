#include <iostream>
#include <string>

int beetle_pow(int base, int exp) {
    if (exp < 0) return 0;
    int result = 1;
    while (exp > 0) {
        result *= base;
        exp--;
    }
    return result;
}

int main() {
    int a = 10;
    int b = 20;
    int c = (a + b);
    std::string msg = "Resultado: ";
    std::cout << msg << std::endl;
    std::cout << c << std::endl;
    return 0;
}
