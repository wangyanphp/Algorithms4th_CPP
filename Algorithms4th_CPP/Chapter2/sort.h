#pragma once
#include<type_traits>
#include"../Chapter1/random.h"
//实现最常用的几个排序算法，使用迭代器作为接口
using std::iterator_traits;
/**
 *2.1---选择排序（原址排序、不稳定以58528为例）
 *
 *分析：对于长度为N的数组，选择排序需要大约N^2/2次比较和N次交换
 *可以通过排序轨迹来证明这一点
 *
 *由此我们可以得到以下结论：
 *1.运行时间与输入无关
 *2.数据的移动次数是最少的
 */

template<typename BidirectionalIt>
void selectionSort(BidirectionalIt first, BidirectionalIt last)
{
	typedef typename iterator_traits<BidirectionalIt>::value_type value_type;

	if (first == last) return;
	for (auto i = first; i != last; ++i)
	{//将a[i]与a[i+1..N)中的最小元素交换
		auto min = i;
		for (auto j = i + 1; j != last; ++j)
		{
			if (*j < *min) min = j;
		}
		using std::swap;
		swap(*i, *min);
	}
}


/**
 *2.1----插入排序（原址排序，稳定）
 *
 *分析：对于长度为N的数组，平均情况下插入排序需要~N^2/4次比较和~N^2/4次交换。最坏情况下需要~N^2/2次比较和~N^2/2次交换
 *最好情况下需要N-1次比较和0次交换
 *
 *插入排序需要的交换操作的数目与数组中倒置的数目相同，需要的比较数目等于倒置数目+（N-1）
 */
//原始低配版：
template<typename BidirectionalIt>
void insertionSort(BidirectionalIt first, BidirectionalIt last)
{
	typedef typename iterator_traits<BidirectionalIt>::value_type value_type;

	if (first == last) return;
	for (auto i = first + 1; i != last; ++i)
	{//将a[i]插入到a[0...i-1]之中
		for (auto j = i; j > first && *j < *(j - 1); --j)
		{
			using std::swap;
			swap(*j, *(j - 1));
		}
	}
}

/**
 * 根据2.1.24——添加哨兵 & 2.1.25 减少交换
 * 根据这两条建议，提高insert排序的性能
 *
 *设立哨兵，将最小元素放到最左侧，不能采用select 的方法，因为这会导致算法不稳定
 */
template<typename BidirectionalIt>
void insertionSortX(BidirectionalIt first, BidirectionalIt last)
{
	typedef typename iterator_traits<BidirectionalIt>::value_type value_type;

	if (first == last) return;
	// put smallest element in position to serve as sentinel
	int exchanges = 0;
	for (auto i = last - 1; i != first; --i) 
	{
		if (*i < *(i-1))
		{
			using std::swap;
			std::swap(*i, *(i - 1));
			++exchanges;
		}
	}
	if (exchanges == 0) return;

	//不需要交换的插入排序,将赋值操作减半
	//insertion sort with half-exchanges
	for (auto i = (first + 1) + 1; i != last; ++i)
	{
		//将a[i]插入到a[0...i-1]之中
		auto t = *i;
		auto j = i;
		for (; t < *(j - 1); --j)
			*j = std::move(*(j - 1));

		*j = std::move(t);
	}
}


/**
 * 2.1---希尔排序
 * 希尔排序更高效的原因是：它权衡了子数组的规模和有序性
 */
template<typename RandomAcessIt>
void shellSort(RandomAcessIt first, RandomAcessIt last)
{
	int N = (last - first);
	int h = 1;
	while (h < N / 3) h = 3 * h + 1;//1, 4, 13, 40, 121, 364, 1093, ...

	using std::swap;
	while (h >= 1)
	{ //将数组变成h有序
		for (int i = h; i < N; ++i)
		{//将a[i]插入到a[i-h..i-2*h..]之中
			auto t = *(first + i);
			auto j = first + i;
			for (; j>=(first+h) && t < *(j - h); j-=h)
				*j = std::move(*( j - h));
			*j = std::move(t);
		}
		h = h / 3;
	}
}

//判断是否已经排序
template<typename It>
bool isSorted(It first, It last)
{
	auto prev = first;
	auto cur = first; ++cur;
	for (; cur != last; ++prev, ++cur)
	{
		if (*cur < *prev) return false;
	}
	return true;
}



//用于测试排序算法的性能：
//对于排序算法性能测试
template<typename Func>
double timeRondomInput(Func time, const std::string& alg, int N, int T)
{ //使用算法将长度为N的数组排序T次
	double total = 0;
	std::vector<double> a(N);
	static Random r;
	for (int t = 0; t < T; ++t)
	{
		for (int i = 0; i < N; ++i)
			a[i] = r.uniform(0, 100);

		total += time(alg, a);
	}

	if (!isSorted(a.begin(), a.end()))
	{
		return 10000;
	}
	return total;
}

/**
 *2-2 归并排序
 */

 /**
 *归并排序：利用分治思想，如果它能将两个子数组排序，它就能通过归并两个子数组来将整个数组排序
 */

template<typename RandomAcessedIt>
class __Merge
{
	//typedef typename std::remove_reference<decltype(*beg)>::type value_type;
	typedef typename std::iterator_traits<RandomAcessedIt>::value_type value_type;

private:
	std::vector<value_type> aux;

public:
	__Merge(RandomAcessedIt first, RandomAcessedIt last) :aux(first, last)
	{
		sort(first, last - 1);
	}

private:
	void sort(RandomAcessedIt lo, RandomAcessedIt hi)
	{//将数组[lo..hi]排序
		if (hi <= lo) return;
		auto mid = lo + (hi - lo) / 2;
		sort(lo, mid);
		sort(mid + 1, hi);
		merge(lo, mid, hi);
	}

	void merge(RandomAcessedIt lo , RandomAcessedIt mid, RandomAcessedIt hi)
	{//归并[lo mid] 和[mid+1 hi]
		auto i = lo, j = mid+1;
		for (int k = 0; k<=(hi-lo); ++k)
		{
			if (i > mid) aux[k] = std::move(*j++);
			else if (j > hi) aux[k] = std::move(*i++);
			else if (*i < *j) aux[k] = std::move(*i++);
			else aux[k] = std::move(*j++);
		}

	
		i = lo;
		for (int k = 0; k <= (hi - lo); ++k, ++i)
			*i = std::move(aux[k]);	
		_ASSERT(isSorted(lo, hi));
	}
};

template<typename RandomAcessedIt>
void mergeSort(RandomAcessedIt beg, RandomAcessedIt end)
{
	__Merge<RandomAcessedIt>(beg, end);
}

/**
*分析：
*对于长度为N的任意数组，自顶向下的归并排序需要1/2NlgN至NlgN次比较
*参见命题F，归并长度为N的数组，需要N/2至N此比较
*对于长度为N的任意数组，自顶向下的归并排序最多需要6NlgN次访问数组
*每次归并，最多需要访问数组6N次（2N用来复制，2N次用来将排序好的元素移动回去，最多进行N次比较（2N次访问数组））
*
*对于这种程序，lgN层的运算代价，要一层作为整体来考虑
*/

/**
*自底向上的归并排序
*/
template<typename RandomAcessedIt>
class __MergeBU
{
	typedef typename std::iterator_traits<RandomAcessedIt>::value_type value_type;
private:
	std::vector<value_type> aux;

public:
	__MergeBU(RandomAcessedIt beg, RandomAcessedIt end) :aux(beg, end)
	{
		int N = end - beg;

		for (int sz = 1; sz < N; sz = sz + sz)//sz是子数组的大小
			for (int lo = 0; lo < N - sz; lo += sz + sz)
				merge(beg + lo, beg + lo + sz - 1, beg + std::min(lo + sz + sz - 1, N - 1));
	}

private:

	void merge(RandomAcessedIt lo, RandomAcessedIt mid, RandomAcessedIt hi);//使用2.2.10的快速归并
};

/**
*分析：
*对于长度为N的任意数组，自底向上的归并排序需要1/2NlgN至NlgN次比较，最多需要6NlgN次数组访问
*当数组的长度是2的幂时，自顶向下和自底向上的归并排序比较次数和数组访问次数正好相同，只是顺序不同
*/
/**
*2.2.10:快速归并：可以省去内循环中检测某半边是否用尽的代码
*由于第二段倒置的关系，此排序是不稳定排序。
*/
template<typename RandomAcessedIt>
void __MergeBU<RandomAcessedIt>::merge(RandomAcessedIt lo, RandomAcessedIt mid, RandomAcessedIt hi)
{
	int lsize = (mid - lo) + 1;
	int rsize = (hi - mid);

	for (int k = 0; k < lsize; ++k)
		aux[k] = std::move(*(lo + k));
	for (int k = 0; k < rsize; ++k)
		aux[lsize + rsize - 1 - k] = std::move(*(mid + k));

	auto i = aux.begin(), j = aux.begin() + (hi - lo);
	auto ret = lo;
	for (int k = 0; k < lsize + rsize; ++k, ++ret)
	{
		if (*i < *j) *ret = std::move(*i++);
		else *ret = std::move(*j--);
	}

}
/**
*2.2.8, 2.2.11, 2.2.10:改进归并排序的算法
*1.加快小数组的排序速度；2.检测数组是否已经有序；3.通过在递归中交换参数来避免数组复制
*对于1来说：假设CUTOFF=8，使用mergesort，则需要三层，每层需要6N次数组访问，总共18N，而使用插入排序则平均为1/4*（N/8)*8^2=N/2，效率高了36倍
*对于2来说：如果数组时有序的，那么只需要线性时间
*对于3来说：省去了移动数据的2N操作，效率提升了1/3
*/
template<typename RandomAcessedIt>
class __MergeX
{
	typedef typename std::iterator_traits<RandomAcessedIt>::value_type value_type;
private:
	std::vector<value_type> aux;
	int CUTOFF = 15;

public:
	__MergeX(RandomAcessedIt beg, RandomAcessedIt end) :aux(beg, end)
	{
		sort( aux.begin(), aux.end() - 1, beg, end - 1);
	}

private:
	void sort(RandomAcessedIt srclo, RandomAcessedIt srchi, RandomAcessedIt destlo, RandomAcessedIt desthi)
	{//将数组[srclo,srchi]排序到[destlo,desthi],其中必须开始时，两个区间对应元素相等

	 //小数组用插入排序提高速度
		if ((srchi - srclo) <= CUTOFF)
		{
			insertionSortX(destlo, desthi + 1);
			return;
		}

		//
		auto srcmid = srclo + (srchi - srclo) / 2;
		auto destmid = destlo + (desthi - destlo) / 2;

		sort(destlo, destmid, srclo, srcmid);
		sort(destmid + 1, desthi, srcmid + 1, srchi);

		//检测是否已经有序
		if (*srcmid <= *(srcmid + 1))
		{
			std::copy(srclo, srchi, destlo);
			return;
		}

		merge(srclo, srcmid, srchi, destlo, desthi);
	}

	void merge(RandomAcessedIt srclo, RandomAcessedIt srcmid, RandomAcessedIt srchi, RandomAcessedIt destlo, RandomAcessedIt desthi)
	{//将[srclo, srcmid,srchi]归并到[destlo,desthi],其中source已经有序

		auto i = srclo, j = srcmid + 1;

		auto dest = destlo;
		for (int k = 0; k <= (srchi - srclo); ++k, ++dest)
		{
			if (i > srcmid) *dest = std::move(*j++);
			else if (j > srchi) *dest = std::move(*i++);
			else if (*i < *j) *dest = std::move(*i++);
			else *dest = std::move(*j++);
		}

	}
};

template<typename RandomAcessedIt>
void mergeSortX(RandomAcessedIt beg, RandomAcessedIt end)
{
	__MergeX<RandomAcessedIt>(beg, end);
}







/**
 *2.3 快速排序
 *
 */

 ////快速排序 VS 归并排序
 ////两者互补：归并排序是将数组分成两个子数组分别排序，并将有序的子数组归并以将整个数组排序；->数组处理发生在递归之后
 ////快速排序是当两个子数组都有序的时候，整个子数组也就自然有序了->数组处理发生在递归之前
 template<typename It>
 class __Quick
 {
 
 public:
 	void sort(It beg, It end)
 	{
 		static Random r;
 		r.shuffle(beg, end);//随机化，消除对输入的依赖
 		__sort(beg, end-1);
 	}
 private:
 	void __sort(It beg, It back)
 	{
 		if (back-beg <= 0) return;
 		auto j = partition(beg, back);
 		if (j != beg)
 			__sort(beg, j - 1);
 		if (j != back)
 			__sort(j + 1, back);
 	}
 
 	/**
 	 *注意此技法
 	 *while(true){while(something){if(something) return/break;} 
 	 *注意双层嵌套循环的终止条件和安排
 	 */
 
 	It partition(It beg, It back)
 	{
 		It i = beg;
 		It j = back+1;//左右扫描指针,注意跟下面的递增递减相统一
 		auto v = *beg;//切分元素
 		
 		//循环不变式是：i左侧的元素都不大于v，j右侧的元素都不小于v
 		while (true)
 		{//扫描元素
 			while (*++i < v)
 			{
 				if (i == back) break;
 			}
 			while (v < *--j)
 			{
 				if (j == beg) break;
 			}
 			//当指针i和j相遇时退出主循环
 			if (i-j >= 0) break;
 			
			using std::swap;
 			swap(*i, *j);
 		}
 		//有*j不大于v，而*i不小于v，根据184页图例
		using std::swap;
 		swap(*beg, *j);
 		return j;
 	}
 
 };
 
 template<typename It>
 void quickSort(It beg, It end)
 {
 	static __Quick<It> q;//static是为了random
 	q.sort(beg, end);
 }



 
 /**
  *可以进行的改进：
  *1.和大部分递归程序一样，对于小数组，快速排序比插入排序慢；
  *2.三取样切分：使用子数组的一小部分元素的中位数来切分数组
  *3.熵最优的排序：三切分
  */
 template<typename It>
 class __Quick3Way
 {
 public:
 	void sort(It beg, It end)
 	{
 		static Random r;
 		r.shuffle(beg, end);//随机化，消除对输入的依赖
 		__sort(beg, end - 1);
 	}
 private:
 	void __sort(It beg, It back)
 	{
 		if (back <= beg) return;
 		It lt = beg, i = beg + 1, gt = back;
 		auto v = *beg;
 		//[beg,lt-1]的元素小于v,[lt..i-1]的元素等于v，[gt+1,back]的元素大于v，处理[i,gt]的元素
 		//注意这种循环遍历方式与上面那种的不同
 		while (i <= gt)
 		{
			using std::swap;
 			if (*i < v) swap(*lt++, *i++);
 			else if (*i>v) swap(*i, *gt--);
 			else  ++i;
 		}
 
 		__sort(beg, lt - 1);
 		__sort(gt + 1, back);
 	}
 
 };
 
 template<typename It>
 void quick3WaySort(It beg, It end)
 {
 	static __Quick3Way<It> q;//static是为了random
 	q.sort(beg, end);
 }



/**
 *2.4 堆排序
 */

#include"pq.h"

 template<typename InputIt>
 void heapSort(InputIt beg, InputIt end)
 {
	 typedef typename std::iterator_traits<InputIt>::value_type value_type;
	 PQ<value_type, std::greater<value_type>> pq(beg,end);
	 auto i = beg;
	 while (!pq.isEmpty())
	 {
		 *i++ = pq.pop();
	 }
 }
 








