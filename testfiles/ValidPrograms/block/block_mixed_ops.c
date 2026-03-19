int main() {
    int v = 0;
    {
        int x = 2;
        int y = 3;
        v = x * y + (x - y);
    }
    return v;
}
