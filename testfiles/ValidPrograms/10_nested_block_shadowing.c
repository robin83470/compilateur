int main() {
    int x = 1;
    {
        int x = 2;
        {
            int y = x + 3;
            x = y;
        }
    }
    return x;
}