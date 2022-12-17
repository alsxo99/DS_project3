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

    // std::weak_ptr<FibonacciNode<T>> current;
    // current = min_node;

    // while (current.lock()->right != nullptr)
    // {
    //     std::cout << current.lock()->right->key << std::endl;
    //     (current.lock()->right).reset();
    //     current = current.lock()->right;
    // }

    // 연습용
    // std::shared_ptr<FibonacciNode<T>> p1 = min_node;
    // std::shared_ptr<FibonacciNode<T>> p2 = min_node;

    // std::cout << (p1 == p2) << std::endl;
    // std::cout << (p1.get() == p2.get()) << std::endl;
    
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
    if(is_empty())
        return std::nullopt;
    std::cout << "-----extract_min-----" << std::endl;

    T min_key = min_node->key;

    // min_node와 child간의 관계 끊기
    // child가 있는 경우,
    if (min_node->degree)
    {
        std::shared_ptr<FibonacciNode<T>> child = min_node->child;
        min_node->child = nullptr; // 필요 없나?

        // for (int i = 0; i < int(min_node->degree); i++)
        // {
        //     child->parent = nullptr;
        //     child = child->right;
        // }
        
        // root list에 min_node 대신 그의 child로 수정해준다.
        (min_node->left).lock()->right = child;
        (child->left).lock()->right = min_node->right;
        (min_node->right)->left = child->left;
        child->left = min_node->left;
    // child가 없는 경우, min_node만 root list에서 빼고 올바르게 연결해준다.
    } else {
        (min_node->left).lock()->right = min_node->right;
        (min_node->right)->left = min_node->left;
    }

    // min_node update, 기존 min_node의 left, right가 사라지는건가? (reference num 줄어드나?)
    // reference num은 당연히 줄어들텐데, 객체가 삭제되는건지 모르겠다.
    // 아니라면 right에 nullptr넣고 left를 주면 되겠다.
    min_node = min_node->right;
    std::shared_ptr<FibonacciNode<T>> start = min_node;
    std::shared_ptr<FibonacciNode<T>> current = min_node;
    do
    {
        if(current->key < min_node->key)
            min_node = current;
        current = current->right;
    } while (start != current);

    consolidate();

    size_--; // node의 개수가 1 줄었으므로, (여기서 줄이는게 맞나 ? ) consolidate 전에 해야 할지도
    return min_key;
}

template <typename T>
void FibonacciHeap<T>::decrease_key(std::shared_ptr<FibonacciNode<T>>& x, T new_key) {
	// TODO
    x->key = new_key;
    // x가 root node인 경우, return.
    if ((x->parent).lock() == nullptr) return;
    // decrease_key가 min heap을 violate하지 않은 경우, key만 바꿔주고 return.
    if ((x->parent).lock()->key <= new_key)
        return;
    // violate 한 경우, 
    else cut(x);
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

    std::cout << "-----consolidate-----" << std::endl;

	std::vector<std::shared_ptr<FibonacciNode<T>>> A(len, nullptr);

    std::shared_ptr<FibonacciNode<T>> start = min_node;
    std::shared_ptr<FibonacciNode<T>> current = min_node;

    int index;
    do {
        index = current->degree;
        if (A[index] == nullptr) {
            A[index] = current;
            current = current->right;
        } else {
            if (A[index] != current) {
                merge(A[index], current);
                if (current->key > A[index]->key)
                    current = A[index];
                A[index] = nullptr;
            }
        }
    } while(start != current);
    
}

template <typename T>
void FibonacciHeap<T>::merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y) {
	// TODO
    std::cout << "------merge------" << std::endl;

    if (x->key < y->key) {
        // y를 root list에서 제거하고, y의 parent를 x로 해준다. x의 degree는 1 증가.
        (y->left).lock()->right = y->right;
        (y->right)->left = y->left;
        y->parent = x;

        // x의 child와 y를 doubly linked 관계로 만들어준다.
        if (x->degree) {
            (x->child->left).lock()->right = y;
            y->right = x->child;
            y->left = x->child->left;
            x->child->left = y;
        } else
            x->child = y;
        // y가 min_node였을 경우, min_node를 x로 수정한다.
        if (y == min_node)
            min_node = x;

        x->degree += 1;
    } else {
        (x->left).lock()->right = x->right;
        (x->right)->left = x->left;
        x->parent = y;

        if (y->degree) {
            (y->child->left).lock()->right = x;
            x->right = y->child;
            x->left = y->child->left;
            y->child->left = x;
        } else
            y->child = x;

        if (x == min_node)
            min_node = y;

        y->degree += 1;
    }
}

template <typename T>
void FibonacciHeap<T>::cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
    // x가 parent의 child였을 경우,
    if ((x->parent).lock()->child == x)
        (x->parent).lock()->child = x->child;

    // x의 child가 존재할 경우, x의 parent와 child간의 관계를 만들어 준다.
    if (x->degree) {
        // 기존의 x의 child를 x의 parend의 child
    }
}

template <typename T>
void FibonacciHeap<T>::recursive_cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO

}

#endif // __FHEAP_H_
