
class Solution
{
public:
  vector<vector<int>> countFreq(vector<int> &arr)
  {
    // code here
    unordered_map<int, int> mp;
    for (int i = 0; i < arr.size(); i++)
    {
      mp[arr[i]]++;
    }
    vector<vector<int>> ans;
    for (auto it = mp.begin(); it != mp.end(); it++)
    {
      ans.push_back({it->first, it->second});
    }
    return ans;
  }
};