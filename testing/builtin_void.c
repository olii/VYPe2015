string foo(int dummy, char c, string asdf){
    read_int();
    read_char();
    read_string();
    string a = read_string();
    get_at(a, 10000);
    set_at(a, -200, 'f');


    return strcat(asdf, a);
}


int main(void){
    print(foo(54654, 'a', "asdf"));
    print("\n");
    return 5555;
}
