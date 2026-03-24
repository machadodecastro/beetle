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

int power2(int x) {
    return beetle_pow(x, 2);
}

int add(int a, int b) {
    return (a + b);
}

int sub(int a, int b) {
    return (a - b);
}

int mul(int a, int b) {
    return (a * b);
}

int division(int a, int b) {
    return (a / b);
}

int main() {
    int a = 5;
    int b = 3;
    int x = 81;
    int y = 3;
    int c = add(a, b);
    int d = sub(a, b);
    int e = mul(a, b);
    int f = division(x, y);
    std::cout << c << std::endl;
    if (((c > 5) && (c < 20))) {
    std::cout << "value is in range" << std::endl;
    }
    else {
    std::cout << "value is out of range" << std::endl;
    }
    int i = 0;
    while ((i < 4)) {
    std::cout << i << std::endl;
    i = (i + 1);
    }
    std::cout << power2(6) << std::endl;
    std::cout << d << std::endl;
    std::cout << e << std::endl;
    std::cout << f << std::endl;
    return 0;
}
