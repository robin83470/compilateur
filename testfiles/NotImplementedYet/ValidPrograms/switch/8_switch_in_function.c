int classify(int n) {
    switch (n) {
        case 0:
            return 0;
        case 1:
            return 1;
        default:
            return 2;
    }
}

int main() {
    return classify(1);
}
