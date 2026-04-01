int main() {
    int i = 0;
    int c = 0;

    while (i < 10) {
        if (i % 2 == 0) {
            c = c + 1;
        }
        i = i + 1;
    }

    return c;
}