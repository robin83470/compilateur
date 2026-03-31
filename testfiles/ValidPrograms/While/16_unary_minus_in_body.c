int main() {
    int a = 3;
    int b = 0;
    while (a > 0) {
        b = b + (-a);
        a = a - 1;
    }
    return b;
}