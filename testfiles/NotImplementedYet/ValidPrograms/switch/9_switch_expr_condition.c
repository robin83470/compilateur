int main() {
    int a = 3;
    int b = 2;
    switch (a * b - 4) {
        case 0:
            return 10;
        case 2:
            return 20;
        case 4:
            return 40;
        default:
            return 0;
    }
}
