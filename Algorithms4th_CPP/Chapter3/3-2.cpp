#include"ST.h"
#include"queue.h"

/**
 *3.2.13 & 3.2.14 实现BST非递归版本 put、get、min、max、ceiling、floor、select、rank
 */

template<typename Key, typename Value>
class NonrecursiveBST
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

	Node* root;
public:
	NonrecursiveBST()
	{
	}
	~NonrecursiveBST()
	{
		destructor(root);
	}

	bool isEmpty() { return root == nullptr; }
	bool contains(Key key) { return get(key) != Value(); }
	


	Value get(Key key)
	{
		Node *x = root;
		while (x != nullptr)
		{
			if (key < x->key) x = x->left;
			else if (x->key < key) x = x->right;
			else return x->value;
		}
		return Value();
	}

	void put(Key key, Value val)
	{
		auto z = createNode(key, val, 1);

		if (root == nullptr)
		{
			root = z;
			return;
		}

		Node *parent = nullptr;
		Node *x = root;

		while (x != nullptr)
		{
			parent = x;
			if (key < x->key) x = x->left;
			else if (x->key < key) x = x->right;
			else
			{
				x->value = val;
				return;
			}
		}
		
		if (key < parent->key) parent->left = z;
		else parent->right = z;

		//我们还需要额外遍历一遍查找路径，来更新结点计数器
#ifndef NORDERMETHOD
		x = root;
		while (x->key != key)
		{
			x->size += 1;
			if (key < x->key) x = x->left;
			else if (x->key < key) x = x->right;
		}
#endif
	}


	void deleteMin() { root = deleteMin(root); }
	void deleteMax() { root = deleteMax(root); }
	void deleteKey(Key key) { root = deleteKey(root, key); }


	Key min()
	{
		auto x = root;
		Node *pa = nullptr;
		while (x != nullptr)
		{
			pa = x;
			x = x->left;
		}
		return pa->key;
	}

	Key max()
	{
		auto x = root;
		Node *pa = nullptr;
		while (x != nullptr)
		{
			pa = x;
			x = x->right;
		}
		return pa->key;
	}


	Key floor(Key key)
	{
		auto x = root;
		Node *p = nullptr;//candidate node
		Node *c2 = nullptr;
		while (x != nullptr)
		{
			if (key < x->key)
			{
				p = x;
				x = x->left;
			}
			else if (key == x->key) return x->key;
			else
			{//floor要么在x的右子树中，（右子树存在小于floor的键值）；否则就是x
				p = nullptr;
				c2 = x;
				x = x->right;
			}
		}

		if (p != nullptr && p->key < key)
			return p->key;
		else if (c2 != nullptr)
			return c2->key;
		else
			return Key();
	

	}

	Key ceiling(Key key)
	{
		auto x = root;
		Node *p = nullptr;//candidate node
		Node *c2 = nullptr;
		while (x != nullptr)
		{
			if (key > x->key)
			{
				p = x;
				x = x->right;
			}
			else if (key == x->key) return x->key;
			else
			{
				p = nullptr;
				c2 = x;
				x = x->left;
			}
		}

		if (p != nullptr && p->key > key)
			return p->key;
		else if (c2 != nullptr)
			return c2->key;
		else
			return Key();

	}

	//与有序性有关的方法
#ifndef NORDERMETHOD
	int size()
	{
		size(root);
	}

	int size(Node *x)
	{
		if (x == nullptr) return 0;
		else return x->size;
	}

	Key select(int k)
	{
		auto x = root;
		while (x != nullptr)
		{
			int t = size(x->left);
			if (t > k) x = x->left;
			else if (t == k) return x->key;
			else
			{
				x = x->right;
				k = k - t - 1;
			}
		}

		return Key();
	}

	int rank(Key key)
	{
		auto x = root;
		int r = 0;
		while (x != nullptr)
		{
			if (key < x->key) x = x->left;
			else if (key == x->key) return size(x->left);
			else
			{
				r = r + size(x->left) + 1;
				x = x->right;
			}
		}

		return r;
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
		toArray(root, ret, lo, hi);
		return ret;
	}
	/**
	 *3.3.37 按层遍历
	 */
	std::vector<Item> toArrayByLevel()
	{
		std::vector<Item> ret;
		Queue<Node*> q;
		q.enqueue(root);
		while (!q.isEmpty())
		{
			auto x = q.dequeue();
			if (x == nullptr) continue;
			ret.push_back({ x->key,x->value });

			if (x->left != nullptr) q.enqueue(x->left);
			if (x->right != nullptr) q.enqueue(x->right);
		}

		return ret;
	}

private:

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

template<typename Key, typename Value> std::allocator<typename NonrecursiveBST<Key, Value>::Node> NonrecursiveBST<Key, Value>::alloc;




/**
 *3.2.28缓存技术
 */
/*
#include<iostream>
int main()
{
	std::string a("EASYQUTION");
	RedBlackBST<char, int> st;
	for (int i = 0; i < a.size(); ++i)
		st.put(a[i], i);

	//std::cout << st.ceiling(6) << '\t' << st.get(3) << '\t' << st.rank(3) << std::endl;
	//st.rank(3);
	if (st.is23Tree() && st.isBalanced())
		std::cout << "2-3Tree & Balanced!" << std::endl;

	st.deleteKey('S');
	if (st.is23Tree() && st.isBalanced())
		std::cout << "2-3Tree & Balanced!" << std::endl;

	auto v = st.toArray();

	for (auto item : v)
		std::cout << item.key << '\t' << item.value << std::endl;

	system("pause");
}
*/