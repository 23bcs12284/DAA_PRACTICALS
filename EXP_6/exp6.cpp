class Solution
{
public:
  bool f(vector<int> &arr, int sum, int i, vector<vector<int>> &dp)
  {
    if (sum == 0)
      return true;
    if (i == arr.size())
      return false;
    if (dp[i][sum] != -1)
      return dp[i][sum];

    bool np = f(arr, sum, i + 1, dp);
    bool p = false;
    if (sum >= arr[i])
      p = f(arr, sum - arr[i], i + 1, dp);

    return dp[i][sum] = p || np;
  }

  bool isSubsetSum(vector<int> &arr, int sum)
  {
    int n = arr.size();
    vector<vector<int>> dp(n, vector<int>(sum + 1, -1));
    return f(arr, sum, 0, dp);
  }
};