int foo(int x){
    return 2*x;
}

int foo2(int x){
    return 2/x;
}

int main(){
    return foo2(foo(2));
}