#pragma once
/**
 *泛型ST（符号表）的API
 *
 * ST() 创建一张符号表
 * void put(Key key,Value val) 将键值对放入表中
 * Value get(Key key)
 * void delete(Key key)
 * bool contains(Key key) 键key是否在表中有对应的值
 * bool isEmpty() 表是否为空
 * int size() 表中键值对的数量
 * 
 *我们需要处理几个问题：
 *1.重复键怎么处理？
 *  每个键只对应一个值；
 *  当键值对已存在时，用新的值来替换旧的值
 *2.如何处理删除操作？
 *  延时删除
 *  即时删除
 *3.如何处理键的等价性
 */

/**
 *上面所说的是泛型ST的API
 *许多符号表都利用了键的有序性来实现put和get
 *这种有序性会极大地扩展ST的API，定义更多实用的操作
 *
 *扩展API
 *
 * Key min() 最小的键
 * Key max() 最大的键
 * Key floor(Key key) 小于等于key的最大键
 * Key ceiling(Key key) 大于等于key的最小键
 * int rank(Key key) 小于key的键的数量
 * Key select(int k) 排名为k的键
 *
 * void deleteMin() 删除最小的键
 * void deleteMax() 删除最大的键
 * int size(Key lo, Key hi) [lo..hi]之间键的数量
 *
 *综上，我们可以看到，都是关于键的位置的一些操作，重点是rank（）操作
 *i==rank(select(i)) & key==select(rank(key))
 */


/**
 *  The {@code SequentialSearchST} class represents an (unordered)
 *  symbol table of generic key-value pairs.
 *  
 *  The <em>put</em> and <em>delete</em> operations take linear time; the
 *  <em>get</em> and <em>contains</em> operations takes linear time in the worst case.
 *  The <em>size</em>, and <em>is-empty</em> operations take constant time.
 *  Construction takes constant time.
 */

//由于C++里没有null这一数值，所以使得程序不再简单
#include<forward_list>
template<typename Key, typename Value>
class SequentialSearchST
{
public:
	struct Item
	{
		Key key;
		Value value;
	};

private:
	std::forward_list<Item> list;
	int N = 0;

public:
	typedef typename std::forward_list<Item>::iterator Iterator;

	int size() { return N; }
	bool isEmpty() { return N == 0; }
	bool contains(Key key) { return get(key) != Value(); }
	
	Value get(Key key)
	{
		auto prev = list.before_begin();
		for (auto it = list.begin(); it != list.end(); ++it,++prev)
			if (it->key == key)
			{//命中查找：使用自组织缓存
				auto ret = it->value;
				list->erase_after(prev);
				list.push_front({ key,ret });
				return ret;
			}
		return Value(); //没有找到，返回什么呢...
	}

	void put(Key key, Value val)
	{
		for (auto it = list.begin(); it != list.end(); ++it)
			if (it->key == key)
			{
				it->value = val;
				return;
			}

		list.push_front({ key,val });
		++N;
		
	}

	void deleteKey(Key key)
	{
		auto prev = list.before_begin();
		for (auto it = list.begin(); it != list.end(); ++it,++prev)
			if (it->key == key)
			{
				list.erase_after(prev);
				--N;
				return;
			}
	}

	Iterator begin() { return list.begin(); }
	Iterator end() { return list.end(); }

};

/**
*  The {@code BST} class represents an ordered symbol table of generic
*  key-value pairs.
*  <p>
*  This implementation uses a sorted array. It requires that
*  The <em>put</em> and <em>remove</em> operations each take linear time in
*  the worst case; the <em>contains</em>, <em>ceiling</em>, <em>floor</em>,
*  and <em>rank</em> operations take logarithmic time; the <em>size</em>,
*  <em>is-empty</em>, <em>minimum</em>, <em>maximum</em>, and <em>select</em>
*  operations take constant time. Construction takes constant time.
*  <p>
*/
/**
 *对于ST的有序实现，rank起到重要作用
 */
#include<vector>
template<typename Key, typename Value>
class BinarySearchST
{
public:
	struct Item
	{
		Key key;
		Value value;
	};

private:
	std::vector<Item> items;
	int N = 0;
	int cache = -1;//缓存

public:
	typedef typename std::vector<Item>::iterator Iterator;
	BinarySearchST()
		:BinarySearchST(2)
	{
	}
	BinarySearchST(int cap)
		:items(cap)
	{
	}

	int size() { return N; }
	bool isEmpty() { return N == 0; }
	bool contains(Key key) { return get(key) != Value(); }

	Value get(Key key)
	{
		if (isEmpty())
			return Value();

		int i = rank(key);
		if (i < N && items[i].key == key)
		{//加缓存
			cache = i;
			return items[i].value;
		}
			
		return Value();
	}

	int rank(Key key)
	{
		if (cache >= 0)
		{//缓存技术
			if (items[cache].key == key)
				return cache;
		}

		int lo = 0, hi = N - 1;
		while (lo <= hi)
		{
			int mid = lo + (hi - lo) / 2;
			if (items[mid].key < key) lo = mid + 1;
			else if (items[mid].key == key) return mid;
			else hi = mid - 1;
		}
		return lo;
	}

	void put(Key key, Value val)
	{	
		
		if (N == items.capacity()) resize(2 * N);


		if (N>0 && key > max())
		{//3.1.28 有序插入
			items[N++] = { key,val };
			return;
		}
		
		int i = rank(key);

		for (int j = size(); j > i; --j)
			items[j] = items[j - 1];
		items[i] = { key,val };
		++N;
	}

	void deleteKey(Key key)
	{
		if (isEmpty()) return;

		int i = rank(key);

		if (i == size() || items[i].key != key) return;

		for (int j = i; j < size() - 1; ++j)
			items[j] = items[j + 1];
		--N;
	}

	void deleteMin() { deleteKey(min()); }
	void deleteMax() { deleteKey(max()); }
	Key min() { return items[0].key; }
	Key max() { return items[N - 1].key; }

	Key select(int k) { return items[k].key; }
	Key floor(Key key)
	{
		int i = rank(key);
		if (i < N && items[i].key == key) return items[i].key;
		if (i == 0) return Key();
		else return items[i - 1].key;
	}
	Key ceiling(Key key)
	{
		inr i = rank(key);
		if (i == n) return Key();
		else return items[i].key;
	}

	int size(Key lo, Key hi)
	{
		if (hi < lo) return 0;
		if (contains(hi)) return rank(hi) - rank(lo) + 1;
		else return rank(hi) - rank(lo);

	}

	Iterator begin() { return items.begin(); }
	Iterator end() { return items.end(); }

private:
	void resize(int cap)
	{
		std::vector<Item> temp(cap);

		for (int i = 0; i < items.size(); ++i)
			temp[i] = std::move(items[i]);

		items = std::move(temp);
	}
};


/**
 *我们需要同时能够支持高效的查找和插入两种操作的符号表实现
 *
 *要支持高效的插入操作：我们似乎需要一种链式结构
 *但是单链接的链表是无法使用二分查找法的，因为二分查找的高效来源于能够快速通过索引取得任何子数组的中间元素
 *为了将二分查找的效率和链表的灵活性结合起来，我们需要更加复杂的数据结构。能够同时拥有两者的就是二叉查找树
 */

/**
 *二叉查找树
 *基本的二叉查找树的实现常常是非递归的
 *二叉查找树和快速排序几乎就是“双胞胎”。树的根结点就是快速排序中的第一个切分元素，而这对于所有的子树同样适用，这和快速排序中对于
 *子数组的递归排序完全对应。
 *
 *许多程序员否偏爱基于二叉查找树的符号表的原因是它还支持高效的rank、select、delete以及范围查找等操作
 *
 */
#include<allocators>
#include<algorithm>
template<typename Key, typename Value>
class BST
{
private:
	struct Node
	{//默认按值拷贝即可
		Key key;
		Value value;
		Node* left;
		Node* right;
		int size;
	};

	Node *root = nullptr;
	Node *cache = nullptr;//缓存
public:
	BST()
	{
	}
	//拷贝构造，按值拷贝
	BST(const BST& rhs)
	{
		constructor(rhs.root);
	}
	BST& operator=(const BST& rhs)
	{
		BST tmp(rhs);
		destructor(root);
		root = tmp.root;
		cache = nullptr;
		return *this;
	}
	BST(BST &&rhs) noexcept
	{
		root = rhs.root;
		cache = rhs.cache;
		rhs.root = nullptr;
		rhs.cache = nullptr;
	}
	BST& operator=(BST&& rhs) noexcept
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
	~BST()
	{
		destructor(root);
	}


	bool isEmpty() { return root == nullptr; }
	bool contains(Key key) { return get(key) != Value(); }
	//注意递归算法与递归的接口
	Value get(Key key) 
	{
		//加缓存
		if (cache && cache->key == key) return cache->value;
		return get(root, key); 
	}
	/**
	 *注意这种技法
	 *返回的是以参数为根结点的新子树的根结点，如此则可以总览全局，不需要保存父节点
	 *从而将与父节点有关的代码转嫁到函数外
	 */
	void put(Key key, Value val) { root = put(root, key, val); }


	void deleteMin() { root = deleteMin(root); }
	void deleteMax() { root = deleteMax(root); }
	void deleteKey(Key key) { root = deleteKey(root, key); }


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

	//与有序性有关的方法
#ifndef NORDERMETHOD
	int size() { return size(root); }
	Key select(int k) 
	{
		if (cache && size(cache->left) == k) return cache->key;
		return select(root, k)->key; 
	}
	int rank(Key key) 
	{ 
		if (cache && cache->key == key) return size(cache->left);
		return rank(root, key); 
	}
#endif

	//为了方便遍历
	struct Item
	{
		Key key;
		Value value;
	};
	std::vector<Item> toArray() { return toArray(min(), max()); }
	std::vector<Item> toArray(const Key lo, const Key hi)
	{
		std::vector<Item> ret;
		toArray(root, ret,lo,hi);
		return ret;
	}

#ifndef NORDERMETHOD
private:
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
		else
		{
			cache = x;
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

private:

	Value get(Node* x, Key key)
	{
		if (x == nullptr) return Value();
		if ( key < x->key) return get(x->left, key);
		else if (x->key < key) return get(x->right, key);
		else
		{
			cache = x;
			return x->value;
		}
	}

	Node* put(Node* x, Key key, Value val)
	{
		if (x == nullptr) return createNode(key, val, 1);
		if (key < x->key) x->left = put(x->left, key, val);
		else if (x->key < key) x->right = put(x->right, key, val);
		else x->value = val;
#ifndef NORDERMETHOD
		x->size = size(x->left) + size(x->right) + 1;
#endif
		return x;
	}



	Node* min(Node* x)
	{
		if (x->left == nullptr) return x;
		else return min(x->left);
	}
	Node* max(Node* x)
	{
		if (x->right == nullptr) return x;
		else return max(x->right);
	}
	Node* floor(Node* x, Key key)
	{
		if (x == nullptr) return nullptr;
		if (key < x->key) return floor(x->left);
		else if (x->key == key) return x;
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
		if (x->key == key) return x;
		else if (x->key < key) return ceiling(x->right, key);

		Node *t = ceiling(x->left, key);
		if (t == nullptr) return x;
		else return t;
	}



	Node* deleteMin(Node *x)
	{	
		if (x->left == nullptr)
		{
			Node* ret = x->right;
			deleteNode(x);
			return ret;//删除x
		}

		x->left = deleteMin(x->left);
#ifndef NORDERMETHOD
		x->size = size(x->left) + size(x->right) + 1;
#endif
		return x;
	}

	Node* deleteMax(Node *x)
	{
		if (x->right == nullptr)
		{
			Node* ret = x->left;
			deleteNode(x);
			return ret;//删除x
		}

		x->right = deleteMax(x->right);
#ifndef NORDERMETHOD
		x->size = size(x->left) + size(x->right) + 1;
#endif
		return x;
	}

	Node* deleteKey(Node *x, Key key)
	{
		if (x == nullptr) return nullptr;

		if (key < x->key) x->left = deleteKey(x->left, key);
		else if (x->key < key) x->right = deleteKey(x->right, key);
		else
		{//删除x有两种情况，如果只有一个子结点，那么直接用子结点代替x
			//如果拥有两个子结点，那么用x的后继来代替x，x的后继在z的右子树，且必然只有小于等于一个子结点
			if (x->right == nullptr)
			{
				Node *ret = x->left;
				deleteNode(x);
				return ret;//删除x
			}
			if (x->left == nullptr)
			{
				Node *ret = x->right;
				deleteNode(x);
				return ret;
			}

			Node *t = x;
			x = copyNode(min(t->right));
			x->right = deleteMin(t->right);
			x->left = t->left;
			deleteNode(t);
		}
#ifndef NORDERMETHOD
		x->size = size(x->left) + size(x->right) + 1;
#endif
		return x;
	}

	//辅助函数：copy node
	Node* copyNode(Node *x)
	{
		auto ret = createNode(x->key, x->value, x->size);
		ret->left = x->left;
		ret->right = x->right;
		return ret;
	}



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



	void toArray(Node *x, std::vector<Item> &q, const Key lo, const Key hi)
	{
		if (x == nullptr)return;
		if (x->key > lo) toArray(x->left, q, lo, hi);
		if (x->key >= lo && x->key <= hi) q.push_back({ x->key,x->value });
		if (x->key < hi) toArray(x->right, q, lo, hi);

	}





private:
	static std::allocator<Node> alloc;
	Node* createNode(const Key &key, const Value &val, const int n)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&(p->key), key);//注意一定是取地址
		alloc.construct(&(p->value), val);		
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
		alloc.construct(&(p->size), n);

		return p;
	}
	Node* createNode(Key &&key, Value &&val, const int n)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&(p->key), std::move(key));
		alloc.construct(&(p->value), std::move(val));
		alloc.construct(&(p->left), nullptr);
		alloc.construct(&(p->right), nullptr);
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

template<typename Key, typename Value> std::allocator<typename BST<Key, Value>::Node> BST<Key,Value>::alloc;



/**
 *1.红黑树最吸引人的一点是它的实现中最复杂的代码仅限于put和delete方法。二叉查找树中的其他方法仍然可以继续使用，它融合了二叉查找树
 *  高效简洁的查找方法和2-3树中高效的平衡插入算法。
 *2.此处红黑树是2-3树的一种实现方式，1）红链接均为左链接；2）没有任何结点同时与两条红链接相连；3）完美黑色平衡
 *  改变此处红黑树的定义，也可以作为2-3-4树的一种实现方式（见：算法导论）
 *3.使用递归程序实现：如果额外处理发生在递归之前，则为自上至下的处理，如果发生在递归之后，则为自下向上的处理（参考快速排序和归并排序）
 *4.操作以x为参数，返回新的子树，这种方式是递归实现二叉查找树的各种方法的原因
 */


template<typename Key, typename Value>
class RedBlackBST
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
	Node *cache = nullptr;

	Key NULLKEY;
	Value NULLVALUE;


public:
	
	RedBlackBST(Key nullkey=Key(), Value nullvalue=Value())
		:NULLKEY(nullkey),NULLVALUE(nullvalue)
	{

	}



//------------------------------------拷贝控制---------------------------------------------------------------------
	//拷贝构造，按值拷贝
	RedBlackBST(const RedBlackBST& rhs)
	{
		root = constructor(rhs.root);
		NULLKEY = rhs.NULLKEY;
		NULLVALUE = rhs.NULLVALUE;
	}

	RedBlackBST& operator=(const RedBlackBST& rhs)
	{
		RedBlackBST tmp(rhs);
		destructor(root);
		root = tmp.root;
		cache = nullptr;
		return *this;
	}

	RedBlackBST(RedBlackBST &&rhs) noexcept
	{
		root = rhs.root;
		cache = rhs.cache;
		NULLKEY = rhs.NULLKEY;
		NULLVALUE = rhs.NULLVALUE;
		rhs.root = nullptr;
		rhs.cache = nullptr;
	}

	RedBlackBST& operator=(RedBlackBST&& rhs) noexcept
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

	~RedBlackBST()
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

	Value get(Node* x, Key key)
	{
		if (x == nullptr) return NULLVALUE;
		if (key < x->key) return get(x->left, key);
		else if (x->key < key) return get(x->right, key);
		else
		{
			cache = x;
			return x->value;
		}
	}

	/**
	 *put操作采用的是自下而上的方法，允许2-3树，一旦发现临时的4-结点，立刻旋转、分解、上传
	 */
	Node* put(Node* h, Key key, Value val)
	{
		if (h == nullptr) return createNode(key, val, 1, RED);

		if (key < h->key) h->left = put(h->left, key, val);
		else if (h->key < key) h->right = put(h->right, key, val);
		else h->value = val;

		if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
		if (isRed(h->left) && isRed(h->left->left))h = rotateRight(h);//必然合理，因为h肯定不是叶子，叶子在第一行就返回了
		if (isRed(h->left) && isRed(h->right)) flipColors(h);

		h->size = size(h->left) + size(h->right) + 1;
		return h;
	}

	/**
	 *1. deleteMin操作采用一些自上而下的操作来使得当前结点的叶结点不是2-结点，通过递归删除之后，再采用一些自下而上的操作来分解掉产生的4-结点
	 *2. 在此操作中，4-结点是允许存在的（不同于put中一旦发现4-结点就分解），等到删除操作完成后，再将查找路径上所有的4-结点分解
	 */



	Node* deleteMin(Node *h)
	{//保证h不是2-结点，因为我们的处理过程是使得左子结点不是2-结点，所以整个过程很容易保证，
		//但是我们需要处理root。如果root是2-结点，那么只需要将其变成红色即可
		

		if (h->left == nullptr)
		{
			deleteNode(h);
			return nullptr;//删除
		}
		//h必然不是2-结点（可能是3-结点，也可能是4-结点）
		//h的情况必然是：1. isRed(h.left),isBlack(h) [isRed(h.right)--对应4-结点，isBlack(h.right)--对应3-结点]
		//             2. isRed(h),isBlack(h.parent)
		//也即：如果h是黑色的话，那么这个3-结点（或4-结点）刚开始；如果h是红色的话，那么这个3-结点（或4-结点）要结束
		
		//所以：
		//case1：h可能是3-/4-结点的开始元素，也可能是结束元素
		//if (!isRed(h)) h->left = deleteMin(h->left);
		//所以：真正执行的情况下是：h is red，此时，h.left必然是black

		//case2：如果h的左子结点不是2-结点,那么，直接进入子结点
		//if(isRed(h->left->left)) h->left=deleteMin(h->left);

		//case3:
		//case3.1: 如果h的左子结点是2-结点，且兄弟结点不是2-结点
		//case3.2：如果h的左子结点是2-结点，且兄弟结点是2-结点
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
		//对于deleteMin：自下往上的过程中，可能存在：2-结点，3-结点，4-结点（根据moveRedLeft可知，4-结点只有一种形态）
		//由于删除操作，可能会存在4-结点删除掉左结点的情况
		//我认为只需要如下代码即可
		//if (h->left==nullptr && isRed(h->right)) h = rotateLeft(h);
		//if (isRed(h->left) && isRed(h->right)) flipColors(h);



		//这些代码对于deleteMax，倒是挺合适的
		//对于deleteMax：可能存在：2-结点，3-结点（right链接），4-结点，由于删除操作，可能存在4-结点删除掉右结点的情况
		//我们要做的工作是：恢复3-结点为left链接，拆分4链接
		//我认为只需要如下代码：
		//if (isRed(h->right)) h = rotateLeft(h);
		//if (isRed(h->left) && isRed(h->right)) flipColors(h);

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
				Node *x = min(h->right);
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

	bool contains(Key key) { return get(key) != NULLVALUE; }

	Value get(Key key)
	{	//加缓存
		if (cache && cache->key == key) return cache->value;
		return get(root, key);
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
		root = deleteKey(root,key);
		if (!isEmpty()) root->color = BLACK;
	}

	Key min() { return min(root)->key; }

	Key max() { return max(root)->key; }

	Key floor(Key key)
	{
		Node* x = floor(root, key);
		if (x == nullptr) return NULLKEY;
		return x->key;
	}

	Key ceiling(Key key)
	{
		Node *x = ceiling(root, key);
		if (x == nullptr) return NULLKEY;
		return x->key;
	}



//--------------------------------与size有关的方法----------------------------------------------------------
#ifndef NORDERMETHOD
public:

	int size() { return size(root); }

	Key select(int k)
	{
		if (cache && size(cache->left) == k) return cache->key;
		return select(root, k)->key;
	}

	int rank(Key key)
	{
		if (cache && cache->key == key) return size(cache->left);
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
			cache = x;
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
		if (x->left == nullptr) return x;
		else return min(x->left);
	}

	Node* max(Node* x)
	{
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
		if (x == nullptr) return nullptr;

		auto left = constructor(x->left);
		auto right = constructor(x->right);
		auto ret = createNode(x->key, x->value, x->size, x->color);
		ret->left = left;
		ret->right = right;
		return ret;
	}



//--------------------------------------内存分配有关-------------------------------------------------------------------------
private:
	static std::allocator<Node> alloc;
	Node* createNode(const Key &key, const Value &val, const int n,bool col)
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

template<typename Key, typename Value> 
std::allocator<typename RedBlackBST<Key, Value>::Node> RedBlackBST<Key, Value>::alloc;



template<typename Key,typename Value,typename hashFunc=std::hash<Key>>
class SeparateChainingHashST
{
private:
	static const int INIT_CAPACITY = 4;
	int N = 0;//键值对总数
	int M;//散列表大小
	std::vector<SequentialSearchST<Key, Value>> st;//存放链表的数组

public:
	SeparateChainingHashST() :SeparateChainingHashST(INIT_CAPACITY) {}

	SeparateChainingHashST(int cap)
		:M(cap)
	{
		for (int i = 0; i < cap; ++i)
			st.push_back(SequentialSearchST<Key,Value>());	
	}

	int size() { return N; }
	bool isEmpty() { return N == 0; }
	
	bool contains(Key key) { return get(Key) != Value(); }

	Value get(Key key)
	{
		int i = hash(key);
		return st[i].get(key);
	}

	void put(Key key, Value val)
	{
		if (N >= 10 * M) resize(2 * M);

		int i = hash(key);
		if (!st[i].contains(key)) ++N;
		st[i].put(key, val);
	}

	void deleteKey(Key key)
	{
		int i = hash(key);
		if (st[i].contains(key)) --N;
		st[i].deleteKey(key);

		if (M > INIT_CAPACITY&& N <= 2 * M) resize(M / 2);
	}

	std::vector<typename SequentialSearchST<Key, Value>::Item> toArray()
	{
		std::vector<typename SequentialSearchST<Key, Value>::Item> ret;

		for (int i = 0; i < M; ++i)
			for (auto item : st[i])
				ret.push_back(item);
		
		return ret;	
	}

private:

	void resize(int cap)
	{
		SeparateChainingHashST temp(cap);
		for (int i = 0; i < M; ++i)
			for (auto item : st[i])
				temp.put(item.key, item.value);

		M = temp.M;
		N = temp.N;
		st = std::move(temp.st);
	}

	int hash(Key key)
	{
		return (hashFunc(key) & 0x7fffffff) % M;
	}

};

template<typename Key,typename Value,typename hashFunc=std::hash<Key>>
class LinearProbingHashST
{

public:
	struct Item
	{
		Key key;
		Value value;
	};

private:
	static const int INIT_CAPACITY = 4;

	int N = 0;
	int M;
	std::vector<Item> st;

public:
	LinearProbingHashST() :LinearProbingHashST(INIT_CAPACITY) {}

	LinearProbingHashST(int m) :M(m), st(M) {}

	int size() { return N; }
	bool isEmpty() { return N == 0; }

	bool contains(Key key)
	{
		return get(key) != Value();
	}

	void put(Key key, Value val)
	{
		if (n >= m / 2) resize(2 * m);

		int i;
		for (i = hash(key); st[i].key != Key();i=(i+1)%M)
			if (st[i].key == key)
			{
				st[i].value = val;
				return;
			}
		st[i] = { key,val };
		++N;
	}

	Value get(Key key)
	{
		for (int i = hash(key); st[i].key != Key(); i = (i + 1) % M)
			if (st[i].key == key)
				return st[i].value;

		return Value();
	}

	void deleteKey(Key key)
	{
		if (!contains(key)) return;

		int i = hash(key);
		while (st[i].key != key)
			i = (i + 1) % M;
		st[i] = { Key(),Value() };

		i = (i + 1) % M;
		while (st[i].key != Key())
		{
			auto item = st[i];
			st[i] = { Key(),Value() };
			--N;
			put(item.key, item.value);
			i = (i + 1) % M;
		}

		--N;
		if (N > 0 && N <= M / 8)resize(M / 2);
	}

	std::vector<typename LinearProbingHashST::Item> toArray()
	{
		std::vector<typename LinearProbingHashST::Item> ret;
		for (int i = 0; i < M; ++i)
			if (st[i].key != Key())
				ret.push_back(st[i]);
		return ret;
	}

private:

	int hash(Key key)
	{
		return (hashFunc(key) & 0x7fffffff) % M;
	}

	void resize(int m)
	{
		LinearProbingHashST temp(m);

		for (int i = 0; i < m; ++i)
			if (st[i].key != Key())
				temp.put(st[i].key, st[i].value);

		M = temp.M;
		st = std::move(temp.st);
	}

};