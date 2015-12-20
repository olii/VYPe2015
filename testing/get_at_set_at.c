int main(void)
{
	string s = "Hello World!";

	print(s, "\n");
	print("get_at(\"Hello World\", 6): ", get_at(s, 6), "\n");
	print("set_at(\"Hello World\", 6, 'w'): ", set_at(s, 6, 'w'), "\n");
	print(s, "\n");
}
