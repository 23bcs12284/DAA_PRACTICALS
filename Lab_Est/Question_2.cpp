class Solution
{
public:
  int longestValidParentheses(string s)
  {
    stack<int> st;
    st.push(-1);
    int maxValue = 0;

    for (int i = 0; i < s.length(); i++)
    {
      if (s[i] == '(')
      {
        st.push(i);
      }
      else
      {
        st.pop();
        if (st.empty())
        {
          st.push(i);
        }
        else
        {
          maxValue = max(maxValue, i - st.top());
        }
      }
    }

    return maxValue;
  }
};