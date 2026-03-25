int main() {
    int x = 1;
    switch (x) {
        case 1: {
            int local = 77;
            return local;
        }
        case 2: {
            int local = 88;
            return local;
        }
        default:
            return 0;
    }
}
