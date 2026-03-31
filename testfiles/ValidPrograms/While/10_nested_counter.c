int main() {
    int i = 1;
    int j = 1;
    int acc = 0;

    while (i <= 3) {
        j = 1;
        while (j <= 2) {
            acc = acc + i + j;
            j = j + 1;
        }
        i = i + 1;
    }

    return acc;
}