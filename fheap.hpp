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
        // ~FibonacciNode() { std::cout << "----delete " << key << "----" << std::endl; }

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
        void destruct_helper(std::shared_ptr<FibonacciNode<T>>& x);
};

template <typename T>
FibonacciHeap<T>::~FibonacciHeap() {
	// TODO
	// NOTE: Be aware of memory leak or memory error.
    destruct_helper(min_node);
    min_node = nullptr;
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
        // std::cout << "initial inserted!, key : " << node->key << std::endl;
    } else {
        (min_node->left).lock()->right = node;
        node->right = min_node;
        node->left = min_node->left;
        min_node->left = node;

        if (node->key < min_node->key)
            min_node = node;
        
        // std::cout << "inserted!, key : " << node->key << std::endl;
        // std::cout << "left : " << (node->left).lock()->key << "    right : " << node->right->key << std::endl;
    }
    size_++;
}

template <typename T>
std::optional<T> FibonacciHeap<T>::extract_min() {
	// TODO
    if(is_empty())
        return std::nullopt;
    // std::cout << "-----extract_min-----" << std::endl;

    T min_key = min_node->key;
    bool is_one_root = false;

    // only one node??? ????????? ??????,
    if (size_ == 1) {
        min_node->right = nullptr;
        min_node = nullptr;
        size_--;
        return min_key;
    }

    if (min_node->right == min_node)
        is_one_root = true;

    // min_node??? child?????? ?????? ??????
    // child??? ?????? ??????,
    if (min_node->degree)
    {
        std::shared_ptr<FibonacciNode<T>> child = min_node->child;
        min_node->child = nullptr; // ?????? ???????

        for (int i = 0; i < int(min_node->degree); i++) // ?????? ??????
        {
            (child->parent).reset();
            child = child->right;
        }
        
        // root list??? min_node ?????? ?????? child??? ???????????????.
        if (is_one_root) {
            min_node->right = nullptr;
            min_node = child;
        } else {
            (min_node->left).lock()->right = child;
            (child->left).lock()->right = min_node->right;
            (min_node->right)->left = child->left;
            child->left = min_node->left;
        }
    // child??? ?????? ??????, min_node??? root list?????? ?????? ???????????? ???????????????.
    } else {
        (min_node->left).lock()->right = min_node->right;
        (min_node->right)->left = min_node->left;
    }

    // min_node update, ?????? min_node??? left, right??? ??????????????????? (reference num ?????????????)
    // reference num??? ????????? ???????????????, ????????? ?????????????????? ????????????.
    // ???????????? right??? nullptr?????? left??? ?????? ?????????.
    min_node = min_node->right;
    std::shared_ptr<FibonacciNode<T>> start = min_node;
    std::shared_ptr<FibonacciNode<T>> current = min_node;
    do
    {
        if(current->key < min_node->key)
            min_node = current;
        current = current->right;
    } while (start != current);

    // std::cout << min_node->key << std::endl;

    consolidate();

    size_--; // node??? ????????? 1 ???????????????, (????????? ???????????? ?????? ? ) consolidate ?????? ?????? ?????????
    return min_key;
}

template <typename T>
void FibonacciHeap<T>::decrease_key(std::shared_ptr<FibonacciNode<T>>& x, T new_key) {
	// TODO
    // std::cout << "-----decrease_key-----" << std::endl;
    x->key = new_key;
    // x??? root node??? ??????, return.
    if ((x->parent).lock() == nullptr) {
        // new_key ??? min_node??? key?????? ?????? ??????, min_node??? x??? update ?????????.
        if (new_key < min_node->key) min_node = x;
        return;
    }
    // decrease_key??? min heap??? violate?????? ?????? ??????, key??? ???????????? return.
    if ((x->parent).lock()->key <= new_key) return;

    // violate ??? ??????, 
    cut(x);
}

template <typename T>
void FibonacciHeap<T>::remove(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
    // ?????? min_node??? key?????? ?????? key??? ????????????. *** ????????????
    // std::cout << "-----remove-----" << std::endl;
	T min_key = -INFINITY;
    // decrease_key??? ?????? node??? ?????? ?????? key??? ?????????.
    decrease_key(x, min_key);
    // extract_min??? ?????? ????????????.
    extract_min();
}

template <typename T>
void FibonacciHeap<T>::consolidate() {
	float phi = (1 + sqrt(5)) / 2.0;
	int len = int(log(size_) / log(phi)) + 1;
	// TODO

    // std::cout << "-----consolidate-----" << std::endl;

	std::vector<std::shared_ptr<FibonacciNode<T>>> A(len, nullptr);

    std::shared_ptr<FibonacciNode<T>> start = min_node;
    std::shared_ptr<FibonacciNode<T>> current = min_node;

    int index;
    bool is_merged = false;
    do {
        is_merged = false;
        // std::cout << "-----iter...-----" << std::endl;
        // std::cout << "key : " << current->key << std::endl;
        index = current->degree;
        // std::cout << "degree : " << index << std::endl;
        if (A[index] == nullptr) {
            A[index] = current;
            current = current->right;
        } else {
            if (A[index] != current) {
                // std::cout << "-----call merge-----" << std::endl;
                merge(A[index], current);
                is_merged = true;
                if (current->key >= A[index]->key)
                    current = A[index];
                A[index] = nullptr;
            }
        }
    } while(start != current || is_merged);
    
}

template <typename T>
void FibonacciHeap<T>::merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y) {
	// TODO
    // std::cout << "------merge------" << std::endl;

    if (x->key <= y->key) {
        // y??? root list?????? ????????????, y??? parent??? x??? ?????????. x??? degree??? 1 ??????.
        (y->left).lock()->right = y->right;
        (y->right)->left = y->left;
        y->parent = x;

        // x??? child??? y??? doubly linked ????????? ???????????????.
        if (x->degree) {
            (x->child->left).lock()->right = y;
            y->right = x->child;
            y->left = x->child->left;
            x->child->left = y;
        } else {
            x->child = y;
            y->left = y;
            y->right = y;
        }
        // y??? min_node?????? ??????, min_node??? x??? ????????????.
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
        } else {
            y->child = x;
            x->left = x;
            x->right = x;
        }

        if (x == min_node)
            min_node = y;

        y->degree += 1;
    }
}

template <typename T>
void FibonacciHeap<T>::cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
    // std::cout << "-----cut-----" << std::endl;
    std::shared_ptr<FibonacciNode<T>> parent_node = (x->parent).lock();
    // decrease_key?????? parent??? ?????? ????????? ???????????????, x??? ????????? parent??? ??????.
    // x??? parent??? ????????? child??? ?????? ??????, x??? childlist?????? ????????? ?????????.
    if (parent_node->child != x) {
        (x->left).lock()->right = x->right;
        (x->right)->left = x->left;
    // x??? parent??? ????????? child??? ??????, ????????? child??? ?????????????????????. ????????? nullptr??? ?????????.
    } else {
        // x??? ????????? child??? ??????.
        if (x->right == x)
            parent_node->child = nullptr;
        // x ????????? child??? ?????? ??????
        else {
            (x->left).lock()->right = x->right;
            (x->right)->left = x->left;
            parent_node->child = x->right;
        }
    }
    // x??? root list??? ????????????.
    (min_node->left).lock()->right = x;
    x->right = min_node;
    x->left = min_node->left;
    min_node->left = x;
    (x->parent).reset();

    // x??? marked ??????????????????, unmarked??? ?????????.
    if (x->marked)
        x->marked = false;
    // x??? min_node??? key?????? ?????? ??????, min_node??? x??? update ?????????.
    if (x->key < min_node->key) 
        min_node = x;

    parent_node->degree -= 1;

    recursive_cut(parent_node);
}

template <typename T>
void FibonacciHeap<T>::recursive_cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
    // x??? root list??? ????????? ?????? return.
    // std::cout << "-----recursive cut-----" << std::endl;
    if ((x->parent).lock() == nullptr) return;
    // x??? root list??? ?????? ?????????, marked??? false??? ????????? true???, marked??? true?????? cut??? call??????.
    if (!(x->marked))
        x->marked = true;
    else 
        cut(x);
}

template <typename T>
void FibonacciHeap<T>::destruct_helper(std::shared_ptr<FibonacciNode<T>>& x) {

    if (min_node == nullptr) return;
    
    std::shared_ptr<FibonacciNode<T>> start = x;
    std::shared_ptr<FibonacciNode<T>> current = x;

    do
    {
        if (current->degree) 
            destruct_helper(current->child);

        current = (current->left).lock();
        current->right = nullptr;
    } while (start != current);
}

#endif // __FHEAP_H_
