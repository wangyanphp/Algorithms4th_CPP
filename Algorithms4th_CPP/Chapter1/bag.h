#pragma once
#include<forward_list>
#include<string>
/**
 * Bag类不支持删除元素，支持插入以及遍历。
 * 实现这种功能最佳数据结构是C++的vector
 * 但是因为不需要随机访问，所以List也可以使用
 * 因为实现一个类需要考虑的细节已经在singleList这个类中实现了，所以我们只需要包装一下就好
 *
 * API
 * Bag()				创建一个空背包
 * void add(Item item)	添加一个元素
 *
 * bool isEmpty()		判断背包是否为空
 * int size()			背包中的数量
 *
 * Iter begin();		迭代
 * Iter end();
 *
 * string toString()	返回bag的字符串表示
 */


template<typename Item>
class Bag
{
public:
	typedef std::forward_list<Item> List;
	typedef typename List::iterator Iterator;
	typedef typename List::const_iterator CIterator;
private:
	List list;
	int N;
public:
	Bag() :list(List()), N(0) {}//使用自带的拷贝控制函数
	bool isEmpty() const{ return list.empty(); }
	int size() const{ return N; }

	void add(const Item &item)
	{
		list.push_front(item);
		++N;
	}
	void add(Item &&item)
	{
		list.push_front(std::forward<Item>(item));
		++N;
	}


	Iterator begin() { return list.begin(); }
	CIterator begin() const { return list.begin(); }

	Iterator end() { return list.end(); }
	CIterator end() const { return list.end(); }

	/**
	 *return the string representation of this bag
	 *不好实现：Item必须有转换成string的统一API,java中有toString(),C++没有
	 */

};


