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


/**
*首先我们需要通过index来索引key
*一种实现方式是：pq[loc]保存的是<index,key>,aux[index]=loc
*这样的话，我们可以通过pq[aux[index]].key来实现访问。
*那么，如何实现优先队列呢？
*我们可以pq的key进行优先队列排序，每当更新pq时，同时更新aux即可。
*aux可以通过map来实现
*
*现在的好处是index是整数，如果index是0到N范围的话，我们就可以使用数组，而不是map来实现aux
*我们还可以实现的一点改进是：不用整体移动<index,key>,只移动index来进行排序
*此时qp[index]=loc, pq[loc]=index, 如果keys[index]=key，那么就不需要移动key，只需要关系qp和pq即可
*根据之前的分析，我们进行优先队列操作的是pq数组，当改变loc时，同时更新qp，这一点不能弄混了
*抓住了这一点，我们就可以写出IndexMinPQ的代码了
*/
template<typename T, typename Compare = std::less<T>>
class IndexPQ
{
private:
	int N = 0;
	std::vector<int> pq; //索引二叉堆
	std::vector<int> qp; //逆序：qp[pq[i]]=pq[qp[i]]=i
	std::vector<T> keys;
	//备注：qp[indx]是位置。keys[qp[index]]=key, pq[qp[index]]=index
	Compare comp;
public:
	explicit IndexPQ(int maxN)
		:pq(maxN + 1), qp(maxN + 1), keys(maxN + 1)
	{
		for (int i = 0; i < qp.size(); ++i)
			qp[i] = -1;
	}
	void push(int i, T k)
	{
		if (contains(i))
			return;
		++N;
		qp[i] = N;
		keys[i] = k;
		pq[N] = i;
		swim(N);
	}
	int indexOfTop()
	{
		return pq[1];
	}
	T top()
	{
		return keys[pq[1]];
	}

	int pop()
	{//删除key值最小的项，返回其对应的索引
		int min = pq[1];
		exch(1, N--);
		sink(1);

		qp[min] = -1;
		pq[N + 1] = -1;
		return min;
	}

	T keyOf(int i)
	{//返回索引所对应的键值
		_ASSERT(contains(i));
		return keys[i];
	}

	void changeKey(int i, T k)
	{
		if (greater(k, keys[i]))
		{
			keys[i] = k;
			sink(qp[i]);
		}
		else
		{
			keys[i] = k;
			swim(qp[i]);
		}
	}

	bool contains(int i) { return qp[i] != -1; }
	void deleteT(int i)
	{
		_ASSERT(contains(i));
		int loc = qp[i];
		exch(loc, N--);
		swim(loc);
		swim(loc);
		qp[i] = -1;
	}

	bool isEmpty() { return N == 0; }
	int size() { return N; }

private:
	bool compare(int i, int j)
	{
		return comp(keys[pq[i]], keys[pq[j]]);
	}

	void exch(int i, int j)
	{
		//两个位置的index互换
		std::swap(pq[i], pq[j]);
		std::swap(qp[pq[i]], qp[pq[j]]);
	}

	void swim(int k)
	{
		while (k > 1 && compare(k >> 1, k))
		{
			exch(k, k >> 1);
			k >>= 1;
		}
	}

	void sink(int k)
	{
		while ((k << 1) <= N)
		{
			int j = k << 1;
			if (j < N && compare(j, j + 1)) ++j;
			if (!compare(k, j))break;
			exch(k, j);
			k = j;
		}
	}
};