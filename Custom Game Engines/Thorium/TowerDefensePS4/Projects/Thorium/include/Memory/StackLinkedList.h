#pragma once

namespace Th
{
    template <class T>
    class StackLinkedList 
    {
    public:
        struct Node 
        {
            T data;
            Node* next;
        };
    
        Node* head;

    public:
        StackLinkedList() = default;
        StackLinkedList(StackLinkedList &stackLinkedList) = delete;
        void push(Node * newNode);
        Node* pop();
    };

    template <class T>
    void StackLinkedList<T>::push(Node* newNode) 
    {
	    newNode->next = head;
	    head = newNode;
    }

    template <class T>
    typename StackLinkedList<T>::Node* StackLinkedList<T>::pop() 
    {
	    Node* top = head;
	    head = head->next;
	    return top;
    }
}

