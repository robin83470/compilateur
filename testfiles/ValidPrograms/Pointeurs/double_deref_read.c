int main() {
    int a;
    int *p;
    int **pp;
    a = 55;
    p = &a;
    pp = &p;
    return **pp;
}
