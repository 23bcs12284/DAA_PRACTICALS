#include <iostream>
using namespace std;

template <class T>
class Stack
{
private:
  int top;
  int capacity;
  T *arr;

public:
  // Constructor
  Stack(int size)
  {
    capacity = size;
    arr = new T[capacity];
    top = -1;
  }

  // Destructor
  ~Stack()
  {
    delete[] arr;
  }

  // Push operation
  void push(T value)
  {
    if (isFull())
    {
      cout << "Stack is FULL! Cannot push " << value << endl;
      return;
    }
    arr[++top] = value;
    cout << value << " pushed into stack." << endl;
  }

  // Pop operation
  void pop()
  {
    if (isEmpty())
    {
      cout << "Stack is EMPTY! Cannot pop." << endl;
      return;
    }
    cout << arr[top--] << " popped from stack." << endl;
  }

  // Return top element
  T peek()
  {
    if (isEmpty())
    {
      cout << "Stack is EMPTY!" << endl;
      return T(); // Return default value of T
    }
    return arr[top];
  }

  // Check if stack is empty
  bool isEmpty()
  {
    return top == -1;
  }

  // Check if stack is full
  bool isFull()
  {
    return top == capacity - 1;
  }
};

int main()
{
  // Stack of integers
  Stack<int> s(5);

  s.push(10);
  s.push(20);
  s.push(30);

  cout << "Top element: " << s.peek() << endl;

  s.pop();
  cout << "Top element after pop: " << s.peek() << endl;

  s.push(40);
  s.push(50);
  s.push(60);
  s.push(70); 

  while (!s.isEmpty())
  {
    cout << "Popping: " << s.peek() << endl;
    s.pop();
  }

  s.pop(); // Will show empty error

  return 0;
}
