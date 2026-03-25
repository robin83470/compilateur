#include <stdio.h>

int main() {
    int x = 2;
    switch (x) {
        case 1:
            putchar('A');
            break;
        case 2:
            putchar('B');
            break;
        case 3:
            putchar('C');
            break;
        default:
            putchar('?');
            break;
    }
    return 0;
}
