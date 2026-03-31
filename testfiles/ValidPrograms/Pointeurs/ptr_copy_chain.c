int main() {
    int a;
    int *p;
    int *q;
    a = 33;
    p = &a;
    q = p;
    return *q;
}
