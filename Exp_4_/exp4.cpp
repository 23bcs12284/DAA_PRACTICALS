#include <iostream>
using namespace std;

// ==================== DOUBLY LINKED LIST ====================
struct DNode
{
  int data;
  DNode *prev;
  DNode *next;
  DNode(int val)
  {
    data = val;
    prev = next = NULL;
  }
};

class DoublyLinkedList
{
  DNode *head;

public:
  DoublyLinkedList() { head = NULL; }

  void insertAtBeginning(int val)
  {
    DNode *newNode = new DNode(val);
    if (!head)
    {
      head = newNode;
      return;
    }
    newNode->next = head;
    head->prev = newNode;
    head = newNode;
  }

  void insertAtEnd(int val)
  {
    DNode *newNode = new DNode(val);
    if (!head)
    {
      head = newNode;
      return;
    }
    DNode *temp = head;
    while (temp->next)
      temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
  }

  void deleteAtBeginning()
  {
    if (!head)
      return;
    DNode *temp = head;
    head = head->next;
    if (head)
      head->prev = NULL;
    delete temp;
  }

  void deleteAtEnd()
  {
    if (!head)
      return;
    if (!head->next)
    {
      delete head;
      head = NULL;
      return;
    }
    DNode *temp = head;
    while (temp->next)
      temp = temp->next;
    temp->prev->next = NULL;
    delete temp;
  }

  void display()
  {
    cout << "DLL: ";
    DNode *temp = head;
    while (temp)
    {
      cout << temp->data << " ";
      temp = temp->next;
    }
    cout << endl;
  }
};

// ==================== CIRCULAR LINKED LIST ====================
struct CNode
{
  int data;
  CNode *next;
  CNode(int val)
  {
    data = val;
    next = NULL;
  }
};

class CircularLinkedList
{
  CNode *tail;

public:
  CircularLinkedList() { tail = NULL; }

  void insertAtBeginning(int val)
  {
    CNode *newNode = new CNode(val);
    if (!tail)
    {
      tail = newNode;
      tail->next = tail;
      return;
    }
    newNode->next = tail->next;
    tail->next = newNode;
  }

  void insertAtEnd(int val)
  {
    CNode *newNode = new CNode(val);
    if (!tail)
    {
      tail = newNode;
      tail->next = tail;
      return;
    }
    newNode->next = tail->next;
    tail->next = newNode;
    tail = newNode;
  }

  void deleteAtBeginning()
  {
    if (!tail)
      return;
    CNode *head = tail->next;
    if (head == tail)
    {
      delete tail;
      tail = NULL;
      return;
    }
    tail->next = head->next;
    delete head;
  }

  void deleteAtEnd()
  {
    if (!tail)
      return;
    CNode *temp = tail->next;
    if (temp == tail)
    {
      delete tail;
      tail = NULL;
      return;
    }
    while (temp->next != tail)
      temp = temp->next;
    temp->next = tail->next;
    delete tail;
    tail = temp;
  }

  void display()
  {
    if (!tail)
    {
      cout << "CLL: (empty)" << endl;
      return;
    }
    cout << "CLL: ";
    CNode *temp = tail->next;
    do
    {
      cout << temp->data << " ";
      temp = temp->next;
    } while (temp != tail->next);
    cout << endl;
  }
};

// ==================== MAIN FUNCTION ====================
int main()
{
  cout << "=== Doubly Linked List Operations ===" << endl;
  DoublyLinkedList dll;
  dll.insertAtBeginning(10);
  dll.insertAtEnd(20);
  dll.insertAtBeginning(5);
  dll.display();
  dll.deleteAtBeginning();
  dll.display();
  dll.deleteAtEnd();
  dll.display();

  cout << "\n=== Circular Linked List Operations ===" << endl;
  CircularLinkedList cll;
  cll.insertAtBeginning(10);
  cll.insertAtEnd(20);
  cll.insertAtBeginning(5);
  cll.display();
  cll.deleteAtBeginning();
  cll.display();
  cll.deleteAtEnd();
  cll.display();

  return 0;
}
