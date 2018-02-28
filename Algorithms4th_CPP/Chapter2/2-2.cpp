#include<vector>
#include<algorithm>
#include<type_traits>
#include<iostream>
#include"sort.h"
#include"../Chapter1/test.h"


/***************************************************************************************************************************
**************************************************练习题********************************************************************
**************************************************************************************************************************/

/**
 *2.2.12 次线性的额外空间：
 *可以参见侯捷《STL源码剖析》中merge对于空间的处理，
 *http://blog.csdn.net/shoulinjun/article/details/20702681
 */

/**
 *2.2.14 & 2.2.15 自底向上的有序队列排序
 */
#include"../Chapter1/queue.h"
template<typename It, typename T>
class __MergeSortQueue
{
private:
	Queue<Queue<T>> queues;
public:
	__MergeSortQueue(It beg, It end)
	{
		for (auto it = beg; it != end; ++it)
		{
			Queue<T> q;
			q.enqueue(*it);
			queues.enqueue(std::move(q));
		}

		while (queues.size() > 1)
		{//用队列来实现自底向上的逻辑
			auto first = queues.dequeue();
			auto second = queues.dequeue();
			queues.enqueue(merge(first, second));
		}
		auto q = queues.dequeue();//应该是移动构造

		for (auto it = beg; it != end; ++it)
			*it = q.dequeue();
	}

private:

	Queue<T> merge(Queue<T>& first, Queue<T>& second)
	{
		Queue<T> ret;
		auto left = first.dequeue();
		auto right = second.dequeue();
		while(ret.size()!=first.size()+second.size())
		{
			if (first.isEmpty()) ret.enqueue(second.dequeue());
			else if (second.isEmpty()) ret.enqueue(first.dequeue());
			else if (left <= right)
			{//<=为稳定排序
				ret.enqueue(left);
				left = first.dequeue();
			}
			else
			{
				ret.enqueue(right);
				right = second.dequeue();
			}
		}
		return ret;
	}	
};


/**
 *2.2.16 自然的归并排序------链表排序方法
 */
template<typename It, typename T>
class __MergeNature
{
private:
	std::vector<T> aux;
public:
	__MergeNature(It beg, It end) :aux(beg, end)
	{
		int N = end - beg;

		while (true)//终止条件：只剩一个有序数组:终止条件移到了内循环中
		{		
			int firstlo = 0, firsthi = 0;
			int secondhi = -1;
			int tmphi = 0;

			while (tmphi<N-1)
			{
				//寻找第一个有序数组
				tmphi = firstlo = secondhi + 1;
				while ((tmphi!=N-1)&&*(beg + tmphi) <= *(beg + tmphi + 1)) //有边界检测
					++tmphi;
				firsthi = tmphi;
				//检测
				if (firsthi - firstlo + 1 == N) return;
				if (firsthi == N - 1) break;

				//寻找第二个有序数组
				tmphi = firsthi + 1;
				while ((tmphi != N - 1) && *(beg + tmphi) <= *(beg + tmphi + 1)) //有边界检测
					++tmphi;
				secondhi = tmphi;

				merge(beg+firstlo, beg+firsthi, beg+secondhi);
			}
		}
	}

private:

	void merge(It beg, It mid, It back)
	{
		int lsize = (mid - beg) + 1;
		int rsize = (back - mid);

		for (int k = 0; k < lsize; ++k)
			aux[k] = *(beg + k);
		for (int k = 0; k < rsize; ++k)
			aux[lsize + rsize - 1 - k] = *(mid +1 + k);

		int i = 0, j = (back - beg);
		for (int k = 0; k < lsize + rsize; ++k)
		{
			if (aux[i] <= aux[j]) 
				*(beg + k) = aux[i++];
			else 
				*(beg + k) = aux[j--];
		}
	}

};

template<typename It>
void mergeSortNature(It beg, It end)
{
	__MergeNature<It, typename std::remove_reference<decltype(*beg)>::type>(beg, end);
}

/**
 *2.2.17：链表排序：自然归并是链表排序的最佳方法
 *类似于2.2.16，不再实现
 */

/**
 *2.2.18:重点是在Merge程序中，next=random（first，second）
 *这里注意是“打乱”分治，一定要注意逻辑上的推理
 */

/**
 *2.2.19：倒置：
 *能否分治？左右分开之后合并具有一定的便利性，所以才分治
 *改编最原始的mergesort程序
 */
template<typename It, typename T>
class __Inversion
{
private:
	std::vector<T> aux;

public:
	__Inversion(It beg, It end) :aux(beg, end)
	{
	}

	int count(It beg, It back)
	{
		if (back <= beg)return 0;
		int ret = 0;
		It mid = beg + (back - beg) / 2;
		ret += count(beg, mid);
		ret += count(mid + 1, back);
		ret += merge(beg, mid, back);
		return ret;
	}

private:

	int merge(It beg, It mid, It back)
	{
		int ret = 0;

		for (int k = 0; k <= (back - beg); ++k)
			aux[k] = *(beg + k);
		
		int i = 0, j = (mid - beg) + 1;
		for (int k = 0; k <= (back - beg); ++k)
		{
			if (i > (mid - beg)) *(beg + k) = aux[j++];
			else if (j > (back - beg)) *(beg + k) = aux[i++];
			else if (aux[i] <= aux[j]) *(beg + k) = aux[i++];
			else {
				*(beg + k) = aux[j++];
				ret += (mid -beg- i + 1);
			}
		}

		return ret;
	}
};


/**
 *2.2.20:间接排序
 *程序基本与原程序一致，只需要在访问数据的时候，解引用即可
 */
template<typename It, typename T>
class __IndexMerge
{
private:
	std::vector<int> aux;
	std::vector<int> index;
	std::vector<T> data;
public:
	__IndexMerge(It beg, It end) :data(beg, end)
	{
		for (int i = 0; i < data.size(); ++i)
		{
			index.push_back(i);
			aux.push_back(i);
		}
		sort(0, index.size()-1);//根据index排序
	}

private:
	void sort(int lo, int hi)
	{
		if (hi <= lo) return;
		int mid = lo + (hi - lo) / 2;
		sort(lo, mid);
		sort(mid + 1, hi);
		merge(lo, mid, hi);
	}

	void merge(int lo, int mid, int hi)
	{
		for (int k = lo; k <= hi; ++k)
			aux[k] = index[k];

		int i = lo, j = mid+1;
		for (int k = lo; k <= hi; ++k)
		{
			if (i > mid) index[k] = aux[j++];
			else if (j > hi) index[k] = aux[i++];
			else if (data[aux[i]] <= data[aux[j]]) index[k] = aux[i++];
			else index[k] = aux[j++];
		}
	}
};

/**
 *2.2.22:三路归并排序与K路归并程序，除了繁琐之外没有什么值得再去实现的地方
 *Merge程序需要注意的地方是：
 *我们的内循环是：for (int k = lo; k <= hi; ++k){}，
 *远比while(i<=mid && j<=hi) + while(i<=mid) +while(j<=hi)简单清晰优雅得多
 *因为前一种内循环是从输出考虑：不过怎么样，结果数组就N个元素，只循环N次就好
 *后一种是从输入考虑。
 *一定要仔细考察这两种表示方法
 */
template<typename It, typename T>
class __KMerge
{
private:
	std::vector<T> aux;
	std::vector<T> data;
	int K;
public:
	__KMerge(It beg, It end, int k) :data(beg, end), aux(beg, end), K(k)
	{
		sort(0, data.size() - 1);

		auto it2 = data.begin();
		for (auto it = beg; it != end; ++it, ++it2)
			*it = *it2;
	}

private:
	void sort(int lo, int hi)
	{
		if (hi <= lo + K)
		{
			insertionSortHalfChange(lo, hi);
			return;
		}

		std::vector<int> cutpoints(K + 1);
		//分成k段,用k+1个点表示[lo,mid1,mid2...hi]
		for (int i = 0; i < K; ++i)
		{
			cutpoints[i] = lo + i*(hi - lo) / K;
		}
		cutpoints[K] = hi;

		//分段sort
		sort(lo, cutpoints[1]);//[lo,mid]
		for (int i = 1; i < K; ++i)
		{
			sort(cutpoints[i] + 1, cutpoints[i + 1]);
		}
		//merge
		merge(cutpoints);
	}

	void merge(std::vector<int>& points)
	{
		//int lo = points[0];
		//int hi = points.back();
		for (int k = points.front(); k <= points.back(); ++k)
			aux[k] = data[k];

		std::vector<int> cur(K);
		cur[0] = points[0];
		for (int i = 1; i < K; ++i)
			cur[i] = points[i] + 1;

		//
		for (int k = points.front(); k <= points.back(); ++k)
		{
			//选取一个段（区间）t，cur[t]为归并时最小元素
			int t=-1;//当t为负时，表示未初始化
			for (int i = 0; i < K; ++i)
			{
				if (cur[i] > points[i + 1])//此区间已经取完
					continue;
				else if (t == -1)
					t = i;
				else if (aux[cur[i]] < aux[cur[t]])
					t = i;
			}
			data[k] = aux[cur[t]];
			++cur[t];
		}

	}

	void insertionSortHalfChange(int lo, int hi)
	{


		//不需要交换的插入排序,将赋值操作减半
		for (int i = lo+1; i <= hi; ++i)
		{ //将a[i]插入到a[0...i-1]之中
			auto t = data[i];
			int j = i;
			for (; (j>lo)&&t < data[j-1]; --j)
				data[j] = data[j-1];

			data[j] = t;
		}
	}
};
template<typename It>
void KMergeSort(It beg, It end, int k)
{
	__KMerge<It, typename std::remove_reference<decltype(*beg)>::type>(beg, end,k);
}


/**
 *2.2.23 比较原始算法与改进算法的性能 
 */

double time2_2(const std::string& alg, std::vector<double>& v)
{
	Stopwatch timer;
	auto beg = v.begin();
	auto end = v.end();
	if (alg == "Merge") mergeSort(beg, end);
	else if (alg == "MergeX") mergeSortX(beg, end);
	else if (alg == "2Merge") KMergeSort(beg, end, 2);
	else if (alg == "3Merge") KMergeSort(beg, end, 3);
	else if (alg == "4Merge") KMergeSort(beg, end, 4);
	else if (alg == "5Merge") KMergeSort(beg, end, 5);
	else if (alg == "6Merge") KMergeSort(beg, end, 6);
	else if (alg == "7Merge") KMergeSort(beg, end, 7);
	else if (alg == "8Merge") KMergeSort(beg, end, 8);
	else if (alg == "Insertion") insertionSort(beg, end);
	else if (alg == "Selection") selectionSort(beg, end);
	else if (alg == "Shell") shellSort(beg, end);
	else if (alg == "InsertionX") insertionSortX(beg, end);

	return timer.elapsedTime();
}

//int main()
//{
//	int N = 1000;
//	int T = 100;
//	double t1 = timeRondomInput(time2_2, "Merge", N, T);
//	double t2 = timeRondomInput(time2_2, "MergeX", N, T);
//	double t3 = timeRondomInput(time2_2, "InsertionX", N, T);
//	double t4 = timeRondomInput(time2_2, "Shell", N, T);
//	std::cout << t1 << '\t' << t2 << '\t' << t1 / t2 << '\n';
//	std::cout << t2 << '\t' << t3 << '\t' << t2 / t3 << '\n';
//	std::cout << t2 << '\t' << t4 << '\t' << t2 / t4 << '\n';
//	system("pause");
//}



/**
 *2.2.25 确定最佳K值
 *此电脑的测试结果是：3，4,5效果较好
 */
