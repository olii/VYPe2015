int deep(char x, char y)
{
    x = 'A';
	while ((int)x >= 32)
	{
		print(x);
		y = x;
		y = (char)((int)y - 1);
		x = y;
	}
	print(x, y);
}

int main (void)
{
	return deep('a', 'b');
}

