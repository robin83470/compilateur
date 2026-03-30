int main() {
    int a;
    int *p;
    a = 10;
    p = &a;
    *p = 99;  // Déréférencement en écriture
    return a;
}
