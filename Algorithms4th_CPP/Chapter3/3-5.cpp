#include"ST.h"
#pragma once
/**
 *Set
 *
 *API
 * SET() 创建一个空集合
 * void add(Key key) 将键key加入集合
 * void delete(Key key) 从集合中删除键key
 * bool contains(Key key) 键key是否在集合中
 * bool isEmpty()
 * int size()
 */

template<typename Key>
class SET
{
private:
	RedBlackBST<Key, int> st;

public:
	SET(Key nullkey=Key()) :st(nullkey,-10){}

	void add(Key key)
	{
		st.put(key, 1);
	}

	bool contains(Key key) { return st.contains(key); }

	void deleteKey(Key key) { return st.deleteKey(key); }

	int size() { return st.size(); }
	
	bool isEmpty() { return st.isEmpty(); }

	std::vector<Key> toArray()
	{
		auto v = st.toArray();
		
		std::vector<Key> ret;

		for (auto item : v)
			ret.push_back(item.key);

		return ret;
	}

public:

	SET unions(const SET& that)
	{
		SET<Key> ret;

		auto v = toArray();
		for (auto item : v)
			ret.add(item);

		v = that.toArray();
		for (auto item : v)
			ret.add(item);

		return ret;
	}

	SET intersects(const SET &that)
	{
		SET ret;
		if (size() < that.size())
		{
			auto v = toArray();
			for (auto x : v)
				if (that.contains(x))
					ret.add(x);
		}
		else
		{
			auto v = that.toArray();
			for (auto x : v)
				if (contains(x))
					ret.add(x);
		}

		return ret;
	}

	SET complement(const SET &all)
	{
		SET ret;

		auto v = all.toArray();

		for (auto x : v)
			if (!contains(x))
				ret.add(x);

		return ret;
	}
};

/**
 *3.5.9 3.5.10 对于BST/RedBlackST，允许重复键
 *
 *只需要修改put和delete即可
 *只要我们把所有的键都设为后继即可，这样不停地删除就可以了
 */

template<typename Key, typename Value>
class RedBlackMultiBST
{
private:
	static const bool RED = true;
	static const bool BLACK = false;
	struct Node
	{//默认按值拷贝即可
		Key key;
		Value value;
		Node* left;
		Node* right;
		bool color;
		int size;
	};

	Node *root = nullptr;

public:

	RedBlackMultiBST()
	{
	}



	//------------------------------------拷贝控制---------------------------------------------------------------------
	//拷贝构造，按值拷贝
	RedBlackMultiBST(const RedBlackMultiBST& rhs)
	{
		constructor(rhs.root);
	}

	RedBlackMultiBST& operator=(const RedBlackMultiBST& rhs)
	{
		RedBlackMultiBST tmp(rhs);
		destructor(root);
		root = tmp.root;
		cache = nullptr;
		return *this;
	}

	RedBlackMultiBST(RedBlackMultiBST &&rhs) noexcept
	{
		root = rhs.root;
		cache = rhs.cache;
		rhs.root = nullptr;
		rhs.cache = nullptr;
	}

	RedBlackMultiBST& operator=(RedBlackMultiBST&& rhs) noexcept
	{
		if (this != &rhs)
		{
			destructor(root);
			root = rhs.root;
			cache = rhs.cache;
			rhs.root = nullptr;
			rhs.cache = nullptr;
		}
		return *this;
	}

	~RedBlackMultiBST()
	{
		destructor(root);
	}



	//-----------------------------------红黑树与二叉查找树的不同点在于：put和delete操作--------------------------------------------
private:
	//--------------------几个辅助函数--------------------------------------
	bool isRed(Node *x)
	{
		if (x == nullptr) return false;//空链接为黑
		return x->color == RED;
	}

	//将红色右链接转为红色左链接
	Node* rotateLeft(Node *h)
	{
		_ASSERT(isRed(h->right));

		Node *x = h->right;
		h->right = x->left;
		x->left = h;
		x->color = h->color;
		h->color = RED;
		x->size = h->size;
		h->size = 1 + size(h->left) + size(h->right);

		return x;
	}

	//将红色左链接转换为红色右链接
	Node* rotateRight(Node *h)
	{
		_ASSERT(isRed(h->left));

		Node *x = h->left;
		h->left = x->right;
		x->right = h;
		x->color = h->color;
		h->color = RED;
		x->size = h->size;
		h->size = 1 + size(h->left) + size(h->right);

		return x;
	}

	void flipColors(Node *h)
	{
		h->color = !h->color;
		h->left->color = !h->left->color;
		h->right->color = !h->right->color;
	}
	//---------------------End-----------------------------------

	void get(Node* x, std::vector<Value> &v, Key key)
	{
		if (x == nullptr) return;

		if (key < x->key) get(x->left, v, key);
		else if (x->key < key) get(x->right, v, key);
		else
		{
			get(x->left, v, key);
			v.push_back(x->value);
			get(x->right, v, key);
		}
	}


	/**
	*put操作采用的是自下而上的方法，允许2-3树，一旦发现临时的4-结点，立刻旋转、分解、上传
	*/
	Node* put(Node* h, Key key, Value val)
	{
		if (h == nullptr) return createNode(key, val, 1, RED);

		if (key < h->key) h->left = put(h->left, key, val);
		else if (h->key <= key) h->right = put(h->right, key, val);


		if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
		if (isRed(h->left) && isRed(h->left->left))h = rotateRight(h);//必然合理，因为h肯定不是叶子，叶子在第一行就返回了
		if (isRed(h->left) && isRed(h->right)) flipColors(h);

		h->size = size(h->left) + size(h->right) + 1;
		return h;
	}




	Node* deleteMin(Node *h)
	{
		if (h->left == nullptr)
		{
			deleteNode(h);
			return nullptr;//删除
		}

		if (!isRed(h->left) && !isRed(h->left->left))
			h = moveRedLeft(h);

		h->left = deleteMin(h->left);
		return balance(h);
	}

	Node* moveRedLeft(Node *h)
	{
		//h必然不是2-结点
		//处理case3的两种情况，使得h的左子结点不是2-结点

		//对于case3.2，只需要将h代表的3-/4-结点的最小元素（此处就是h），左子结点的最近兄弟（此处是h.right)和左子结点合并为4-结点即可
		flipColors(h);

		//对于case3.1：
		if (isRed(h->right->left))//必然合理，根据moveRedLeft的调用条件，h.right必然存在
		{//先执行flipColors(h)
			h->right = rotateRight(h->right);
			h = rotateLeft(h);
			flipColors(h);
		}

		return h;
	}

	Node* balance(Node *h)
	{

		if (isRed(h->right)) h = rotateLeft(h);
		if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
		if (isRed(h->left) && isRed(h->right)) flipColors(h);

		h->size = size(h->left) + size(h->right) + 1;
		return h;
	}

	Node* deleteMax(Node *h)
	{//删除最大值，因为红链接都是左链接，所以我们在整个过程中，需要把红链接转换为右链接
	 //其他的情况跟删除最小值接近
		if (isRed(h->left))
			h = rotateRight(h);

		if (h->right == nullptr)
		{
			deleteNode(h);
			return nullptr;
		}
		if (!isRed(h->right) && !isRed(h->right->left))
			h = moveRedRight(h);

		h->right = deleteMax(h->right);
		return balance(h);
	}

	Node* moveRedRight(Node *h)
	{
		flipColors(h);

		if (isRed(h->left->left))
		{
			h = rotateRight(h);
			flipColors(h);
		}
		return h;
	}

	Node* deleteKey(Node *h, Key key)
	{//当前结点不为2-结点
	 //如果向左边搜索，那么确保当前结点的左子结点不为2-结点
	 //如果向右边搜索，那么首先更改当前结点为右链接，然后确保当前结点的右子结点不为2-结点
	 //如果删除当前结点：那么也是同样的：确保右子结点不为2-结点，以此为初始条件，deleteMin（h.right）
		if (key < h->key)
		{
			if (!isRed(h->left) && !isRed(h->left->left))
				h = moveRedLeft(h);
			h->left = deleteKey(h->left, key);
		}
		else
		{
			if (isRed(h->left))
				h = rotateRight(h);
			if (key == h->key &&h->right == nullptr)
			{
				deleteNode(h);
				return nullptr;
			}

			if (!isRed(h->right) && !isRed(h->right->left))
				h = moveRedRight(h);

			if (key == h->key)
			{
				//先删除前驱，再删除后继
				Node *x = max(h->left);
				while (x != nullptr && x->key == key)
				{
					h->left = deleteMax(h->left);
					x = max(h->left);
				}

				x = min(h->right);
				while (x != nullptr && x->key == key)
				{
					h->right = deleteMin(h->right);
					x = min(h->right);
				}

					
				x = min(h->right);
				h->key = x->key;
				h->value = x->value;
				h->right = deleteMin(h->right);
			}
			else h->right = deleteKey(h->right, key);
		}

		return balance(h);
	}
	//-------------------------------接口函数-----------------------------------------------------------------------------
public:
	bool isEmpty() { return root == nullptr; }

	bool contains(Key key) { return get(key) != Value(); }

	std::vector<Value> get(Key key)
	{
		std::vector<Value> ret;
		get(root, ret,key);
		return ret;
	}

	void put(Key key, Value val)
	{
		root = put(root, key, val);
		root->color = BLACK;
	}

	void deleteMin()
	{
		if (!isRed(root->left) && !isRed(root->right)) root->color = RED;//保证当前结点不是2-结点

		root = deleteMin(root);
		if (!isEmpty()) root->color = BLACK;
	}

	void deleteMax()
	{
		if (!isRed(root->left) && !isRed(root->right)) root->color = RED;
		root = deleteMax(root);
		if (!isEmpty()) root->color = BLACK;
	}

	void deleteKey(Key key)
	{
		if (!isRed(root->left) && !isRed(root->right)) root->color = RED;
		root = deleteKey(root, key);
		if (!isEmpty()) root->color = BLACK;
	}

	Key min() { return min(root)->key; }

	Key max() { return max(root)->key; }

	Key floor(Key key)
	{
		Node* x = floor(root, key);
		if (x == nullptr) return Value();
		return x->key;
	}

	Key ceiling(Key key)
	{
		Node *x = ceiling(root, key);
		if (x == nullptr) return Value();
		return x->key;
	}



	//--------------------------------与size有关的方法----------------------------------------------------------
#ifndef NORDERMETHOD
public:

	int size() { return size(root); }

	Key select(int k)
	{
		return select(root, k)->key;
	}

	int rank(Key key)
	{
		return rank(root, key);
	}
#endif



	//-------------------------------与遍历有关---------------------------------------------------------------------------------
public:

	struct Item
	{
		Key key;
		Value value;
	};

	std::vector<Item> toArray() { return toArray(min(), max()); }

	std::vector<Item> toArray(const Key lo, const Key hi)
	{
		std::vector<Item> ret;
		toArray(root, ret, lo, hi);
		return ret;
	}


	//--------------------------------与测试有关-----------------------------------------------------------------------------
public:
	bool is23Tree() { return is23Tree(root); }

	bool isBalanced()
	{
		int black = 0;//number of black links on path from root to min
		Node *x = root;
		while (x != nullptr)
		{
			if (!isRed(x)) ++black;
			x = x->left;
		}
		return isBalanced(root, black);
	}

private:

	bool is23Tree(Node *h)
	{
		if (h == nullptr) return true;
		if (isRed(h->right)) return false;
		if (h != root &&isRed(h) && isRed(h->left))
			return false;
		return is23Tree(h->left) && is23Tree(h->right);
	}

	bool isBalanced(Node *x, int black)
	{
		if (x == nullptr) return black == 0;
		if (!isRed(x)) --black;
		return isBalanced(x->left, black) && isBalanced(x->right, black);
	}

	//--------------------------------接口函数的实现1-------------------------------------------------------------------------
#ifndef NORDERMETHOD
private:
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
		else
		{
			return x;
		}
	}

	int rank(Node *x, Key key)
	{
		if (x == nullptr) return 0;
		if (key < x->key) return rank(x->left, key);
		else if (x->key < key) return 1 + size(x->left) + rank(x->right, key);
		else return size(x->left);
	}
#endif



	//------------------------------与接口函数有关的实现2-------------------------------------------------------------------------
private:

	Node* min(Node* x)
	{
		if (x == nullptr) return nullptr;
		if (x->left == nullptr) return x;
		else return min(x->left);
	}

	Node* max(Node* x)
	{
		if (x == nullptr) return nullptr;
		if (x->right == nullptr) return x;
		else return max(x->right);
	}

	Node* floor(Node* x, Key key)
	{
		if (x == nullptr) return nullptr;
		if (key < x->key) return floor(x->left);
		else if (x->key == key) return x;

		Node *t = floor(x->right, key);
		if (t != nullptr) return t;
		else return x;
	}

	Node* ceiling(Node *x, Key key)
	{
		if (x == nullptr) return nullptr;
		if (x->key == key) return x;
		else if (x->key < key) return ceiling(x->right, key);

		Node *t = ceiling(x->left, key);
		if (t == nullptr) return x;
		else return t;
	}

	void toArray(Node *x, std::vector<Item> &q, const Key lo, const Key hi)
	{
		if (x == nullptr)return;
		if (x->key > lo) toArray(x->left, q, lo, hi);
		if (x->key >= lo && x->key <= hi) q.push_back({ x->key,x->value });
		if (x->key < hi) toArray(x->right, q, lo, hi);

	}



	//----------------------------------辅助函数-----------------------------------------------------------------------------

	void destructor(Node *x)
	{
		if (x == nullptr) return;
		destructor(x->left);
		destructor(x->right);
		deleteNode(x);
	}

	Node* constructor(Node* x)
	{
		if (x == nullptr) return;

		auto left = constructor(x->left);
		auto right = constructor(x->right);
		auto ret = copyNode(x);
		ret->left = left;
		ret->right = right;
		return ret;
	}



	//--------------------------------------内存分配有关-------------------------------------------------------------------------
private:
	static std::allocator<Node> alloc;
	Node* createNode(const Key &key, const Value &val, const int n, bool col)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&(p->key), key);//注意一定是取地址
		alloc.construct(&(p->value), val);
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->color), col);
		alloc.construct(&(p->size), n);

		return p;
	}
	Node* createNode(Key &&key, Value &&val, const int n, bool col)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&(p->key), std::move(key));
		alloc.construct(&(p->value), std::move(val));
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->color), col);
		alloc.construct(&(p->size), n);
		return p;
	}
	void deleteNode(Node* node)
	{
		alloc.destroy(&(node->key));
		alloc.destroy(&(node->value));
		alloc.deallocate(node, 1);
	}

};

template<typename Key, typename Value> std::allocator<typename RedBlackMultiBST<Key, Value>::Node> RedBlackMultiBST<Key, Value>::alloc;

/**
 *3.5.8 & 3.5.19
 *hashTable MultiST
 */
#include<unordered_map>
template<typename Key, typename Value, typename hashFunc = std::hash<Key>>
class SeparateChainingHashMultiST
{
public:
	struct Item
	{
		Key key;
		Value value;
	};

private:
	static const int INIT_CAPACITY = 4;
	int N = 0;//键值对总数
	int M;//散列表大小
	std::vector<std::forward_list<Item>> st;



public:
	SeparateChainingHashMultiST() :SeparateChainingHashMultiST(997) {}

	SeparateChainingHashMultiST(int m) :M(m), st(m) {}

	int size() { return N; }

	bool isEmpty() { return N == 0; }

	bool contains(Key key) { return get(key) != Value(); }

	Value get(Key key)
	{
		size_t i = hash(key);
		for (auto x = st[i].begin(); x != st[i].end(); ++x)
			if (x->key == key) return x->value;

		return Value();
	}

	std::vector<Value> getAll(Key key)
	{
		std::vector<Value> ret;
		size_t i = hash(key);
		for (auto x = st[i].begin(); x != st[i].end(); ++x)
			if (x->key == key) ret.push_back(x->value);
		return ret;
	}

	void put(Key key, Value val)
	{
		if (N >= 10 * M) resize(2 * M);
		size_t i = hash(key);
		++N;

		st[i].push_front({ key,val });
	}

	void deleteKey(Key key)
	{
		size_t i = hash(key);

		auto begin = st[i].begin();
		auto prev = st[i].before_begin();
		for (auto it = begin; it != st[i].end(); ++it)
		{
			if (it->key == key)
			{
				st[i].erase_after(prev);
				it = prev;
			}
			prev = it;
		}

		if (M > INIT_CAPACITY&& N <= 2 * M) resize(M / 2);
	}

	std::vector<Item> toArray()
	{
		std::vector<Item> ret;
		for (int i = 0; i < M; ++i)
			for (auto x = st[i].begin(); x != st[i].end(); ++x)
				ret.push_back({ x->key,x->value });
		return ret;
	}

private:
	void resize(int cap)
	{
		SeparateChainingHashMultiST temp(cap);
		for (int i = 0; i < M; ++i)
			for (auto x = st[i].begin(); x != st[i].end(); ++x)
				temp.put(x->key, x->value);


		M = cap;
		st = std::move(temp.st);
	}

	size_t hash(Key key)
	{
		hashFunc h;
		return (h(key)&0x7fffffff) % M;
	}


};

/**
 *3.5.24 不重叠的区间查找
 *只要重新定义一种数据类型即可
 */
template<typename T>
class Interval
{
private:
	T lo;
	T hi;

public:
	Interval(T t1 = T(), T t2 = T()) :lo(t1), hi(t2) {}

	Interval(T t) :lo(t), hi(t) {}

	bool operator<(const Interval& rhs)
	{
		return hi < rhs.lo;
	}
	bool operator<=(const Interval& rhs)
	{
		return (*this<rhs)||(*this==rhs);
	}
	bool operator>(const Interval &rhs)
	{
		return lo > rhs.hi;
	}
	bool operator>=(const Interval &rhs)
	{
		return (*this>rhs) || (*this == rhs);
	}
	bool operator==(const Interval &rhs)
	{
		return (lo <= rhs.lo) && (rhs.hi <= hi);
	}
	bool operator!=(const Interval &rhs)
	{
		return !(*this == rhs);
	}

};
#include<iostream>
/*
int main()
{
	RedBlackBST<Interval<int>, int> st;
	st.put(Interval<int>(1643, 2033),1);
	st.put(Interval<int>(5532, 7643), 2);
	st.put(Interval<int>(8999, 10332), 3);
	st.put(Interval<int>(5666653, 5669321), 4);

	std::cout << st.get(Interval<int>(9122,9122));




	system("pause");

}*/

