class Solution
{
public:
  double myPow(double base, int exp)
  {
    long long n = exp; // use long long to avoid overflow
    if (n < 0)
    {
      base = 1 / base;
      n = -n;
    }

    double result = 1.0;
    while (n > 0)
    {
      if (n % 2 == 1) // if odd
        result *= base;
      base *= base; // square the base
      n /= 2;       // halve the exponent
    }
    return result;
  }
};
