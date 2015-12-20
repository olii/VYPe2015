int main(void)
{
	int x = 305402420;
	char c = 'A';

	char int_to_char = (char)x;
	int char_to_int = (int)c;
	string char_to_string = (string)c;

	print("(char)305402420: ", int_to_char, "\n");
	print("(int)'A': ", char_to_int, "\n");
	print("(string)'A' ", char_to_string, "\n");
}
