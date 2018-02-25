#include<iostream>
#include<string>
#include"bag.h"
#include"stack.h"
#include"queue.h"
#include"random.h"
#include"singlelist.h"
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
 *测试
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
 *1.3.11 输入后序表达式，求值
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
*测试
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
 *  1.3.14
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




/**
 *1.3.18-1.3.30 实现单链表，具体参见singlelist.h
 */


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
 *1.3.40:前移编码策略：如何链表中没有此元素，则添加到表头；如果有此元素，删除此元素并再次插入到表头
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


