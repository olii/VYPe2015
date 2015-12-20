void test(string testName, int result)
{
	print(testName, ": ", result, "\n");
}

int main(void)
{
	test("0 && 0", 0 && 0);
	test("0 && 5", 0 && 5);
	test("5 && 0", 5 && 0);
	test("5 && 6", 5 && 6);
	test("0 || 0", 0 || 0);
	test("0 || 5", 0 || 5);
	test("5 || 0", 5 || 0);
	test("5 || 6", 5 || 6);
	test("!7", !7);
	test("!!7", !!7);
}
