class Solution
{
public:
  int f(int W, int i, vector<int> &val, vector<int> &wt, vector<vector<int>> &dp)
  {
    if (i == wt.size())
      return 0; // base case: no items left
    if (dp[i][W] != -1)
      return dp[i][W];

    // Not pick item i
    int np = f(W, i + 1, val, wt, dp);

    // Pick item i if weight allows
    int p = 0;
    if (W >= wt[i])
      p = val[i] + f(W - wt[i], i + 1, val, wt, dp);

    return dp[i][W] = max(p, np);
  }

  int knapsack(int W, vector<int> &val, vector<int> &wt)
  {
    int n = wt.size();
    vector<vector<int>> dp(n, vector<int>(W + 1, -1));
    return f(W, 0, val, wt, dp);
  }
};