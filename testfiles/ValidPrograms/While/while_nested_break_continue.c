int main() {
    int i = 0;
    int j = 0;
    int count = 0;

    while (i < 3) {
        i = i + 1;
        j = 0;

        while (j < 4) {
            j = j + 1;
            count = count + 1;
            continue;
            count = count + 10;
        }

        break;
    }

    return count;
}
