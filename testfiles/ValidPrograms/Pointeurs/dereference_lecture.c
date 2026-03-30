int main() {
    int a;
    int *p;
    int b;
    a = 42;
    p = &a;
    b = *p;  // Déréférencement en lecture
    return b;
}
