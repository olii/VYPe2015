void foo(void)
{
	print("foo(void)\n");
}

int foo(int x)
{
	print("foo(int)\n");
}

char foo(char c)
{
	print("foo(char)\n");
}

string foo(string s)
{
	print("foo(string)\n");
}

int main(void)
{
	foo();
	foo(1);
	foo('a');
	foo("abc");
}
