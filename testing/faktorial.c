int faktorial(int n)
{
	if (n == 0)
	{
		return 1;
	}
	else
	{
		return n * faktorial(n - 1);
	}
}

int main(void)
{
	print(faktorial(5), "\n");
}
