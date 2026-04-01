int main() {
    int a = 1;
    int b = 0;
    int c = 1;
    return ((a && (b || c)) || (!a && c));
}
