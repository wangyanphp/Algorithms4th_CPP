#pragma once


/**
 *  The {@code PQ} class represents a priority queue of generic keys.
 *  It supports the usual <em>insert</em> and <em>delete-the-maximum</em>
 *  operations, along with methods for peeking at the maximum key,
 *  testing if the priority queue is empty, and iterating through
 *  the keys.
 *  <p>
 *  This implementation uses a binary heap.
 *  The <em>insert</em> and <em>delete-the-maximum</em> operations take
 *  logarithmic amortized time.
 *  The <em>max</em>, <em>size</em>, and <em>is-empty</em> operations take constant time.
 *  Construction takes time proportional to the specified capacity or the number of
 *  items used to initialize the data structure.
 *  <p>
 * API
 * PQ() 创建一个优先队列
 * PQ(int max) 创建一个最大容量为max的优先队列
 * PQ(It beg, It end) 用a[]的元素创建一个优先队列
 * void push(Key v) 向优先队列插入一个元素
 * Key top() 返回最大/最小元素
 * Key pop() 删除并返回最大/最小元素
 * bool isEmpty() 返回队列是否为空
 * int size() 返回优先队列中的元素个数
 */

/**
 *优先队列的实现方式多种多样，就其底层实现而言，可以基于链表或者数组
 *无论是哪一种，可以基于stack的变式来实现：
 *无序实现：插入元素->push();delMax：将边界元素与最大元素互换，然后pop（）
 *有序实现：插入元素->插入排序->pop()；delMax，直接pop（）
 *
 *然而，我们要学的东西，并不是上述的简单实现，而是二叉堆实现
 *当一棵二叉树的每个结点都大于等于它的两个子结点时，它被称为堆有序。
 *二叉堆是一组能够用堆有序的完全二叉树排序的元素，并在数组中按照层级存储（可以不使用数组的第一个位置）
 */

//MaxPQ -> Compare = less<T>
//MinPQ -> Compare = greater<T>
#include<vector>
#include<functional>
template<typename T, typename Compare= std::less<T>>
class PQ
{
public:
	explicit PQ(const Compare &x = Compare()) :comp(x) {}
	template<typename InputIt> PQ(InputIt first, InputIt last, const Compare &x = Compare())
		:pq(first, last), comp(x)
	{
		int N = pq.size();
		for (int k = (N-1) / 2; k >= 0; --k)
			sink(k);
	}

	void push(const T &t)
	{
		pq.push_back(t);
		int N = pq.size();
		swim(N - 1);
	}

	T pop()
	{
		T ret = pq[0];
		int N = pq.size();
		pq[0] = pq[N-1];
		pq.pop_back();
		if(!pq.empty())sink(0);
		return ret;
	}
	
	T top() const { return pq[0]; }
	bool isEmpty() { return pq.empty(); }
	int size() { return pq.size(); }

private:
	void swim(int k)
	{
		auto key = pq[k];
		while (k > 0 && comp(pq[(k-1) >> 1], key))
		{
			//exch((k-1) >> 1, k);
			//k = (k - 1) >> 1;
			//使用2.4.26 借鉴插入排序，减少交换次数
			pq[k] = std::move(pq[(k - 1) >> 1]);
			k = (k - 1) >> 1;
		}
		pq[k] = std::move(key);
	}

	void sink(int k)
	{
		auto key = pq[k];
		int N = pq.size();

		while ((k<<1) +1 < N)
		{
			int j = (k << 1) + 1;
			if (j < N-1 && compare(j, j + 1)) ++j;
			if (!comp(key, pq[j])) break;
			//exch(k, j);
			pq[k] = std::move(pq[j]);
			k = j;
		}
		pq[k] = std::move(key);
	}

	bool compare(int i, int j)
	{
		return comp(pq[i], pq[j]);
	}

	void exch(int i, int j)
	{
		using std::swap;
		swap(pq[i], pq[j]);
	}

private:
	std::vector<T> pq;
	Compare comp;
};