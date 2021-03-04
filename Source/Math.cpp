#include "Math.h"

namespace math {

	int gcd(int a, int b)
	{
		for (;;)
		{
			if (a == 0) return b;
			b %= a;
			if (b == 0) return a;
			a %= b;
		}
	}

	int lcm(int a, int b)
	{
		int temp = gcd(a, b);

		return temp ? (a / temp * b) : 0;
	}

}