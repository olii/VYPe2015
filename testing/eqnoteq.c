int zero(void)
{
	return 0;
}

int one(void)
{
	return 1;
}

int main(void)
{
	print("check for !=");
	if (zero() != zero())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (zero() != one())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	if (one() != zero())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	if (one() != one())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	print("check for ==");
	if (zero() == zero())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	if (zero() == one())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() == zero())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() == one())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	print("check for <=");
	if (zero() <= zero())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	if (zero() <= one())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() <= zero())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() <= one())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	print("check for >");
	if (zero() > zero())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	if (zero() > one())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() > zero())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() > one())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	print("check for >=");
	if (zero() >= zero())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	if (zero() >= one())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() >= zero())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() >= one())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	print("check for <");
	if (zero() < zero())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
	if (zero() < one())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() < zero())
	{
		print("Never goes here.\n");
	}
	else
	{
		print("Goes here.\n");
	}
	if (one() < one())
	{
		print("Goes here.\n");
	}
	else
	{
		print("Never goes here.\n");
	}
}
