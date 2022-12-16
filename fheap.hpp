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
        size_t size_;

        void consolidate();
        void merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y);
		void cut(std::shared_ptr<FibonacciNode<T>>& x);
		void recursive_cut(std::shared_ptr<FibonacciNode<T>>& x);

};

template <typename T>
FibonacciHeap<T>::~FibonacciHeap() {
	// TODO
	// NOTE: Be aware of memory leak or memory error.

    std::weak_ptr<FibonacciNode<T>> temp;
    temp = min_node;

    std::shared_ptr<FibonacciNode<T>> temp_lock;

    while ((temp_lock = temp.lock())->right != nullptr)
    {
        a->right = nullptr;
        min
    }
    
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
        if (std::shared_ptr<FibonacciNode<T>> min_node_left = (min_node->left).lock())
            min_node_left->right = node;
        node->right = min_node;
        node->left = min_node->left;
        min_node->left = node;

        if (node->key < min_node->key)
            min_node = node;
        
        std::cout << "inserted!, key : " << node->key << std::endl;
    }
    size_++;
}

template <typename T>
std::optional<T> FibonacciHeap<T>::extract_min() {
	// TODO

	return std::nullopt;
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

}

template <typename T>
void FibonacciHeap<T>::merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y) {
	// TODO

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
