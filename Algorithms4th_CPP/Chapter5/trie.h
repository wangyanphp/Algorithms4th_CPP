#pragma once
#include"string.h"
#include<allocators>

/**
 *其有序性操作与之前的二叉树的不同在于：需要考虑x是否命中
 */
template<typename Value>
class TrieST
{
private:	
	
	struct Node
	{
		Value value;
		std::vector<Node*> next;
		int size;
		Node(Value val, int n, int r) :value(val), size(n), next(r, nullptr) {}
	};

private:
	Node *root = nullptr;
	Alphabet alphabet;
	int R;
	Value NULLVALUE;
//--------------------------------------------新的函数------------------------------------------------------------
public:
	std::vector<std::string> keys()
	{
		return keysWithPrefix("");
	}

	std::vector<std::string> keysWithPrefix(const std::string &pre)
	{
		std::vector<std::string> ret;
		collect(get(root, pre, 0), pre, ret);
		return ret;
	}

	std::vector<std::string> keysThatMatch(const std::string &pat)
	{
		std::vector<std::string> ret;
		collect(root, "", pat, v);
		return ret;
	}	

	std::string longestPrefixOf(const std::string &s)
	{
		int length = search(root, s, 0, 0);
		return s.substr(0, length);
	}

private:

	void collect(Node *x, const std::string &pre, std::vector<std::string> &v)
	{//收集以pre为前缀的所有key
		if (x == nullptr) return;
		if (x->value != NULLVALUE) v.push_back(pre);
		for (int c = 0; c < R; ++c)
			collect(x->next[c], pre + alphabet.toChar(c), v);
	}

	void collect(Node *x, const std::string &pre, std::string &pat, std::vector<std::string> &v)
	{//收集以pre为前缀，所有匹配pat的key
		int d = pre.size();
		if (x == nullptr) return;
		if (d == pat.size() && x->value != nullptr) v.push_back(pre);
		if (d == pat.size()) return;

		char next = pat[next];
		for (int c = 0; c < R; ++c)
			if (next == '.' || next == c)
				collect(x->next[c], pre + alphabet.toChar(c), pat, v);
	}

	int search(Node *x, const std::string &s, int d, int length)
	{
		if (x == nullptr) return length;
		if (x->value != NULLVALUE) length = d;
		if (d == s.length()) return length;
		int c = charAt(s, d);
		return search(x->next[c], s, d + 1, length);
	}


//--------------------------------------------------------------------------------------------------------------//
public:

	TrieST(Alphabet a, Value nullval = Value()) :alphabet(a), NULLVALUE(nullval)
	{
		R = alphabet.R();
	}
	//拷贝构造，按值拷贝
	TrieST(const TrieST& rhs)
	{
		root = constructor(rhs.root);
		NULLVALUE = rhs.NULLVALUE;
		alphabet = rhs.alphabet;
		R = rhs.R;

	}

	TrieST& operator=(const TrieST& rhs)
	{
		TrieST tmp(rhs);
		destructor(root);
		root = tmp.root;

		return *this;
	}

	TrieST(TrieST &&rhs) noexcept
	{
		root = rhs.root;
		NULLVALUE = rhs.NULLVALUE;
		alphabet = std::move(rhs.alphabet);
		rhs.root = nullptr;
	}

	TrieST& operator=(TrieST&& rhs) noexcept
	{
		if (this != &rhs)
		{
			destructor(root);
			root = rhs.root;
			rhs.root = nullptr;
		}
		return *this;
	}

	~TrieST()
	{
		destructor(root);
	}

private:
	void destructor(Node *x)
	{
		if (x == nullptr) return;
		for (int c = 0; c < R; ++c)
			destructor(x->next[c]);
		deleteNode(x);
	}

	Node* constructor(Node* x)
	{
		if (x == nullptr) return nullptr;

		auto ret = createNode(x->value, x->size);
		for (int c = 0; c < R; ++c)
			ret->next[c] = constructor(x->next[c]);
		return ret;
	}

//-----------------------------------------------------------------------------------------------------------//
public:
	Value get(const std::string &key)
	{
		Node *x = get(root, key, 0);
		if (x == nullptr) return NULLVALUE;
		return x->value;
	}

	void put(const std::string &key, Value val)
	{
		root = put(root, key, val, 0);
	}

	bool contains(const std::string &key)
	{
		return get(key) != NULLVALUE;
	}

	int size() { return size(root); }

	bool isEmpty() { return size() == 0; }

	void deleteKey(const std::string &key)
	{
		root = deleteKey(root, key, 0);
	}

	std::string minKey()
	{
		std::string ret;
		min(root, "", ret);
		return ret;
	}

	std::string maxKey()
	{
		std::string ret;
		max(root, "", ret);
		return ret;
	}

	std::string select(int k)
	{
		std::string ret;
		select(root, k, "", ret);
		return ret;
	}

	int rank(const std::string &key)
	{
		return rank(root, "", key, 0);
	}

	std::string floor(const std::string &key)
	{//参考最长前缀
		std::string ret;
		floor(root, "", key, ret);
		return ret;
	}

private:

	int size(Node *x) 
	{
		if (x == nullptr) return 0;
		return x->size; 
	}

	void min(Node *x, const std::string &pre, std::string &ret)
	{
		if (x == nullptr) return;
		//if(x->size==0) return;
		if (size(x) == 1 && x->value != NULLVALUE)
		{//就是x
			ret = pre; return;
		}

		int c = 0;
		for (; c < R; ++c)
		{
			if (size(x->next[c]) != 0)
				break;
		}
		min(x->next[c], pre + alphabet.toChar(c), ret);
	}

	void max(Node *x, const std::string &pre, std::string &ret)
	{
		if (x == nullptr) return;
		//if(x->size==0) return;
		if (size(x) == 1 && x->value != NULLVALUE)
		{//就是x
			ret = pre; return;
		}

		int c = R-1;
		for (; c >= 0; --c)
		{
			if (size(x->next[c]) != 0)
				break;
		}
		min(x->next[c], pre + alphabet.toChar(c), ret);
	}

	void select(Node *x, int k, const std::string &pre, std::string &ret)
	{
		if (x == nullptr) return;
		if (k == 0 && x->value != NULLVALUE)
		{
			ret = pre;   return;
		}

		int t = 0, t2 = 0;
		if (x->value != NULLVALUE) t2 = 1;

		for (int c = 0; c < R; ++c)
		{
			t += size(x->next[c]);
			if (t > k) return select(x->next[c], k - t -t2 + size(x->next[c]), pre + alphabet.toChar(c), ret);
		}
	}

	int rank(Node *x, const std::string &pre, const std::string &key, int rank_)
	{//pre必然包含了x的char
		int d = pre.size();
		if (x == nullptr) return rank_;
		if (d == key.size() && x->value != NULLVALUE) return rank_;
		if (d == key.size()) return rank_;

		int next = charAt(key, d);
		if (x->value != NULLVALUE)
			rank_ += 1;
		for (int c = 0; c < next; ++c)
			rank_ += size(x->next[c]);
		
		return rank(x->next[next], pre + key[d], key, rank_);
	}

	void floor(Node *x, const std::string &pre, const std::string &key, std::string &ret)
	{//有点错误
		if (x == nullptr) return;
		int d = pre.size();
		if (d == key.size() && x->value != NULLVALUE)
		{
			ret = key; return;
		}
		if (d == key.size()) { return; }

		if (x->value != NULLVALUE) ret = pre;
		
		int c = charAt(key, d);

		if (size(x->next[c]) != 0)
			floor(x->next[c], pre + alphabet.toChar(c), key, ret);
		else
		{//返回x->next左侧的最大值,or 前缀 or 
			int i = c-1;
			for (; i >= 0; --i)
				if (size(x->next[i]) != 0) break;
			if (i >= 0)
			{
				std::string max_;
				max(x->next[i], pre + alphabet.toChar(i), max_);
				ret = max_;
			}
			return;
		}
	}


	int charAt(const std::string &s, int d)
	{
		return alphabet.toIndex(s[d]);
	}

	Node* get(Node *x, const std::string &key, int d)
	{//返回的node，可能有值也可能没有值
		if (x == nullptr) return nullptr;
		if (d == key.size()) return x;
		int c = charAt(key, d);
		return get(x->next[c], key, d + 1);
	}

	Node* put(Node *x, const std::string &key, const Value &val, int d)
	{
		if (x == nullptr) x = createNode(NULLVALUE,0);
		if (d == key.size())
		{
			if (x->value == NULLVALUE) ++x->size;
			x->value = val;
			return x;
		}
		int c = charAt(key, d);
		x->next[c] = put(x->next[c], key, val, d + 1);

		if (x->value == NULLVALUE) x->size = 0;
		else x->size = 1;

		for (int c = 0; c < R; ++c)
			x->size += size(x->next[c]);
		
		return x;
	}

	Node* deleteKey(Node *x, const std::string &key, int d)
	{
		if (x == nullptr) return nullptr;
		if (d == key.size())
		{
			if (x->value != NULLVALUE) --x->size;
			x->value = NULLVALUE;
		}
		else
		{
			int c = charAt(key, d);
			x->next[c] = deleteKey(x->next[c], key, d + 1);
		}

		if (x->value == NULLVALUE) x->size = 0;
		else x->size = 1;
		for (int c = 0; c < R; ++c)
			x->size += size(x->next[c]);

		if (x->size == 0)
		{
			destructor(x);
			return nullptr;
		}
		else return x;
		/*
		if (x->value != nullptr) return x;

		for (int c = 0; c < R; ++c)
			if (x->next[c] != nullptr) return x;
		return nullptr;*/
	}


//-----------------------------------------内存分配相关--------------------------------------------------------------
private:
	static std::allocator<Node> alloc;
	Node* createNode(const Value &val, const int n)
	{
		auto p = alloc.allocate(1);
		alloc.construct(p, val, n, R);
		return p;
	}
	Node* createNode(Value &&val, const int n)
	{
		auto p = alloc.allocate(1);
		alloc.construct(p, std::forward<Value>(val), n);
		return p;
	}
	void deleteNode(Node* node)
	{
		alloc.destroy(node);
		alloc.deallocate(node, 1);
	}
};
template<typename Value> std::allocator<typename TrieST<Value>::Node> TrieST<Value>::alloc;


template<typename Value>
class TST
{

private:
	struct Node
	{
		char c;
		Node *left =nullptr, *mid = nullptr, *right = nullptr;
		Value value;
		int size;

		Node(char c_, const Value val, int n) :c(c_), value(val), size(n) {}
	};

private:
	Node *root = nullptr;
	Value NULLVALUE;

public:
	std::string longestPrefixOf(const std::string &s)
	{
		if (s.size() == 0) return "";
		int length = 0;
		Node *x = root;
		int d = 0;
		while (x != nullptr&&d < s.size())
		{
			char c = s[c];
			if (c < x->c) x = x->left;
			else if (c>x->c) x = x->right;
			else
			{
				++d;
				if (x->value != NULLVALUE) length = d;
				x = x->mid;
			}
		}
		return s.substr(0, length);
	}

	std::vector<std::string> keys()
	{
		std::vector<std::string> ret;
		collect(root, "", ret);
		return ret;
	}

	std::vector<std::string> keysWithPrefix(const std::string &pre)
	{
		std::vector<std::string> ret;

		Node *x = get(root, pre, 0);
		if (x == nullptr) return ret;
		if (x->value != NULLVALUE) ret.push_back(pre);
		collect(x, pre, ret);
		return ret;
	}

	std::vector<std::string> keysThatMatch(const std::string &pat)
	{
		std::vector<std::string> ret;
		collect(root, "", 0, pat, ret);
		return ret;
	}

private:

	void collect(Node *x, const std::string &pre, std::vector<std::string> &ret)
	{
		if (x == nullptr) return;
		collect(x->left, pre, ret);
		if (x->value != NULLVALUE) ret.push_back(pre+x->c);
		collect(x->mid, pre + x->c, ret);
		collect(x->right, pre, ret);
	}

	void collect(Node *x, const std::string &pre, int d, const std::string &pat, std::vector<std::string> &ret)
	{
		if (x == nullptr) return;
		char c = pat[d];
		if (c == '.' || c < x->c) collect(x->left, pre, d, pat, ret);
		if (c == '.' || c == x->c)
		{
			if (d == pat.length() - 1 && x->value != NULLVALUE) ret.push_back(pre + x->c);
			if (d < pat.length() - 1)
				collect(x->mid, pre + x->c, d + 1, pat, ret);
		}
		if (c == '.' || c>x->c) collect(x->right, pre, d, pat, ret);
	}

public:
	bool contains(const std::string &key)
	{
		return get(key) != NULLVALUE;
	}

	Value get(const std::string &key)
	{
		Node *x = get(root, key, 0);
		if (x == nullptr) return NULLVALUE;
		return x->value;
	}

	void put(const std::string &key, const Value &val)
	{
		root = put(root, key, val, 0);
	}

	int size() { return size(root); }

private:

	int size(Node *x)
	{
		if (x == nullptr) return 0;
		else return x->size;
	}

	Node* get(Node *x, const std::string &key, int d)
	{
		if (x == nullptr) return nullptr;
		char c = key[d];
		if (c < x->c) return get(x->left, key, d);
		else if (x->c < c) return get(x->right, key, d);
		else if (d < key.size() - 1) return get(x->mid, key, d + 1);
		else return x;
	}

	Node* put(Node *x, const std::string &key, const Value &val, int d)
	{
		char c = key[d];
		if (x == nullptr) { x = createNode(c, NULLVALUE, 0); }
		if (c < x->c) x->left = put(x->left, key, val, d);
		else if (x->c < c) x->right = put(x->right, key, val, d);
		else if (d < key.size() - 1) x->mid = put(x->mid, key, val, d + 1);
		else
		{
			if (x->value == NULLVALUE) ++x->size;
			x->value = val;
		}

		x->size = size(x->left) + size(x->mid) + size(x->right);
		if (x->value != NULLVALUE) x->size += 1;

		return x;
	}
//-----------------------------------------------------------------------------------------------------------------
public:
	TST( Value nullval = Value()) :NULLVALUE(nullval)
	{
	}
	//拷贝构造，按值拷贝
	TST(const TST& rhs)
	{
		root = constructor(rhs.root);
		NULLVALUE = rhs.NULLVALUE;
	}

	TST& operator=(const TST& rhs)
	{
		TST tmp(rhs);
		destructor(root);
		root = tmp.root;

		return *this;
	}

	TST(TST &&rhs) noexcept
	{
		root = rhs.root;
		NULLVALUE = rhs.NULLVALUE;
		rhs.root = nullptr;
	}

	TST& operator=(TST&& rhs) noexcept
	{
		if (this != &rhs)
		{
			destructor(root);
			root = rhs.root;
			rhs.root = nullptr;
		}
		return *this;
	}

	~TST()
	{
		destructor(root);
	}

private:
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
		auto ret = createNode(x->c,x->value, x->size);
		ret->left = left;
		ret->right = right;
		return ret;
	}

//---------------------------------------------------------------------------------------------------------------//
//-----------------------------------------内存分配相关--------------------------------------------------------------
private:
	static std::allocator<Node> alloc;
	Node* createNode(char c, const Value &val, const int n)
	{
		auto p = alloc.allocate(1);
		alloc.construct(p, c, val, n);
		return p;
	}
	Node* createNode(char c, Value &&val, const int n)
	{
		auto p = alloc.allocate(1);
		alloc.construct(p, std::forward<Value>(val), n);
		return p;
	}
	void deleteNode(Node* node)
	{
		alloc.destroy(node);
		alloc.deallocate(node, 1);
	}

};

template<typename Value> std::allocator<typename TST<Value>::Node> TST<Value>::alloc;
