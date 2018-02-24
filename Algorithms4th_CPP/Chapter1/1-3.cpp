#include<iostream>
#include<string>
#include"bag.h"
#include"stack.h"
#include"queue.h"
#include"random.h"
using std::string;
/**
 *1.3.4 Parentheses
 *using helper function trim()
 */
#include<vector>
string trim(const string &s)
{
	std::cout << "左值\n";
	string ret = s;
	ret.erase(ret.find_last_not_of(" ") + 1);
	ret.erase(0, ret.find_first_not_of(" "));
	return ret;
}

string  trim(string &&s)
{
	std::cout << "右值\n";
	string ret(std::move(s));
	ret.erase(ret.find_last_not_of(" ") + 1);
	ret.erase(0, ret.find_first_not_of(" "));
	return ret;
}

bool parentheses(const string &s)
{
	auto cs = trim(s);
	Stack<char> stack;
	for (char c : cs)
	{
		switch (c)
		{
		case '[':
		case '(':
		case '{':
			stack.push(c);
			break;
		case ']':
			if (stack.pop() != '[')
				return false;
			break;
		case ')':
			if (stack.pop() != '(')
				return false;
			break;
		case '}':
			if (stack.pop() != '{')
				return false;
			break;
		default:
			throw std::invalid_argument("have invalid character");
			break;
		}
	}
	return true;
}

/**
 *测试用例
 */
/*
int main()
{
	string s("[()]{}{[()()]()}");
	bool ret = parentheses(s);
	s = "[(])";
	ret = parentheses(s);
}
*/

/**
 *1.3.5:如何打印数字N的二进制表示？
 */
void printBitsOfInt(int N)
{
	Stack<int> stack;
	while (N > 0)
	{
		stack.push(N % 2);
		N /= 2;
	}
	for (auto d : stack)
		std::cout << d;
}

/**
 *1.3.6:如何将队列反序
 *记住：（反序跟stack有关）
 */
template<typename Item>
void reverseQueue(Queue<Item> &q)
{
	Stack<Item> stack;
	while (!q.isEmpty())
		stack.push(q.dequeue());
	while (!stack.isEmpty())
		q.enqueue(stack.pop());
}

/**
 *1.3.9:主要依赖于空白，我们可以不用处理读取问题
 */
#include<sstream>
string addLeft(const string &s)
{
	Stack<string> vals;
	Stack<string> ops;

	std::istringstream in(s);
	string tmp;
	while (in >> tmp)
	{
		if (tmp == "+"||tmp=="-"||tmp=="*"||tmp=="/") 
			ops.push(tmp);
		else if (tmp == ")")
		{
			string rhv = vals.pop();
			string exp = "( " + vals.pop() + " " + ops.pop() + " " + rhv + " )";
			vals.push(std::move(exp));
		}
		else
			vals.push(tmp);
	}
	return vals.pop();
}

/**
 *1.3.10 中序表达式转化为后序表达式，
 *与1.3.9思路相似，但是我们不需要更改前面的string，所以可以一条路解析到底，不用回头看
 */
string infixToPostfix(const string &s)
{
	string ret;
	Stack<string> ops;

	std::istringstream in(s);
	string tmp;
	while (in >> tmp)
	{
		if (tmp == "+" || tmp == "-" || tmp == "*" || tmp == "/")
			ops.push(tmp);
		else if (tmp == ")")
		{
			ret = ret + ops.pop() + " ";
		}
		else if (tmp == "(")
			ret += " ";
		else
			ret=ret+tmp+" ";
	}
	return ret;
}


/**
 *1.3.11
 */
double evaluatepostPix(const string& s)
{
	Stack<double> vals;

	std::istringstream in(s);
	string tmp;
	while (in >> tmp)
	{
		if (tmp == "+")
			vals.push(vals.pop() + vals.pop());
		else if (tmp == "*")
			vals.push(vals.pop()*vals.pop());
		else if (tmp == "-")
			vals.push(-(vals.pop() - vals.pop()));
		else if (tmp == "/")
		{
			double rhv = vals.pop();
			vals.push(vals.pop() / rhv);
		}
		else
			vals.push(std::stod(tmp));
	}
	return vals.pop();
}
/**
*测试用例
*/
/*
int main()
{
	string s = "1 + 2 ) * 3 - 4 ) * 5 - 6 ) ) )";
	s = addLeft(s);
	std::cout << s << std::endl;
	s = infixToPostfix(s);
	std::cout << s << std::endl;
	std::cout << evaluatepostPix(s);
	system("pause");
}
*/


/**
 *  The {@code ResizingArrayQueue} class represents a first-in-first-out (FIFO)
 *  queue of generic items.
 *  It supports the usual <em>enqueue</em> and <em>dequeue</em>
 *  operations, along with methods for peeking at the first item,
 *  testing if the queue is empty, and iterating through
 *  the items in FIFO order.
 *  <p>
 *  This implementation uses a resizing array, which double the underlying array
 *  when it is full and halves the underlying array when it is one-quarter full.
 *  The <em>enqueue</em> and <em>dequeue</em> operations take constant amortized time.
 *  The <em>size</em>, <em>peek</em>, and <em>is-empty</em> operations takes
 *  constant time in the worst case.
 *  <p>
 *使用vector作为底层实现，由此可以借用合成的拷贝控制和移动控制函数
 *算了，还是自己实现以下吧......
 *随思：当覆盖一个元素之前，需不需要先销毁？pop_back都是使用destory函数（又牵扯到内置类型和其他类型）
 *但是这里我们不需要销毁，例如一个类来说，当我们覆盖的时候，会自动调用拷贝赋值函数（或移动赋值函数），其中必然包含了
 *我们想要的操作
 *vector中存指针，最好使用智能指针
 *使用wrap-around技巧
 */

#include<allocators>
template<typename Item> class __queueIterator;
template<typename Item>
class  ResizingArrayQueue
{
	friend void swap(ResizingArrayQueue &lhs, ResizingArrayQueue &rhs);
	friend class __queueIterator<Item>;
private:
	Item* q;//queue elements
	int qLength;
	int first;//index of first element
	int last;//index of off-last element
	int N;//the number of element
public:
	typedef __queueIterator<Item> Iterator;
	/**
	 *Initializes an empty queue
	 */
	ResizingArrayQueue()
		:first(0), last(0), N(0)
	{
		q = new Item[2];
		if (!q)
			throw std::runtime_error("Memory exhaustion");
		qLength = 2;
	}
	/**
	 *拷贝构造函数：类值
	 */
	ResizingArrayQueue(const ResizingArrayQueue &r)
	{
		first = 0;
		last = r.last - r.first;
		N = last - first;
		qLength = N;
		q = new Item[qLength];
		if (!q)
			throw std::runtime_error("Memory exhaustion");
		for (int i = 0; i < N; ++i)
			q[i] = r.q[(r.first + i) % (r.qLength)];
		
		return *this;
	}

	/**
	 *移动构造函数
	 */
	ResizingArrayQueue(ResizingArrayQueue &&r) noexcept
		:first(r.first),last(r.last),qLength(r.qLength),N(r.N),q(r.q)
	{
		r.q = nullptr;
		return *this;
	}

	/**
	 *拷贝赋值函数：类值,此处需要处理的情况挺繁琐的（参见拷贝构造函数），所以我们使用拷贝并交换技术
	 */
	ResizingArrayQueue& operator=(ResizingArrayQueue rhs)
	{
		swap(*this, rhs);
		return *this;
	}

	/**
	 *移动赋值函数
	 */
	ResizingArrayQueue& operator=(ResizingArrayQueue &&rhs) noexcept
	{
		if (this != &rhs)
		{
			delete[] q;
			q = rhs.q;
			first = rhs.first;
			last = rhs.last;
			N = rhs.N;
			qLength = rhs.qLength;
		}
		return *this;
	}

	/**
	 *析构函数
	 */
	~ResizingArrayQueue()
	{
		delete[] q;
	}

	bool isEmpty() const
	{
		return N == 0;
	}

	int size() const
	{
		return N;
	}
	
	void resize(int cap)
	{
		_ASSERT(cap >= N);
		auto newq = new Item[cap];
		if (!newq)
			throw std::runtime_error("Memory exhaustion");
		for (int i = 0; i < N; ++i)
			newq[i] = q[(first + i) % qLength];
		q = newq;
		first = 0;
		last = N;
		qLength = cap;
	}

	void enqueue(const Item &item)
	{
		//为什么要差1呢？如果不差1个的话，first就会跟last重叠。
		if (N == qLength-1)
			resize(2 * N);
		q[last++] = item;
		if (last == qLength)
			last = 0;
		++N;
	}

	void enqueue(Item &&item)
	{
		if (N == qLength-1)
			resize(2 * qLength);
		q[last++] = std::move(item);
		if (last == qLength)
			last = 0;
		++N;
	}

	Item dequeue()
	{
		if(isEmpty())
			throw std::underflow_error("Queue underflow");
		Item ret = q[first];
		++first;
		--N;
		if (first == qLength) first = 0;
		if (N > 0 && N == qLength / 4) resize(qLength / 2);
		return ret;
	}

	Item peek() const
	{
		if (isEmpty())
			throw std::underflow_error("Queue underflow");
		return q[first];
	}

	Iterator begin()
	{
		return Iterator(q, qLength, first);
	}
	Iterator end()
	{
		return Iterator(q, qLength, last);
	}


};

template<typename Item>
inline void swap(ResizingArrayQueue<Item> &lhs, ResizingArrayQueue<Item> &rhs)
{
	using std::swap;
	swap(lhs.q, rhs.q);
	swap(lhs.first, rhs.first);
	swap(lhs.last, rhs.last);
	swap(lhs.N, rhs.N);
	swap(lhs.qLength, rhs.qLength);
}

template<typename Item>
class __queueIterator
{
	typedef __queueIterator<Item> self;
private:
	Item* q;
	int index;
	int qLength;
public:
	__queueIterator(Item* _q, int ql, int i)
		:q(_q), qLength(ql), index(i) {}
	bool operator==(const self &rhs) const
	{
		return (q == rhs.q) && (index == rhs.index);
	}
	bool operator!=(const self &rhs) const
	{
		return !(*this == rhs);
	}
	Item& operator*() const { return q[index]; }
	Item* operator->() const { return &(q[index]); }
	self& operator++()
	{
		if ((++index) == qLength) index = 0;
		return *this;
	}
	self& operator++(int)
	{
		self ret = *this;
		++*this;
		return ret;
	}	
};

template<typename Item>
class Vector
{
public:
	/**
	 *默认构造函数
	 */
	Vector(){}
	/**
	 *拷贝控制
	 */
	Vector(const Vector &s)
	{
		auto newdata = alloc_n_copy(s.begin(), s.end());
		elements = newdata.first;
		first_free = cap = newdata.second;
	}

	Vector &operator=(const Vector &rhs)
	{
		auto newdata = alloc_n_copy(rhs.begin(), rhs.end());
		free();
		elements = newdata.first;
		first_free = cap = newdata.second;
		return *this;
	}

	Vector(Vector &&s) noexcept
		:elements(s.elements),first_free(s.first_free),cap(s.cap)
	{
		s.elements = s.first_free = s.cap = nullptr;
	}

	Vector &operator=(Vector &&rhs) noexcept
	{
		if (this != rhs)
			free();
		
		//take over resources from rhs
		elements = rhs.elements;
		first_free = rhs.first_free;
		cap = rhs.cap;

		//leave rhs in a destructible state
		rhs.elements = rhs.first_free = rhs.cap = nullptr;

		return *this;
	}

	~Vector() noexcept 
	{ 
		free(); 
	}

	/**
	 *list assignment
	 */
	Vector &operator=(std::initializer_list<Item> il)
	{
		auto newdata = alloc_n_copy(il.begin(), il.end());
		free();
		elements = newdata.first;
		first_free = cap = data.second;
	}

	/**
	 *add item to vector
	 */
	void push_back(const Item &s)
	{
		chk_n_alloc();
		alloc.construct(first_free++, s);
	}

	void push_back(Item &&s)
	{
		chk_n_alloc();
		alloc.construct(first_free++, std::move(s));
	}

	template<class...Args> void emplace_back(Args&&... args)
	{
		chk_n_alloc();
		alloc.construct(first_free++, std::forward<Args>(args)...);
	}

	/**
	 *size and capacity
	 */
	size_t size() const { return first_free - elements; }
	size_t capacity() const { return cap - elements; }

	/**
	 *item access
	 */
	Item &operator[](size_t n) { return elements[n]; }
	const Item &operator[](size_t n) const { return elements[n]; }

	/**
	 *iterator interface
	 */
	Item* begin() const { return elements; }
	Item* end() const { return first_free; }

private:
	static std::allocator<Item> alloc;
	
	/**
	 *check whether have memory,
	 *if not, reallocate 
	 */
	void chk_n_alloc()
	{
		if (first_free == cap)
			reallocate();
	}

	std::pair<Item*, Item*> alloc_n_copy(const Item *b, const Item *e)
	{
		auto data = alloc.allocate(e - b);
		return{ data, std::uninitialized_copy(b,e,data) };
	}
	
	/**
	 *销毁元素，释放内存
	 */
	void free()
	{
		//destory the older items in reverse order
		for (auto p = first_free; p != elements;/*empty*/)
			alloc.destroy(--p);

		//deallocate memory
		if (elements)
			alloc.deallocate(elements, cap - elements);
	}

	void reallocate()
	{
		auto newcapacity = size() ? 2 * size() : 2;

		//allocate new space
		auto first = alloc.allocate(newcapacity);
		auto dest = first;
		auto elem = elements;

		//move the elements
		for (size_t i = 0; i != size(); ++i)
			alloc.construct(dest++, std::move(*elem++));
		free();

		elements = first;
		first_free = dest;
		cap = elements + newcapacity;
	}


	Item *elements = nullptr;//指向数组首元素的地址
	Item* first_free = nullptr;//指向数组第一个空闲元素
	Item* cap = nullptr;//指向数组尾后位置的指针
};
//definition for the static data member
template<typename Item> std::allocator<Item> Vector<Item>::alloc;


/**
 *1.3.28
 */
template<typename Item>
struct __Node
{
	__Node *next;
	Item item;
};
//declaration
template<typename Item> class __SingleListIterator;

template<typename Item>
class SingleList
{
	typedef __Node<Item> Node;
	typedef __SingleListIterator<Item> Iterator;
public:
	SingleList()
		:N(0)
	{
		beforeFirst = alloc.allocate(1);
		beforeFirst->next = beforeFirst;
	}

	bool isEmpty() const { return N == 0; }
	int size() const { return N; }
	/**
	 *1.3.19 remove last node
	 */
	void pop_back()
	{
		erase(N - 1);
	}

	/**
	 * 1.3.20 remove the kth elements, k starting from 0
	 */
	void erase(int k)
	{
		_ASSERT(k < N);
		auto cur = beforeFirst;
		auto prev = cur;
		for (int i = 0; i <= k; ++i)
		{
			prev = cur;
			cur = cur->next;
		}

		prev->next = cur->next;		
		deleteNode(cur);
		--N;
	}
	
	/**
	 * 1.3.21 find key
	 */
	bool find(const Item &s)
	{
		for (auto c : *this)
			if (c == s) return true;
		
		return false;
	}

	/**
	 *1.3.24
	 */
	Iterator removeAfter(Iterator it)
	{	
		auto it_copy = it;
		if (it_copy == end() || ++it_copy == end())
			return it;

		(it.node)->next = (it_copy.node)->next;
		deleteNode(it_copy.node);
		--N;
		return ++it;
	}

	/**
	 *1.3.25
	 */
	Iterator insertAfter(Iterator it, const Item &item)
	{
		auto newNode = createNode(item);
		newNode->next = (it.node)->next;
		(it.node)->next = newNode;
		++N;
		return Iterator(newNode);
	}
	
	/**
	 *1.3.40:工具函数，插入到表头
	 */
	Iterator push_front(const Item &s)
	{
		return insertAfter(beforeBegin(), item);
	}

	/**
	 *1.3.26
	 */
	int remove(const Item &key)
	{
		int count = 0;
		auto prev = beforeFirst;
		auto cur = prev;
		for (int i = 0; i < N; ++i)
		{
			prev = cur;
			cur = cur->next;
			if (cur->item == key)
			{
				prev->next = cur->next;
				deleteNode(cur);
				++count;
				--N;
				cur = prev;//保持了与for循环的同步
			}
		}
		return count;
	}
	
	/**
	 *1.3.27
	 */
	Item max() const
	{
		if (isEmpty())
			return Item;
		auto ret = *begin();
		for (auto item : *this)
			if (ret < item) ret = item;

		return ret;
	}

	/**
	 *1.3.30:reverse the list
	 *迭代实现：保存prev，cur，next三个节点
	 */

	Iterator reverse(Iterator beg)
	{
		if (beg.node == beforeFirst) return end();
		if ((beg.node)->next==beforeFirst) return beg;

		Iterator second((beg.node)->next);
		auto rest = reverse(second);
		(second.node)->next = (beg.node);
		(beg.node)->next = beforeFirst;
		beforeFirst->next = rest.node;
		return rest;
	}


	/**
	 *iterator the list
	 */
	Iterator beforeBegin() const { return beforeFirst; }
	Iterator begin() const { return beforeFirst->next; }
	Iterator end() const { return beforeFirst; }
private:
	Node* beforeFirst;//equals to off last//保存一个指针，实现环装链表
	size_t N; //the numbers of list

private:

private:
	static std::allocator<Node> alloc;
	/**
	 *不同于数组内存分配：只有reallocator和析构函数使用alloc的分配、构造和销毁
	 *这里经常使用这些操作，所以有必要定义工具函数
	 */
	Node* createNode(const Item &s)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&(p->item), s);//注意一定是取地址
		return p;
	}
	Node* createNode(Item &&s)
	{
		auto p = alloc.allocate(1);
		alloc.construct(&(p->item), std::move(s));
		return p;
	}
	void deleteNode(Node* node)
	{
		alloc.destroy(&(node->item));
		alloc.deallocate(node,1);
	}
	
};

template<typename Item> std::allocator<__Node<Item>> SingleList<Item>::alloc;

template<typename Item>
class __SingleListIterator
{
	typedef __SingleListIterator<Item> self;
public:
	__Node<Item>* node;
public:
	__SingleListIterator(__Node<Item>* s)
		:node(s) {}
	bool operator==(const self &rhs) const
	{
		return node==rhs.node;
	}
	bool operator!=(const self &rhs) const
	{
		return !(*this == rhs);
	}
	Item& operator*() const { return node->item; }
	Item* operator->() const { return &(node->item); }
	self& operator++()
	{
		node = node->next;
		return *this;
	}
	self& operator++(int)
	{
		self ret = *this;
		++*this;
		return ret;
	}
};




/**
 *1.3.32 Steque:只需要在queue类中添加stack类的push方法即可
 */

/**
 *1.3.34 随机背包：既然是随机，必然需要随机访问，所以必须使用数组
 */
#include<vector>
#include<random>

template<typename Item> class __RandomBagIterator;

template<typename Item>
class RandomBag
{
	friend class __RandomBagIterator<Item>;
private:
	std::vector<Item> items;
	std::vector<int> r;

public:
	typedef  __RandomBagIterator<Item> Iterator;
	RandomBag() = default;
	
	bool isEmpty() const { return items.empty(); }
	int size() const { return items.size(); }

	void add(const Item &s) { items.push_back(s); }
	void add(Item &&s) { items.push_back(std::forward<Item>(s)); }

	//随机迭代访问的要求是：
	Iterator begin() { return Iterator(this, 0); }
	Iterator end() { return Iterator(this, size()); }

    //产生一个顺序
	void shuffle()
	{
		r.resize(items.size());
		for (int i = 0; i < r.size(); ++i)
			r[i] = i;

		//产生随机数
		static Random random;
		random.shuffle(r.begin(), r.end());
	}


};

template<typename Item>
class __RandomBagIterator
{
	typedef __RandomBagIterator<Item> self;
private:
	RandomBag<Item>* bag;
	int n;
public:
	__RandomBagIterator(RandomBag<Item>* b,int i)
		:bag(b),n(i) {}

	bool operator==(const self &rhs) const
	{
		return bag==rhs.bag && n==rhs.n;
	}
	bool operator!=(const self &rhs) const
	{
		return !(*this == rhs);
	}
	Item& operator*() const { return (bag->items).at((bag->r)[n]); }
	Item* operator->() const { return &(*this); }
	self& operator++()
	{
		++n;
		return *this;
	}
	self& operator++(int)
	{
		self ret = *this;
		++*this;
		return ret;
	}
	self& operator--()
	{
		--n;
		return *this;
	}
	self& operator--(int)
	{
		self ret = *this;
		++*this;
		return ret;
	}
};

/**
 *1.3.37
 */
void josephus(int N, int M)
{
	Queue<int> q;
	for (int i = 0; i < N; ++i)
		q.enqueue(i);

	while (!q.isEmpty())
	{
		for (int i = 1; i < M; ++i)
			q.enqueue(q.dequeue());

		//此时是第M个人
		std::cout << q.dequeue()<<' ';
	}
}

/**
 *1.3.39:前移编码策略：如何链表中没有此元素，则添加到表头；如果有此元素，删除此元素并再次插入到表头
 *这个策略的假设是：最近访问过的元素很可能会再次访问,用于缓存和数据压缩
 *使用之前1.3.19-1.3.30定义的SingleList
 */
template<typename Item>
void moveToFront(SingleList<Item>& buf, const Item &s)
{
	buf.remove(s);
	buf.push_front(s);
}

/**
 *1.3.44文本编辑缓冲区：使用两个栈
 */
class Buffer
{
private:
	Stack<char> lstack;
	Stack<char> rstack;

public:
	Buffer() = default;

	/**
	 *在光标位置插入字符
	 */
	void insert(char c) { lstack.push(c); }

	/**
	 *在光标位置删除字符
	 */
	char remove() { return lstack.pop(); }

	/**
	 *光标左/右移k个位置
	 */
	void left(int k)
	{
		_ASSERT(k <= lstack.size());
		for (int i = 0; i < k; ++i)
			rstack.push(lstack.pop());
	}

	void right(int k)
	{
		_ASSERT(k <= rstack.size());
		for (int i = 0; i < k; ++i)
			lstack.push(rstack.pop());
	}

	/**
	 *缓冲区中的字符数量
	 */
	int size() const { return lstack.size() + rstack.size(); }
};

/**
 *1.3.49：两个栈实现一个队列
 *负负得正->两个栈可以实现一个队列；但是正正不能得负，所以两个队列不能实现一个栈
 */
template<typename Item>
class TwoStackQueue
{
private:
	Stack<Item> left;//only for add item
	Stack<Item> right;//only for delete item

public:
	TwoStackQueue() = default;

	void enqueue(const Item &s) { left.push(s); }

	/**
	 *只有等right为空了之后，再把元素填充到right中，才能保证逻辑正确
	 */
	Item dequeue()
	{
		if (!right.isEmpty())
			return right.pop();

		while (!left.isEmpty())
			right.push(left.pop());

		return right.pop();
	}
};

/**
 *1.3.50:如何实现可以捕捉数据集合变化的迭代器？
 *用一个计数器记录push和pop操作的次数，检查计数器是否发生变化。
 */

