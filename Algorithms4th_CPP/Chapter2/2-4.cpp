#include<iostream>
#include"pq.h"
#include"../Chapter1/stack.h"

/**
*一个用例TopM：对于输入N（可以非常大），找出最大的M个数
*（如果所有的N一口气输入，可以排序算法，可以用quick sort，如果是平均情况下，只需要2N次比较就可以）（具体行为参见寻找第M大元素）
*对于这种不是一口气输入的，可以使用优先队列
*/

void topM()
{
	int M = 10;
	PQ<int> pq;
	while (std::cin)
	{
		int tmp;
		std::cin >> tmp;
		pq.push(tmp);
		if (pq.size() > M)
		pq.pop();
	}

	Stack<int> stack;
	while (!pq.isEmpty())
	stack.push(pq.pop());

	for (auto item : stack)
	std::cout << item << std::endl;
}

////测试PQ：
//
//int main()
//{
//	std::vector<int> vec{ 10,9,8,7,6,5,4,3,2,1 };
//	PQ<int> pq(vec.begin(), vec.end());
//	//PQ<int,std::greater<int>> pq(vec.begin(), vec.end());
//
//	while (!pq.isEmpty())
//	{
//		std::cout << pq.pop() << std::endl;
//	}
//
//	system("pause");
//
//}


//
///**
// *2.4.26：参考插入排序的2.1.25，能够减少一半的交换操作
// */

/**
 *2.4.30:动态中位数查找。要查找中位数，依然可以采用查找第M大元素的方法，但是，重点在于动态查找
 *使用两个堆来：左边是保持小于中位数的最大优先队列；右边是保持大于等于中位数的最小优先队列
 *只存在两种情况：num(left)=num(right),或者num(left)+1=num(right)
 */

template<typename T>
class MedianFinding
{
public:
	MedianFinding() = default;
	template<typename It> MedianFinding(It beg, It end)
	{
		for (auto it = beg; it != end; ++it)
			push(*it);
	}

	void push(T t)
	{
		if (right.isEmpty())//没有元素
		{
			right.push(t);
			med = t;
			return;
		}
		
		//将t插入适当的位置
		if (t < med)
			left.push(t);
		else
			right.push(t);

		//可能有四种状态：插入到left-> num(left)=num(right)+1; num(left)=num(right);
		//插入到right-> num(left)+1= num(right); num(left)+2 =num(right)
		//此时状态可能会改变为：num(left)=num(right)+1; num(right)=num(left)+2,处理好这两种情况即可
		//可以考虑{1,2,2,2,2,2,3}这种含有重复元素的情况

		if (left.size() == right.size() + 1)//必然之前有num(left)==num(right),此时必然有奇数个元素
			right.push(left.pop());
		else if(right.size()==left.size()+2)//之前必然有num(right)==num(left)+1,此时必然有偶数个元素
			left.push(right.pop());
			

		if (left.size() == right.size())
			med = (left.top() + right.top()) / 2;
		else// if(left.size()+1==right.size())
			med = right.top();
	}

	T median() { return med; }
private:
	PQ<T,std::less<T>> left;
	double med;
	PQ<T, std::greater<T>> right;
};

//int main()
//{
//	std::vector<int> vec{ 10,9,8,7,6,5,4,3,2,1 };
//	MedianFinding<double> pq(vec.begin(), vec.end());
//	//PQ<int,std::greater<int>> pq(vec.begin(), vec.end());
//	std::cout << pq.median() << std::endl;
//
//	system("pause");
//}

///**
//*首先我们需要通过index来索引key
//*一种实现方式是：pq[loc]保存的是<index,key>,aux[index]=loc
//*这样的话，我们可以通过pq[aux[index]].key来实现访问。
//*那么，如何实现优先队列呢？
//*我们可以pq的key进行优先队列排序，每当更新pq时，同时更新aux即可。
//*aux可以通过map来实现
//*
//*现在的好处是index是整数，如果index是0到N范围的话，我们就可以使用数组，而不是map来实现aux
//*我们还可以实现的一点改进是：不用整体移动<index,key>,只移动index来进行排序
//*此时qp[index]=loc, pq[loc]=index, 如果keys[index]=key，那么就不需要移动key，只需要关系qp和pq即可
//*根据之前的分析，我们进行优先队列操作的是pq数组，当改变loc时，同时更新qp，这一点不能弄混了
//*抓住了这一点，我们就可以写出IndexMinPQ的代码了
//*/
//template<typename T, typename Compare = std::less<T>>
//class IndexPQ
//{
//private:
//	int N = 0;
//	std::vector<int> pq; //索引二叉堆
//	std::vector<int> qp; //逆序：qp[pq[i]]=pq[qp[i]]=i
//	std::vector<T> keys;
//	//备注：qp[indx]是位置。keys[qp[index]]=key, pq[qp[index]]=index
//	Compare comp;
//public:
//	explicit IndexPQ(int maxN)
//		:pq(maxN + 1), qp(maxN + 1), keys(maxN + 1)
//	{
//		for (int i = 0; i < qp.size(); ++i)
//			qp[i] = -1;
//	}
//	void push(int i, T k)
//	{
//		if (contains(i))
//			return;
//		++N;
//		qp[i] = N;
//		keys[i] = k;
//		pq[N] = i;
//		swim(N);
//	}
//	int indexOfTop()
//	{
//		return pq[1];
//	}
//	T top()
//	{
//		return keys[pq[1]];
//	}
//
//	int pop()
//	{//删除key值最小的项，返回其对应的索引
//		int min = pq[1];
//		exch(1, N--);
//		sink(1);
//
//		qp[min] = -1;
//		pq[N + 1] = -1;
//		return min;
//	}
//
//	T keyOf(int i)
//	{//返回索引所对应的键值
//		_ASSERT(contains(i));
//		return keys[i];
//	}
//
//	void changeKey(int i, T k)
//	{
//		if (greater(k, keys[i]))
//		{
//			keys[i] = k;
//			sink(qp[i]);
//		}
//		else
//		{
//			keys[i] = k;
//			swim(qp[i]);
//		}
//	}
//
//	bool contains(int i) { return qp[i] != -1; }
//	void deleteT(int i)
//	{
//		_ASSERT(contains(i));
//		int loc = qp[i];
//		exch(loc, N--);
//		swim(loc);
//		swim(loc);
//		qp[i] = -1;
//	}
//
//	bool isEmpty() { return N == 0; }
//	int size() { return N; }
//
//private:
//	bool compare(int i, int j)
//	{
//		return comp(keys[pq[i]], keys[pq[j]]);
//	}
//
//	void exch(int i, int j)
//	{
//		//两个位置的index互换
//		std::swap(pq[i], pq[j]);
//		std::swap(qp[pq[i]], qp[pq[j]]);
//	}
//
//	void swim(int k)
//	{
//		while (k > 1 && compare(k >> 1, k))
//		{
//			exch(k, k >> 1);
//			k >>= 1;
//		}
//	}
//
//	void sink(int k)
//	{
//		while ((k << 1) <= N)
//		{
//			int j = k << 1;
//			if (j < N && compare(j, j + 1)) ++j;
//			if (!compare(k, j))break;
//			exch(k, j);
//			k = j;
//		}
//	}
//};

//int main()
//{
//	std::vector<int> vec{ 10,9,8,7,6,5,4,3,2,1 };
//	IndexPQ<double, std::greater<double>> pq(10);
//	pq.push(1, 3.2);
//	pq.push(2, 2.2);
//
//	while (!pq.isEmpty())
//	{
//		std::cout <<pq.top() <<  std::endl;
//		pq.pop();
//	}
//	system("pause");
//}



/**
*之前的多路归并程序的设计思路是：对于每一路保存一个顶头的指针，然后找出最大值来，复杂度是M
*我们需要保存两个信息：哪一路？这一路的哪个元素？
*这里的优先队列可以使得复杂度变为logM
*/
template<typename T>
std::vector<T> MultiWayMerge(std::vector<std::vector<T>>& a)
{
	std::vector<T> ret;

	int M = a.size();
	std::vector<int> points;
	for (int i = 0; i < M; ++i)
		points.push_back(0);
	IndexPQ<T,std::greater<T>> pq(M);
	for (int i = 0; i < M; ++i)
		if (points[i] != a[i].size())
			pq.push(i, a[i][points[i]++]);

	while (!pq.isEmpty())
	{
		ret.push_back(pq.top());
		int i = pq.pop();
		if (points[i] != a[i].size())
			pq.push(i, a[i][points[i]++]);
	}

	return ret;
}

//int main()
//{
//	std::vector<std::vector<int>> a;
//	a.push_back({ 1,3,5,7,9 });
//	a.push_back({ 2,4,6,8,10 });
//
//	auto ret = MultiWayMerge(a);
//
//	for (auto item : ret)
//		std::cout << item << std::endl;
//	system("pause");
//}

/**
*一点说明：索引优先队列的用处是什么呢？
*类似于多路归并时一种情况
*但是其他的应用还需要更加深入的探究
*理解这种数据结构的一个较好方法是将它看做一个能够快速访问其中最小元素的数组
*/




