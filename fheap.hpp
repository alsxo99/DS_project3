#ifndef __FHEAP_H_
#define __FHEAP_H_

#include <iostream>
#include <initializer_list>
#include <optional>
#include <vector>
#include <cmath>
#include <memory>
#include <queue>

template <typename T>
class PriorityQueue {
    public:
        virtual void insert(const T& item) = 0;
        virtual std::optional<T> extract_min() = 0;
        virtual bool is_empty() const = 0;
};

template <typename T>
class FibonacciNode {
    public:
        // Constructors
        FibonacciNode();
        FibonacciNode(const T& item)
            :key(item), degree(0), marked(false), child(nullptr), right(nullptr) {}

        // Destructor
		// You can implement your custom destructor.
        ~FibonacciNode() = default;

        T key;
        size_t degree;
		bool marked;

		std::shared_ptr<FibonacciNode<T>> right;
		std::shared_ptr<FibonacciNode<T>> child;
        // NOTE: To prevent circular reference, left/parent pointer should be set to weak_ptr.
        std::weak_ptr<FibonacciNode<T>> left;
        std::weak_ptr<FibonacciNode<T>> parent;
};

template <typename T>
class FibonacciHeap : public PriorityQueue<T> {
    public:
        // Constructors
        FibonacciHeap()
            : min_node(nullptr), size_(0) {}
        FibonacciHeap(const T& item)
            : min_node(nullptr), size_(0) { insert(item); }

        // Disable copy constructor.
        FibonacciHeap(const FibonacciHeap<T> &);

        // Destructor
        ~FibonacciHeap();

        void insert(const T& item) override;
		void insert(std::shared_ptr<FibonacciNode<T>>& node);

        // Return raw pointer of the min_node.
        FibonacciNode<T>* get_min_node() { return min_node.get(); }

        std::optional<T> get_min() const;

        std::optional<T> extract_min() override;

		void decrease_key(std::shared_ptr<FibonacciNode<T>>& x, T new_key);

		void remove(std::shared_ptr<FibonacciNode<T>>& node);

        bool is_empty() const override { return !size_; }

        size_t size() const { return size_; }
		
    private:

        std::shared_ptr<FibonacciNode<T>> min_node;
        size_t size_; // heap의 개수인듯?

        void consolidate();
        void merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y);
		void cut(std::shared_ptr<FibonacciNode<T>>& x);
		void recursive_cut(std::shared_ptr<FibonacciNode<T>>& x);

};

template <typename T>
FibonacciHeap<T>::~FibonacciHeap() {
	// TODO
	// NOTE: Be aware of memory leak or memory error.

    // std::weak_ptr<FibonacciNode<T>> current;
    // current = min_node;

    // while (current.lock()->right != nullptr)
    // {
    //     std::cout << current.lock()->right->key << std::endl;
    //     (current.lock()->right).reset();
    //     current = current.lock()->right;
    // }
    
}

template <typename T>
std::optional<T> FibonacciHeap<T>::get_min() const {
	if(!min_node) 
		return std::nullopt;
	else
		return min_node->key;
}

template <typename T>
void FibonacciHeap<T>::insert(const T& item) {
	std::shared_ptr<FibonacciNode<T>> node = std::make_shared<FibonacciNode<T>>(item);
	insert(node);
}

template <typename T>
void FibonacciHeap<T>::insert(std::shared_ptr<FibonacciNode<T>>& node) {
	// TODO

    if (is_empty()) {
        node->right = node;
        node->left = node;
        min_node = node;
        std::cout << "initial inserted!, key : " << node->key << std::endl;
    } else {
        (min_node->left).lock()->right = node;
        node->right = min_node;
        node->left = min_node->left;
        min_node->left = node;

        if (node->key < min_node->key)
            min_node = node;
        
        std::cout << "inserted!, key : " << node->key << std::endl;
        // std::cout << "left : " << (node->left).lock()->key << "    right : " << node->right->key << std::endl;
    }
    size_++;
}

template <typename T>
std::optional<T> FibonacciHeap<T>::extract_min() {
	// TODO
    if(is_empty)
        return std::nullopt;

    T min_key = min_node->key;

    // min_node와 child간의 관계 끊기
    std::shared_ptr<FibonacciNode<T>> child = min_node->child;
    min_node->child = nullptr;
    for (int i = 0; i < int(min_node->degree); i++)
    {
        child->parent = nullptr;
        child = child->right;
    }
    
    // root list에 min_node 대신 그의 child로 수정해준다.
    (min_node->left).lock()->right = child;
    (child->left).lock()->right = min_node->right;
    (min_node->right)->left = child->left;
    child->left = min_node->left;

    // size도 일시적으로 수정. min_node가 빠지고 child들이 root list로 들어왔으니까
    size_ += min_node->degree - 1;

    // min_node update, 기존 min_node의 left, right가 사라지는건가? (reference num 줄어드나?)
    // 아니라면 right에 nullptr넣고 left를 주면 되겠다.
    min_node = min_node->right;

    consolidate();

    // size_--; // size가 heap 개수인지, node 개수인지 확인해야함.
    return min_key;
}

template <typename T>
void FibonacciHeap<T>::decrease_key(std::shared_ptr<FibonacciNode<T>>& x, T new_key) {
	// TODO

}

template <typename T>
void FibonacciHeap<T>::remove(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
	
}

template <typename T>
void FibonacciHeap<T>::consolidate() {
	float phi = (1 + sqrt(5)) / 2.0;
	int len = int(log(size_) / log(phi)) + 1;
	// TODO

	std::vector<std::shared_ptr<FibonacciNode<T>>> A(len, nullptr);
    for (int i = 0; i < int(size_); i++)
    {
        int index = min_node->degree;
        if (A[index] == nullptr)
            A[index] = min_node;
        else
            merge(A[index], min_node);

        min_node = min_node->right;
    }
    
}

template <typename T>
void FibonacciHeap<T>::merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y) {
	// TODO

    if (x->key < y->key) {
        // y를 root list에서 제거하고, y의 parent를 x로 해준다. x의 degree는 1 증가.
        (y->left).lock()->right = y->right;
        (y->right)->left = y->left;
        y->parent = x;
        x->degree += 1;

        // x의 child와 y를 doubly linked 관계로 만들어준다.
        (x->child->left).lock()->right = y;
        y->right = x->child;
        y->left = x->child->left;
        x->child->left = y;
    } else {
        (x->left).lock()->right = x->right;
        (x->right)->left = x->left;
        x->parent = y;
        y->degree += 1;

        (y->child->left).lock()->right = x;
        x->right = y->child;
        x->left = y->child->left;
        y->child->left = x;
    }
}

template <typename T>
void FibonacciHeap<T>::cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO

}

template <typename T>
void FibonacciHeap<T>::recursive_cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO

}

#endif // __FHEAP_H_
