/**
* 实现单链表需要考虑几个问题：
* 1. 如何进行内存的动态分配？可以使用默认的空间配置器allocate
* 2. 如何实现迭代器？可以使用节点的指针作为迭代器。那么end是什么呢？所以我们需要在List类中保存一个end的标志
* 这里使用dummy节点，也就是指向链表开始节点的节点（before first node）
* dummy-->first node-->....-->last node--|
* ↑--------------------------------------|
*/
#include<memory>

template<typename Item>
class SingleList
{
	class Node;//声明类
	class Iterator;

private://数据成员
	Node* dummy;//equals to off last，
	size_t N; //the numbers of list

public: //基本功能
	SingleList() :N(0)
	{
		dummy = alloc.allocate(1);
		dummy->next = dummy;
	}
	bool isEmpty() const { return N == 0; }
	int size() const { return N; }

public://添加元素，删除元素
	void popBack();                                            //删除最后一个元素，习题1.3.19
	void erase(int k);                                         //删除第k个元素，习题1.3.20
	Iterator removeAfter(Iterator it);                         //删除it后面的节点，习题1.3.24
	int remove(const Item &key);                               //删除key，习题1.3.26


	Iterator insertAfter(Iterator it, const Item &item);       //在it后插入元素，习题1.3.25
	Iterator pushFront(const Item &s);                         //在链表最前面插入元素

	bool find(const Item &key);                                //find key，习题1.3.21
	Iterator max() const;                                          //习题1.3.27
	Iterator reverse(Iterator beg);                            //翻转链表，习题1.3.30

public: //迭代功能
	Iterator beforeBegin() const { return dummy; }
	Iterator begin() const { return dummy->next; }
	Iterator end() const { return dummy; }

public: //拷贝控制
	SingleList(const SingleList &rhs);
	SingleList(SingleList &&rhs) noexcept;
	SingleList& operator=(const SingleList &rhs);
	SingleList& operator=(SingleList &&rhs) noexcept;
	~SingleList();
private:
	void free();//拷贝控制的辅助函数——释放链表所有节点


private: //内部类
	struct Node
	{
		Node *next;
		Item item;
	};
	struct Iterator
	{
		Node* node;
		Iterator(Node* s) :node(s) {}
		bool operator==(const Iterator &rhs) const{ return node == rhs.node; }
		bool operator!=(const Iterator &rhs) const{ return !(*this == rhs); }
		Item& operator*() const { return node->item; }
		Item* operator->() const { return &(node->item); }
		Iterator& operator++(){ node = node->next; return *this; }
		Iterator& operator++(int){ Iterator ret = *this; ++*this; return ret; }
	};


private: //动态内存分配
	static std::allocator<Node> alloc;
	//create Node分处理左值和右值
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
		alloc.deallocate(node, 1);
	}
};

template<typename Item> std::allocator<typename SingleList<Item>::Node> SingleList<Item>::alloc;



//拷贝控制函数
template<typename Item>
SingleList<Item>::SingleList(const SingleList &rhs) 
	:SingleList()
{
	for (auto item : rhs) 
		pushFront(item);

	reverse(begin());
	N = rhs.N;

}

template<typename Item>
SingleList<Item>::SingleList(SingleList &&rhs) noexcept
	:dummy(rhs.dummy), N(rhs.N)
{
	rhs.dummy = nullptr;
	rhs.N = 0;
}

template<typename Item> 
SingleList<Item>& SingleList<Item>::operator=(const SingleList &rhs)
{
	if (*this != rhs)
	{
		free();
		for (auto item : rhs)
			pushFront(item);

		reverse(begin());
		N = rhs.N;
	}
}
template<typename Item>
SingleList<Item>& SingleList<Item>::operator=(SingleList &&rhs) noexcept
{
	if (*this != rhs)
	{
		free();
		dummy = rhs.dummy;
		N = rhs.N;
		rhs.dummy = nullptr;
		rhs.N = 0;
	}
}

template<typename Item>
SingleList<Item>::~SingleList()
{
	free();
}

template<typename Item>
void SingleList<Item>::free()
{
	while (!isEmpty())
	{
		erase(1);
	}
	N = 0;
	dummy = nullptr;
}
//-------拷贝控制函数结束-----------------------------------------//

template<typename Item>
void  SingleList<Item>::popBack()
{
	erase(N);
}

template<typename Item>
void  SingleList<Item>::erase(int k)
{
	_ASSERT(k <= N);
	auto cur = dummy;
	auto prev = cur;
	for (int i = 0; i < k; ++i)
	{
		prev = cur;
		cur = cur->next;
	}

	prev->next = cur->next;
	deleteNode(cur);
	--N;
}

template<typename Item>
bool SingleList<Item>::find(const Item &s)
{
	for (auto c : *this)
		if (c == s) return true;
	return false;
}

template<typename Item> typename SingleList<Item>::Iterator
SingleList<Item>::removeAfter(Iterator it)
{
	auto it_copy = it;
	if (it_copy == end() || ++it_copy == end())
		return it;

	(it.node)->next = (it_copy.node)->next;
	deleteNode(it_copy.node);
	--N;
	return ++it;
}

template<typename Item> typename SingleList<Item>::Iterator
SingleList<Item>::insertAfter(Iterator it, const Item &item)
{
	auto newNode = createNode(item);
	newNode->next = (it.node)->next;
	(it.node)->next = newNode;
	++N;
	return Iterator(newNode);
}

template<typename Item> typename SingleList<Item>::Iterator
SingleList<Item>::pushFront(const Item &s)
{
	return insertAfter(beforeBegin(), s);
}

template<typename Item>
int SingleList<Item>::remove(const Item &key)
{
	int count = 0;
	auto prev = dummy;
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

template<typename Item> typename SingleList<Item>::Iterator
SingleList<Item>::max() const
{
	if (isEmpty()) return end();
	auto ret = begin();
	for (auto it = begin(); it != end(); ++it)
		if (*ret < *it) ret = it;
	return ret;
}


//迭代实现的话需要保存prev，cur，next三个节点，下面采用递归实现
template<typename Item> typename SingleList<Item>::Iterator
SingleList<Item>::reverse(Iterator beg)
{
	//auto beg = begin();
	if (beg.node == dummy) return end();
	if ((beg.node)->next == dummy) return beg;

	Iterator second((beg.node)->next);
	auto rest = reverse(second);
	(second.node)->next = (beg.node);
	(beg.node)->next = dummy;
	dummy->next = rest.node;
	return rest;
}

//测试代码
//#include<iostream>
//int main()
//{
//拷贝控制测试：
//SingleList<int> s1;
//s1.pushFront(1);
//s1.pushFront(2);
//s1.pushFront(3);
//
//SingleList<int> s2(s1);
//SingleList<int> s3(std::move(s1));
//SingleList<int> s4 = s2;
//SingleList<int> s5 = std::move(s4);
// 功能测试：
//	SingleList<int> list;
//	list.pushFront(1);
//	list.pushFront(2);
//	list.pushFront(3);
//
//	for (auto item : list) std::cout << item << ' ';
//	std::cout << std::endl;
//
//	list.erase(2);
//	for (auto item : list) std::cout << item << ' ';
//	std::cout << std::endl;
//
//	if (list.find(5)) std::cout << "Yes, found 5";
//	else std::cout << "No, Not found 5";
//	std::cout << std::endl;
//
//	list.pushFront(4);
//	list.pushFront(5);
//	if (list.find(5)) std::cout << "Yes, found 5";
//	else std::cout << "No, Not found 5";
//	std::cout << std::endl;
//
//	list.removeAfter(list.begin());
//	for (auto item : list) std::cout << item << ' ';
//	std::cout << std::endl;
//
//	list.remove(3);
//	for (auto item : list) std::cout << item << ' ';
//	std::cout << std::endl;
//
//
//	std::cout << *list.max() << std::endl;
//
//	list.pushFront(6);
//	list.pushFront(7);
//	list.reverse(list.begin());
//	for (auto item : list) std::cout << item << ' ';
//	std::cout << std::endl;
//
//	system("pause");
//
//}


