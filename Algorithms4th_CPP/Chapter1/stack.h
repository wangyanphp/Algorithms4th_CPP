#pragma once
#include<forward_list>
#include<exception>
/**
*  The {@code Stack} class represents a last-in-first-out (LIFO) stack of generic items.
*  It supports the usual <em>push</em> and <em>pop</em> operations, along with methods
*  for peeking at the top item, testing if the stack is empty, and iterating through
*  the items in LIFO order.
*  <p>
*  This implementation uses a singly-linked list with a static nested class for
*  linked-list nodes. See {@link LinkedStack} for the version from the
*  textbook that uses a non-static nested class.
*  The <em>push</em>, <em>pop</em>, <em>peek</em>, <em>size</em>, and <em>is-empty</em>
*  operations all take constant time in the worst case.
*  <p>
* API
* Stack() 创建一个空栈
* void push(Item item) 添加一个元素
* Item pop() 删除最近添加的元素
* bool isEmpty() 栈是否为空
* int size() 栈中的元素数量
* 用于迭代
* Iter begin();
* Iter end();
*用于调试/测试
* string toString() 返回stack的字符串表示
*  @param <Item> the generic type of an item in this stack
*/

template<typename Item>
class Stack
{
public:
	typedef std::forward_list<Item> List;
	typedef typename List::iterator Iterator;
	typedef typename List::const_iterator CIterator;
private:
	List list;
	int N;//number of elements in bag
public:
	/**
	 *Initializes an empty Stack
	 */
	Stack() :list(List()),N(0) {}

	/**
	 *return true if this stack is empty
	 */
	bool isEmpty() const
	{
		return list.empty();
	}

	/**
	 * return the number of items in this stack
	 */
	int size() const
	{
		return N;
	}

	/**
	 * add the item to this stack
	 */
	void push(const Item &item)
	{
		list.push_front(item);
		++N;
	}
	void push(Item &&item)
	{
		list.push_front(std::forward<Item>(item));
		++N;
	}

	/*
	 *return and remove item most recently added in this stack
	 */
	Item pop()
	{
		if (isEmpty())
			throw std::underflow_error("Stack underflow");
		Item item = list.front();
		list.pop_front();
		--N;
		return item;
	}

	/*
	 *return the item most recently added in this stack
	 */
	Item peek() const
	{
		if (isEmpty())
			throw std::underflow_error("Stack underflow");
		return list.front();
	}

	/*
	*return the begin iterator
	*/
	Iterator begin() { return list.begin(); }
	CIterator begin() const { return list.begin(); }

	/*
	*return the end iterator
	*/
	Iterator end() { return list.end(); }
	CIterator end() const { return list.end(); }
};