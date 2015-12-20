int main(void)
{
	int i;
	for (i = 0; i < 6; i = i + 1)
	{
		print(i, "\n");
	}
	print("\n");

	for (i = 0; i < 10; )
	{
		print(i, "\n");
		i = i + 1;
	}
	print("\n");

	for (i = 0; i != 0;) {}
}
