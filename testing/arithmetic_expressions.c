void test(string testName, int n)
{
	print(testName, ": ", n, "\n");
}

int main(void)
{
	test("5", 5);
	test("-5", -5);
	test("+5", +5);
	test("1 + 2", 1 + 2);
	test("-1 + -2", -1 + -2);
	test("-(1 + 2)", -(1 + 2));
	test("+(1 + 2)", +(1 + 2));
	test("3 + 5 / 2", 3 + 5 / 2);
	test("3 + 5 * 2", 3 + 5 * 2);
	test("(3 + 5) * 2", (3 + 5) * 2);
	test("14 % 6", 14 % 6);
}
