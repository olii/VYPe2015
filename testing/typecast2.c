char number(void)
{
	return 'c';
}

int main(void)
{
	print("\n(char) (((int) number()) + 5) = ", (char) (((int) number()) + 5));
	print("\n(int) number() + 5 = ", ((int) number()) + 5);
}
