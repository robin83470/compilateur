int main() {
    int i = 0;
    int x = 0;
    while (i < 5 && (x < 3 || i < 2)) {
        x = x + 1;
        i = i + 1;
    }
    return x;
}
