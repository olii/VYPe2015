int gcd(int x, int y)
{
	if (y == 0)
	{
		return x;
	}
	else
	{
		return gcd(y, x%y);
	}
}

int main(void)
{
	print("gcd(2147483647, 2147483647) = ", gcd(2147483647, 2147483647), "\n");
}
