int main() {
    int a = 1;
    int b = 0;
    if (a && (b || a)) {
        return 9;
    }
    return 4;
}
