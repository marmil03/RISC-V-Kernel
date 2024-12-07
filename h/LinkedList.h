#pragma once
#include "MemoryAllocator.h"

template<typename T>
class LinkedList{
public:
    LinkedList();

    LinkedList(const LinkedList &) = delete;
    LinkedList(LinkedList &&) = delete;

    void delete_element(T* element);

    void push_back(T* element);
    void push_front(T* element);
    void pop_front();

    T* front();
    T* back();

    LinkedList& operator=(const LinkedList&) = delete;
    LinkedList& operator=(LinkedList&&) = delete;

    void* operator new(size_t);
    void operator delete(void*);
//private:
    struct Node{
        T* val;
        Node *next;
        Node *prev;
        explicit Node(T* val);
        void* operator new(size_t);
        void operator delete(void*);
    };
    Node *head, *tail;
};

template<typename T>
void LinkedList<T>::delete_element(T *element) {
    Node* curr = head;
    while(curr){
        if(curr->val == element) break;
        curr = curr->next;
    }
    if(curr == head) head = head->next;
    if(curr == tail) tail = tail->prev;
    if(curr){
       if(curr->prev) curr->prev->next = curr->next;
       if(curr->next) curr->next->prev = curr->prev;
    }
    if(!head || !tail) head = tail = nullptr;
    delete curr;
}

template<typename T>
void *LinkedList<T>::Node::operator new(size_t size) {
    return MemoryAllocator::kernel_malloc(size);
}

template<typename T>
void LinkedList<T>::Node::operator delete(void *mem_block) {
    MemoryAllocator::kernel_free(mem_block);
}

template<typename T>
void *LinkedList<T>::operator new(size_t size) {
    return MemoryAllocator::kernel_malloc(size);
}

template<typename T>
void LinkedList<T>::operator delete(void *mem_block) {
    MemoryAllocator::kernel_free(mem_block);
}

template<typename T>
LinkedList<T>::Node::Node(T* val)
    :val(val), next(nullptr), prev(nullptr) {}

template<typename T>
LinkedList<T>::LinkedList()
    :head(nullptr), tail(nullptr){}

template<typename T>
void LinkedList<T>::push_back(T* element) {
    if(!head){
        head = new Node(element);
        tail = head;
    }
    else{
        tail->next = new Node(element);
        tail->next->prev = tail;
        tail = tail->next;
    }
}

template<typename T>
void LinkedList<T>::push_front(T *element) {
    if(!head) head = new Node(element);
    else{
        Node* new_head = new Node(element);
        new_head->next = head;
        head = new_head;
    }
}

template<typename T>
void LinkedList<T>::pop_front() {
    if(!head) return;
    if(head == tail) tail = nullptr;
    Node* t = head;
    head = head->next;
    if(head) head->prev = nullptr;
    delete t;
}

template<typename T>
T* LinkedList<T>::front() {
    return (head ? head->val : 0);
}

template<typename T>
T* LinkedList<T>::back() {
    return (tail ? tail->val : 0);
}
