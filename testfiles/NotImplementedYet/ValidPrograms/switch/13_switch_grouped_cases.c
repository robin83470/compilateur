int main() {
    int x = 2;
    int result = 0;
    switch (x) {
        case 1:
        case 2:
        case 3:
            result = 100;
            break;
        default:
            result = 0;
    }
    return result;
}
