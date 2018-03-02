#pragma once
//根据list实现过程，我们首先需要的是内存分配器
//第二对于树结构，我们需要实现迭代器
#include<memory>
#include<algorithm>
//1. treenode的分配器
template<typename Key, typename Value>
struct TreeNode
{
	std::pair<Key, Value> item;
	TreeNode* left;
	TreeNode* right;
	TreeNode* parent; //仅仅是用于迭代器
	int size;
private:
	static std::allocator<TreeNode> alloc;
public:
	//create Node分处理左值和右值
	static TreeNode* createNode()
	{
		auto p = alloc.allocate(1);
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->parent), nullptr);
		return p;
	}
	TreeNode* createNode(const Key &key, const Value &val, int n=1)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&((p->item).first), key);//注意一定是取地址
		alloc.construct(&((p->item).second), val);
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->parent), nullptr);
		alloc.construct(&(p->size), n);

		return p;
	}
	TreeNode* createNode(Key &&key, Value &&val, int n=1)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&((p->item).first), std::move(key));//注意一定是取地址
		alloc.construct(&((p->item).second), std::move(val));
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->parent), nullptr);
		alloc.construct(&(p->size), n);
		return p;
	}
	TreeNode* cloneNode()
	{
		auto ret = createNode((this->item).first, (this->item).second, this->size);
		ret->left = this->left;
		ret->right = this->right;
		return ret;
	}
	void deleteNode()
	{
		alloc.destroy(&(this->item));
		alloc.deallocate(this, 1);
	}
};
template<typename Key, typename Value> std::allocator<TreeNode<Key,Value>> TreeNode<Key,Value>::alloc;

template<typename Key, typename Value>
struct RedBlackTreeNode 
{
	static const bool RED = true;
	static const bool BLACK = false;
	bool color;

	std::pair<Key, Value> item;
	RedBlackTreeNode* left;
	RedBlackTreeNode* right;
	RedBlackTreeNode* parent; //仅仅是用于迭代器
	int size;
private:
	static std::allocator<RedBlackTreeNode> alloc;
public:
	//create Node分处理左值和右值
	static RedBlackTreeNode* createNode()
	{
		auto p = alloc.allocate(1);
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->parent), nullptr);
		return p;
	}

	RedBlackTreeNode* createNode(const Key &key, const Value &val, const int n = 1, bool col = RED)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&((p->item).first), key);//注意一定是取地址
		alloc.construct(&((p->item).second), val);
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->parent), nullptr);
		alloc.construct(&(p->size), n);
		p->color = col;
		return p;
	}
	RedBlackTreeNode* createNode(Key &&key, Value &&val, const int n = 1, bool col = RED)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&((p->item).first), std::move(key));//注意一定是取地址
		alloc.construct(&((p->item).second), std::move(val));
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->parent), nullptr);
		alloc.construct(&(p->size), n);
		p->color = col;
		return p;
	}

	RedBlackTreeNode* cloneNode()
	{
		auto ret = createNode((this->item).first, (this->item).second, this->size, this->color);
		ret->left = this->left;
		ret->right = this->right;
		return ret;
	}

	void deleteNode()
	{
		alloc.destroy(&(this->item));
		alloc.deallocate(this, 1);
	}
};

template<typename Key, typename Value> std::allocator<RedBlackTreeNode<Key, Value>> RedBlackTreeNode<Key, Value>::alloc;

//2.树结构的迭代器，为了能够由一个节点开始迭代，必须采用parent来向上追溯
template<typename TreeNode>
struct TreeIterator
{

	TreeNode* node;
	typedef typename std::remove_reference<decltype(node->item)>::type value_type;
	TreeIterator(TreeNode* s) :node(s) {}
	bool operator==(const TreeIterator &rhs) const { return node == rhs.node; }
	bool operator!=(const TreeIterator &rhs) const { return !(*this == rhs); }

	value_type& operator*() const { return node->item; }
	value_type* operator->() const { return &(node->item); }
	TreeIterator& operator++() { increment(); return *this; }
	TreeIterator& operator++(int) { TreeIterator ret = *this; increment(); return ret; }
	TreeIterator& operator--() { decrement(); return *this; }
	TreeIterator& operator--(int) { TreeIterator ret = *this; decrement(); return ret; }
private:
	void increment()
	{
		if (node->right != nullptr) //如果node存在右子树
		{
			node = node->right;
			while (node->left != 0)
				node = node->left;

		}
		else {
			auto parent = node->parent;//找出父节点
			while (node == parent->right)//如果当前node是父节点的右子节点
			{
				node = parent;
				parent = node->parent;
			}//上溯至“不为右子节点”为止。
			if (node->right != parent)
				node = parent;
			//如果node->right == parent，此时发生了特殊情况：我们欲寻找根节点的下一个节点，而恰巧根节点没有右子节点
			//具体参见《STL源码剖析》P216
		}
	}

	void decrement()
	{
		if (node->left != nullptr)//如果有左子树，返回左子树最大值
		{
			auto y = node->left;
			while (y->right != nullptr)
				y = y->right;
			node = y;
		}
		else {//如果没有左子树
			auto y = node->parent;
			while (node == y->left)
			{
				node = y;
				y = node->parent;
			}
			if (node->left != y)
				node = y;
		}
	}
};


template<typename Key, typename Value, typename Node = TreeNode<Key, Value>>
class BST
{
	typedef typename TreeIterator<Node> Iterator;
protected:
	Node* dummy;//类似于singlelist中的dummy
	
//拷贝控制函数
public:
	BST();
	BST(const BST& rhs);
	BST& operator=(const BST& rhs);
	BST(BST &&rhs) noexcept;
	BST& operator=(BST&& rhs) noexcept;
	~BST();
protected:
	void init();
	void destructor(Node *x);
	Node* constructor(Node* x);

//接口函数
public:
	bool isEmpty() { return root() == nullptr; }
	bool contains(const Key &key) { return get(key) != end(); }
	//注意递归算法与递归的接口
	Iterator get(const Key &key) { return get(root(), key); }
	/**
	*注意这种技法
	*返回的是以参数为根结点的新子树的根结点，如此则可以总览全局，不需要保存父节点
	*从而将与父节点有关的代码转嫁到函数外
	*/
	void put(const Key &key, const Value &val) 
	{
		auto r = put(root(), key, val);
		r->parent = dummy;
		dummy->parent = r;
		dummy->left = min(r);
		dummy->right = max(r);
	}
	virtual void deleteMin() 
	{ 
		auto r = deleteMin(root()); 
		r->parent = dummy;
		dummy->parent = r;
		dummy->left = min(r);
		dummy->right = max(r);
	}
	virtual void deleteMax() 
	{ 
		auto r = deleteMax(root()); 
		r->parent = dummy;
		dummy->parent = r;
		dummy->left = min(r);
		dummy->right = max(r);
	}
	virtual void deleteKey(const Key &key) 
	{ 
		auto r = deleteKey(root(), key); 
		r->parent = dummy;
		dummy->parent = r;
		dummy->left = min(r);
		dummy->right = max(r);
	}
	Iterator min() { return min(root()); }
	Iterator max() { return max(root()); }
	Iterator floor(const Key &key)
	{
		Node* x = floor(root(), key);
		if (x == nullptr) return end();
		return x;
	}
	Iterator ceiling(const Key &key)
	{
		Node *x = ceiling(root(), key);
		if (x == nullptr) return end();
		return x;
	}
	int size() { return size(root()); }
	Iterator select(int k) 
	{ 
		Node *x = select(root(), k);
		if (x == nullptr) return end();
		return x;
	}
	int rank(const Key &key) { return rank(root(), key); }

//接口函数的辅助函数
protected:
	Node*& root() const { return dummy->parent; }
public:
	Iterator begin() { return min(); }
	Iterator end() { return dummy; }

//接口函数的辅助函数2
protected:
	//size（）将空链接的值当做0，那么 
	//size(x)=size(x->left)+size(x->right)+1 总是成立
	int size(Node* x)
	{
		if (x == nullptr) return 0;
		else return x->size;
	}

	Node* select(Node *x, int k)
	{
		if (x == nullptr) return nullptr;
		int t = size(x->left);
		if (t > k) return select(x->left, k);
		else if (t < k) return select(x->right, k - t - 1);
		else return x;
	}
	int rank(Node *x, const Key &key)
	{
		if (x == nullptr) return 0;
		if (key < x->item.first) return rank(x->left, key);
		else if (x->item.first < key) return 1 + size(x->left) + rank(x->right, key);
		else return size(x->left);
	}

	Node* floor(Node* x, const Key &key)
	{
		if (x == nullptr) return nullptr;
		if (key < x->item.first) return floor(x->left, key);
		else if (x->item.first == key) return x;
		//如果key大于根结点的键值，那么f
		//当右子树中存在小于key的结点，那么floor在右子树中
		//否则，floor就是根结点
		Node *t = floor(x->right, key);
		if (t != nullptr) return t;
		else return x;
	}
	Node* ceiling(Node *x, Key key)
	{
		if (x == nullptr) return nullptr;
		if (x->item.first == key) return x;
		else if (x->item.first < key) return ceiling(x->right, key);

		Node *t = ceiling(x->left, key);
		if (t == nullptr) return x;
		else return t;
	}

protected://
	Node* get(Node* x, const Key &key)
	{
		if (x == nullptr) return dummy;
		if (key < x->item.first) return get(x->left, key);
		else if (x->item.first < key) return get(x->right, key);
		else return x;
	}

	virtual Node* put(Node* x, const Key &key, const Value &val)
	{//
		if (x == nullptr) return dummy->createNode(key, val, 1);
		if (key < x->item.first)
		{
			x->left = put(x->left, key, val);
			x->left->parent = x;
		}
		else if (x->item.first < key) {
			x->right = put(x->right, key, val);
			x->right->parent = x;
		}
		else x->item.second = val;
		x->size = size(x->left) + size(x->right) + 1;
		return x;
	}

	Node* min(Node* x)
	{
		if (x == nullptr) return x;
		if (x->left == nullptr) return x;
		else return min(x->left);
	}
	Node* max(Node* x)
	{
		if (x == nullptr) return x;
		if (x->right == nullptr) return x;
		else return max(x->right);
	}

	virtual Node* deleteMin(Node *x)
	{
		if (x == nullptr) return nullptr;
		if (x->left == nullptr)
		{
			Node* ret = x->right;
			x->deleteNode();
			return ret;//删除x
		}

		x->left = deleteMin(x->left);
		x->left->parent = x;
		x->size = size(x->left) + size(x->right) + 1;
		return x;
	}

	virtual Node* deleteMax(Node *x)
	{
		if (x == nullptr) return nullptr;
		if (x->right == nullptr)
		{
			Node* ret = x->left;
			x->deleteNode();
			return ret;//删除x
		}
		x->right = deleteMax(x->right);
		x->right->parent = x;
		x->size = size(x->left) + size(x->right) + 1;
		return x;
	}

	virtual Node* deleteKey(Node *x, const Key &key)
	{
		if (x == nullptr) return nullptr;

		if (key < x->item.first)
		{
			x->left = deleteKey(x->left, key);
			x->left->parent = x;
		}
		else if (x->item.first < key) {
			x->right = deleteKey(x->right, key);
			x->right->parent = x;
		}
		else
		{//删除x有两种情况，如果只有一个子结点，那么直接用子结点代替x
		 //如果拥有两个子结点，那么用x的后继来代替x，x的后继在z的右子树，且必然只有小于等于一个子结点
			if (x->right == nullptr)
			{
				Node *ret = x->left;
				x->deleteNode();
				return ret;//删除x
			}
			if (x->left == nullptr)
			{
				Node *ret = x->right;
				x->deleteNode();
				return ret;
			}

			Node *t = x;
			x = min(t->right)->cloneNode();
			x->right = deleteMin(t->right);
			x->right->parent = x;
			x->left = t->left;
			x->left->parent = x;
			t->deleteNode();
		}
		x->size = size(x->left) + size(x->right) + 1;
		return x;
	}
};



//--------------------------------拷贝控制函数-------------------------------------------------------//
template<typename Key, typename Value, typename Node> 
void BST<Key, Value, Node>::destructor(Node *x)
{
	if (x == nullptr) return;
	destructor(x->left);
	destructor(x->right);
	x->deleteNode();
}

template<typename Key, typename Value, typename Node>
Node* BST<Key, Value, Node>::constructor(Node* x)
{
	if (x == nullptr) return nullptr;

	auto left = constructor(x->left);
	auto right = constructor(x->right);
	auto ret = x->cloneNode();
	left->parent = ret;
	right->parent = ret;
	ret->left = left;
	ret->right = right;
	return ret;
}

template<typename Key, typename Value, typename Node>
void BST<Key, Value, Node>::init()
{	
	dummy = Node::createNode();
	dummy->parent = nullptr; //root
	dummy->left = dummy; //left most
	dummy->right = dummy; // right most
}

template<typename Key, typename Value, typename Node>
BST<Key, Value, Node>::BST()
{
	init();
}
template<typename Key, typename Value, typename Node>
BST<Key, Value, Node>::BST(const BST& rhs)
{
	
	init();
	if (rhs.size() > 0)
	{
		auto x = constructor(rhs.root()); //x is root
		x->parent = dummy;
		dummy->left = min(x);
		dummy->right = max(x);
		dummy->parent = x;
	}
}

template<typename Key, typename Value, typename Node>
BST<Key, Value, Node>& BST<Key, Value, Node>::operator=(const BST& rhs)
{
	if (*this != rhs)
	{
		BST tmp(rhs);
		*this = std::move(tmp);
		return *this;
	}

}
template<typename Key, typename Value, typename Node>
BST<Key, Value, Node>::BST(BST &&rhs) noexcept
{
	dummy = rhs.dummy;
	rhs.init();
}

template<typename Key, typename Value, typename Node>
BST<Key, Value, Node>& BST<Key, Value, Node>::operator=(BST&& rhs) noexcept
{
	if (this != &rhs)
	{
		destructor(root());
		dummy = rhs.dummy;
		rhs.init();
	}
	return *this;
}
template<typename Key, typename Value, typename Node>
BST<Key, Value, Node>::~BST()
{
	destructor(root());
}

//RedBlackTree




//红黑树与二叉查找树的不同点在于：put和delete操作
template<typename Key, typename Value, typename Node = RedBlackTreeNode<Key, Value>>
class RedBlackBST :public BST<Key, Value, Node>
{
public:
	//需要测试
	void put(const Key &key, const Value &val)
	{
		BST<Key, Value, Node>::put(key, val);
		root()->color = BLACK;
	}
	bool is23Tree() const { return is23Tree(root()); }



private:
	static const bool RED = true;
	static const bool BLACK = false;
	bool isRed(Node *x) const
	{
		if (x == nullptr) return false;//空链接为黑
		return x->color == RED;
	}
	Node* rotateLeft(Node *h);
	Node* rotateRight(Node *h);
	void flipColors(Node *h);

private:
	virtual Node* put(Node* h, const Key &key, const Value &val)
	{
		if (h == nullptr) return dummy->createNode(key, val, 1, RED);

		if (key < h->item.first)
		{
			h->left = put(h->left, key, val);
			h->left->parent = h;
		}
		else if (h->item.first < key) {
			h->right = put(h->right, key, val);
			h->right->parent = h;
		}
		else h->item.second = val;

		if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
		if (isRed(h->left) && isRed(h->left->left))h = rotateRight(h);
		if (isRed(h->left) && isRed(h->right)) flipColors(h);

		h->size = size(h->left) + size(h->right) + 1;
		return h;
	}

	bool is23Tree(Node *h) const
	{
		if (h == nullptr) return true;
		if (isRed(h->right)) return false;
		if (h != root() &&isRed(h) && isRed(h->left))
			return false;
		return is23Tree(h->left) && is23Tree(h->right);
	}

};

//红黑树三大辅助函数:
template<typename Key, typename Value, typename Node>
Node* RedBlackBST<Key,Value,Node>::rotateLeft(Node *h)
{
	_ASSERT(isRed(h->right));

	Node *x = h->right;
	h->right = x->left; 
	if (h->right != nullptr) h->right->parent = h;
	x->left = h; x->left->parent = x;
	x->color = h->color;
	h->color = RED;
	x->size = h->size;
	h->size = 1 + size(h->left) + size(h->right);

	return x;
}

template<typename Key, typename Value, typename Node>
Node* RedBlackBST<Key, Value, Node>::rotateRight(Node *h)
{
	_ASSERT(isRed(h->left));

	Node *x = h->left;
	h->left = x->right; 
	if (h->left != nullptr)h->left->parent = h;
	x->right = h; x->right->parent = x;
	x->color = h->color;
	h->color = RED;
	x->size = h->size;
	h->size = 1 + size(h->left) + size(h->right);

	return x;
}
template<typename Key, typename Value, typename Node>
void RedBlackBST<Key, Value, Node>::flipColors(Node *h)
{
	h->color = !h->color;
	h->left->color = !h->left->color;
	h->right->color = !h->right->color;
}






