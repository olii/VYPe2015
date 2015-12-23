int foo(int a1){
    int a2 = a1+95;
    int a3 = a2 + a1;
    int a4 = a3+a2+a1;
    int a5 = a4+a3+a2+a1;
    int a6 = a5+a4+a3+a2+a1;
    int a7 = a6+a5+a4+a3+a2+a1;
    int a8 = a7+a6+a5+a4+a3+a2+a1;
    int a9 = a8+a7+a6+a5+a4+a3+a2+a1;
    int a10 = a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a11 = a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a12 = a11 + a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a13 = a12 + a11 + a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a14 = a13+ a12 + a11 + a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a15 = a14 + a13+ a12 + a11 + a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a16 = a15 + a14 + a13+ a12 + a11 + a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a17 = a16 + a15 + a14 + a13+ a12 + a11 + a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a18 = a17 + a16 + a15 + a14 + a13+ a12 + a11 + a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    int a19 = a18 + a17 + a16 + a15 + a14 + a13+ a12 + a11 + a10 + a9+a8+a7+a6+a5+a4+a3+a2+a1;
    return a8+a11-a7+a2*(a3-a2) - a18;
}

int main(void){
    int tmp = foo(8/3);
    print(tmp, "\n");
    return tmp;
}
