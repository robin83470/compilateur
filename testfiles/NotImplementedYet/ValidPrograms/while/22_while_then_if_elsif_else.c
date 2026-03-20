int main() {
    int a = 0;
    int b = 0;

    while (a < 6) {
        if (a < 2) {
            b = b + 10;
        } else if (a < 4) {
            b = b + 5;
        } else {
            b = b + 1;
        }
        a = a + 1;
    }

    return b;
}