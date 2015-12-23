void a(void);
int b(int);
char c(char);
string d(string);

void e(int, char, string);
int f(int, char, string);
char g(int, char, string);
string h(int, char, string);

int b(int a){ return a; }

int main(void)
{
	int x = b(5);
}
