int main() {
    int a = 0;
    { { { a = 7; } } }
    return a;
}
