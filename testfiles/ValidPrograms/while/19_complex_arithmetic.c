int main() {
    int a = 1;
    int b = 2;
    int c = 0;

    while (a < 5) {
        c = c + a * b;
        a = a + 1;
        b = b + 1;
    }

    return c;
}