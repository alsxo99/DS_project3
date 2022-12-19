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
    bool is_one_root = false;

    // only one node만 있었을 경우,
    if (size_ == 1) {
        min_node->right = nullptr;
        min_node = nullptr;
        size_--;
        return min_key;
    }

    if (min_node->right == min_node)
        is_one_root = true;

    // min_node와 child간의 관계 끊기
    // child가 있는 경우,
    if (min_node->degree)
    {
        std::shared_ptr<FibonacciNode<T>> child = min_node->child;
        min_node->child = nullptr; // 필요 없나?

        for (int i = 0; i < int(min_node->degree); i++) // 확인 필요
        {
            (child->parent).reset();
            child = child->right;
        }
        
        // root list에 min_node 대신 그의 child로 수정해준다.
        if (is_one_root) {
            min_node->right = nullptr;
            min_node = child;
        } else {
            (min_node->left).lock()->right = child;
            (child->left).lock()->right = min_node->right;
            (min_node->right)->left = child->left;
            child->left = min_node->left;
        }
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

    std::cout << min_node->key << std::endl;

    consolidate();

    size_--; // node의 개수가 1 줄었으므로, (여기서 줄이는게 맞나 ? ) consolidate 전에 해야 할지도
    return min_key;
}

template <typename T>
void FibonacciHeap<T>::decrease_key(std::shared_ptr<FibonacciNode<T>>& x, T new_key) {
	// TODO
    std::cout << "-----decrease_key-----" << std::endl;
    x->key = new_key;
    // x가 root node인 경우, return.
    if ((x->parent).lock() == nullptr) {
        // new_key 가 min_node의 key보다 작을 경우, min_node를 x로 update 해준다.
        if (new_key < min_node->key) min_node = x;
        return;
    }
    // decrease_key가 min heap을 violate하지 않은 경우, key만 바꿔주고 return.
    if ((x->parent).lock()->key <= new_key) return;

    // violate 한 경우, 
    cut(x);
}

template <typename T>
void FibonacciHeap<T>::remove(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
    // 현재 min_node의 key보다 낮은 key를 생성한다. *** 확인필요
    std::cout << "-----remove-----" << std::endl;
	T min_key = -INFINITY;
    // decrease_key로 해당 node를 가장 낮은 key로 만든다.
    decrease_key(x, min_key);
    // extract_min을 통해 제거한다.
    extract_min();
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

    // merge를 할 경우에 current가 안바뀌어서 지금 문제가 발생한다.
    int index;
    bool is_merged = false;
    do {
        is_merged = false;
        std::cout << "-----iter...-----" << std::endl;
        std::cout << "key : " << current->key << std::endl;
        index = current->degree;
        std::cout << "degree : " << index << std::endl;
        if (A[index] == nullptr) {
            A[index] = current;
            current = current->right;
        } else {
            if (A[index] != current) {
                std::cout << "-----call merge-----" << std::endl;
                merge(A[index], current);
                is_merged = true;
                if (current->key > A[index]->key)
                    current = A[index];
                A[index] = nullptr;
            }
        }
    } while(start != current || is_merged);
    
}

template <typename T>
void FibonacciHeap<T>::merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y) {
	// TODO
    std::cout << "------merge------" << std::endl;

    if (x->key <= y->key) {
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
        } else {
            x->child = y;
            y->left = y;
            y->right = y;
        }
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
    std::cout << "-----cut-----" << std::endl;
    std::shared_ptr<FibonacciNode<T>> parent_node = (x->parent).lock();
    // decrease_key에서 parent가 없는 경우를 걸렀으므로, x는 반드시 parent가 있다.
    // x가 parent에 저장된 child가 아닐 경우, x를 childlist에서 연결을 끊는다.
    if (parent_node->child != x) {
        (x->left).lock()->right = x->right;
        (x->right)->left = x->left;
    // x가 parent에 저장된 child일 경우, 새로운 child를 등록해줘야한다. 없다면 nullptr을 넣는다.
    } else {
        // x가 유일한 child인 경우.
        if (x->right == x)
            parent_node->child = nullptr;
        // x 이외의 child가 있는 경우
        else {
            (x->left).lock()->right = x->right;
            (x->right)->left = x->left;
            parent_node->child = x->right;
        }
    }
    // x를 root list에 추가한다.
    (min_node->left).lock()->right = x;
    x->right = min_node;
    x->left = min_node->left;
    min_node->left = x;
    (x->parent).reset();

    // x가 marked 되어있었다면, unmarked로 바꾼다.
    if (x->marked)
        x->marked = false;
    // x가 min_node의 key보다 작은 경우, min_node를 x로 update 해준다.
    if (x->key < min_node->key) 
        min_node = x;

    parent_node->degree -= 1;

    recursive_cut(parent_node);
}

template <typename T>
void FibonacciHeap<T>::recursive_cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
    // x가 root list에 있으면 그냥 return.
    std::cout << "-----recursive cut-----" << std::endl;
    if ((x->parent).lock() == nullptr) return;
    // x가 root list에 있지 않으면, marked가 false인 경우엔 true로, marked가 true라면 cut를 call한다.
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
