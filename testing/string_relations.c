string trueFalse(int a){
    if (a){
        return "true";
    } else {
        return "false";
    }
}



void printer(string a, string b, string op){
    print(a, " ", op, " ", b, " = ");
}

void tester(string a, string b){
    printer(a, b, "<");
    print(trueFalse(a < b));
    print ("\n");
    
    printer(a, b, "<=");
    print(trueFalse(a <= b));
    print ("\n");
    
    printer(a, b, ">");
    print(trueFalse(a < b));
    print ("\n");
    
    printer(a, b, ">=");
    print(trueFalse(a >= b));
    print ("\n");
    
    printer(a, b, "==");
    print(trueFalse(a == b));
    print ("\n");
        printer(a, b, "!=");
    print(trueFalse(a != b));
    print ("\n");
}


int main(void)
{
    tester("aaa", "aaa");
    tester("aaa", "aab");
    tester("aab", "aaa");
    tester("zxczczxc", "dsgvdgsr");
    tester("", "dsgvdgsr");
    tester("sdfsdfsadf", "");
    tester("", "");
    return 22;
}
