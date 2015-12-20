int ackermann(int m, int n)
{
	if (!m) {return n + 1;} else {}
	if (!n) {return ackermann(m - 1, 1);} else {}
	return ackermann(m - 1, ackermann(m, n - 1));
}

int main(void){
	
	int tmp = ackermann(3, 4);
	return tmp;
}









