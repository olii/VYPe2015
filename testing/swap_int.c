int deep(int x, int y)
{
	x = 3;
	while (x >= 1)
	{
		print(x);
		y = x;
		y = y - 1;
		x = y;
	}
	print(x, y);
}

int main (void)
{
	return deep(22 > 3, 22 < 3);
}

