#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "exceptions.hpp"
#include "heap_cleaner.hpp"
#include "ienumerator.hpp"

template <class T>
class LinkedList {
private:
    struct Node {
        T value;
        Node* next;

        explicit Node(const T& val) : value(val), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int length;

    void Swap(LinkedList<T>& other) {
        Node* tempHead = head;
        head = other.head;
        other.head = tempHead;

        Node* tempTail = tail;
        tail = other.tail;
        other.tail = tempTail;

        int tempLength = length;
        length = other.length;
        other.length = tempLength;
    }

    void Clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* toDelete = current;
            current = current->next;
            delete toDelete;
        }

        head = nullptr;
        tail = nullptr;
        length = 0;
    }

    Node* GetNode(int index) const {
        if (index < 0 || index >= length) {
            throw IndexOutOfRangeException("LinkedList: index out of range");
        }

        Node* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current;
    }

public:
    class Enumerator : public IEnumerator<T> {
    private:
        const LinkedList<T>* list;
        const Node* initial;
        const Node* current;
        bool started;

    public:
        explicit Enumerator(const LinkedList<T>* owner)
            : list(owner), initial(owner->head), current(nullptr), started(false) {}

        Enumerator(const LinkedList<T>* owner, int startIndex)
            : list(owner), initial(nullptr), current(nullptr), started(false) {
            if (startIndex < 0 || startIndex > owner->length) {
                throw IndexOutOfRangeException("LinkedList::Enumerator: start index out of range");
            }

            if (startIndex < owner->length) {
                initial = owner->GetNode(startIndex);
            }
        }

        bool MoveNext() override {
            if (!started) {
                current = initial;
                started = true;
            } else if (current != nullptr) {
                current = current->next;
            }
            return current != nullptr;
        }

        T Current() const override {
            return CurrentRef();
        }

        const T& CurrentRef() const override {
            if (current == nullptr) {
                throw EmptyCollectionException("LinkedList::Enumerator: no current element");
            }
            return current->value;
        }

        void Reset() override {
            current = nullptr;
            started = false;
        }
    };

    LinkedList() : head(nullptr), tail(nullptr), length(0) {}

    LinkedList(T* items, int count) : head(nullptr), tail(nullptr), length(0) {
        if (count < 0) {
            throw InvalidArgumentException("LinkedList: negative count");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("LinkedList: null items pointer with positive count");
        }

        try {
            for (int i = 0; i < count; ++i) {
                Append(items[i]);
            }
        } catch (...) {
            Clear();
            throw;
        }
    }

    LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), length(0) {
        try {
            Node* current = other.head;
            while (current != nullptr) {
                Append(current->value);
                current = current->next;
            }
        } catch (...) {
            Clear();
            throw;
        }
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this == &other) {
            return *this;
        }

        LinkedList<T> temp(other);
        Swap(temp);
        return *this;
    }

    ~LinkedList() {
        Clear();
    }

    IEnumerator<T>* GetEnumerator() const {
        return new Enumerator(this);
    }

    IEnumerator<T>* GetEnumerator(int startIndex) const {
        return new Enumerator(this, startIndex);
    }

    const T& GetFirst() const {
        if (length == 0) {
            throw EmptyCollectionException("LinkedList: list is empty");
        }
        return head->value;
    }

    const T& GetLast() const {
        if (length == 0) {
            throw EmptyCollectionException("LinkedList: list is empty");
        }
        return tail->value;
    }

    const T& Get(int index) const {
        return GetNode(index)->value;
    }

    LinkedList<T>* GetSubList(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex < 0 || startIndex > endIndex || endIndex >= length) {
            throw IndexOutOfRangeException("LinkedList: invalid sublist range");
        }

        HeapCleaner<LinkedList<T>> result(new LinkedList<T>());
        HeapCleaner<IEnumerator<T>> enumerator(GetEnumerator(startIndex));

        int count = endIndex - startIndex + 1;
        for (int i = 0; i < count && enumerator->MoveNext(); ++i) {
            result->Append(enumerator->CurrentRef());
        }

        return result.Release();
    }

    int GetLength() const {
        return length;
    }

    void Append(const T& item) {
        Node* node = new Node(item);

        if (length == 0) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }

        ++length;
    }

    void Prepend(const T& item) {
        Node* node = new Node(item);

        if (length == 0) {
            head = node;
            tail = node;
        } else {
            node->next = head;
            head = node;
        }

        ++length;
    }

    void InsertAt(const T& item, int index) {
        if (index < 0 || index > length) {
            throw IndexOutOfRangeException("LinkedList: index out of range in InsertAt");
        }

        if (index == 0) {
            Prepend(item);
            return;
        }

        if (index == length) {
            Append(item);
            return;
        }

        Node* previous = GetNode(index - 1);
        Node* node = new Node(item);
        node->next = previous->next;
        previous->next = node;
        ++length;
    }

    LinkedList<T>* Concat(LinkedList<T>* list) const {
        if (list == nullptr) {
            throw InvalidArgumentException("LinkedList: null pointer in Concat");
        }

        HeapCleaner<LinkedList<T>> result(new LinkedList<T>(*this));
        HeapCleaner<IEnumerator<T>> enumerator(list->GetEnumerator());

        while (enumerator->MoveNext()) {
            result->Append(enumerator->CurrentRef());
        }

        return result.Release();
    }
};

#endif
