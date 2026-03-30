int main() {
    int a;
    int *p;
    a = 1;
    p = &a;
    *(p) = 7;
    return *(p);
}
