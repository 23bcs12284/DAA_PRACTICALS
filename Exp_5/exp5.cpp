class Solution
{
public:
  void quickSort(vector<int> &arr, int low, int high)
  {
    // code here
    if (low >= high)
      return;
    int p = partition(arr, low, high);
    quickSort(arr, low, p - 1);
    quickSort(arr, p + 1, high);
  }

public:
  int partition(vector<int> &arr, int low, int high)
  {

    // code here
    int p = arr[low];
    int i = low;
    int j = high;
    while (i < j)
    {
      while (arr[i] <= p && i <= high - 1)
      {
        i++;
      }
      while (arr[j] > p && j >= low + 1)
      {
        j--;
      }
      if (i < j)
      {
        swap(arr[i], arr[j]);
      }
    }
    swap(arr[j], arr[low]);
    return j;
  }
};