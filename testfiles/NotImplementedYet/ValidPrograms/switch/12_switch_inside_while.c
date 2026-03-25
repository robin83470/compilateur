int main() {
    int i = 0;
    int sum = 0;
    while (i < 4) {
        switch (i) {
            case 0:
                sum = sum + 1;
                break;
            case 1:
                sum = sum + 2;
                break;
            case 2:
                sum = sum + 4;
                break;
            default:
                sum = sum + 8;
                break;
        }
        i = i + 1;
    }
    return sum;
}
