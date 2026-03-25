int main() {
    int x = 3;
    int result = 0;
    switch (x) {
        case 1:
            result = 10;
            break;
        case 3:
            result = 30;
            break;
        case 5:
            result = 50;
            break;
        default:
            result = 99;
            break;
    }
    return result;
}
