void foo(void){

}
void bar(void){
	return;
}

int retInput(int a){
	return a;
}

int main(void){
	int val = read_int();
	char cVal = read_char();
	string str = read_string();

	foo();
	bar();
	print(retInput(5*(2+3)));
	print (str);
	print("\n");


}
