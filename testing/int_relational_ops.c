void test(string testName, int result)
{
	print(testName, ": ", result, "\n");
}

int main(void)
{
	int a,b;
	a = 5;
	b = 6;

	test("a == a", a == a);
	test("a == b", a == b);
	test("a != a", a != a);
	test("a != b", a != b);
	test("a < a", a < a);
	test("a < b", a < b);
	test("a <= a", a <= a);
	test("a <= b", a <= b);
	test("a > a", a > a);
	test("a > b", a > b);
	test("a >= a", a >= a);
	test("a >= b", a >= b);
	test("!a", !a);
	test("!!a", !!a);
}
