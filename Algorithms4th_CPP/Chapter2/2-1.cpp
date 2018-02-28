/*
 学习排序算法仍有三大实际意义：
 1. 对排序算法的分析将有助于全面理解本书中比较算法性能的方法
 2. 类似的技术也能有效解决其他类型的问题
 3. 排序算法常常是我们解决问题的第一步

 相对简单的算法的原因在于：
 1. 这些简单的算法在某些情况下比我们之后讨论的复杂算法更有效
 2. 它们有助于我们改进复杂算法的效率
 */
#pragma once

#include"../Chapter1/test.h"
#include"../Chapter1/random.h"
#include<algorithm>
#include<vector>
#include<string>
#include<iostream>
#include"sort.h"

//比较各个排序算法的性能
double time2_1(const std::string& alg, std::vector<double>& v)
{
	Stopwatch timer;
	auto beg = v.begin();
	auto end = v.end();
	if (alg == "Insertion") insertionSort(beg, end);
	else if (alg == "Selection") selectionSort(beg, end);
	else if (alg == "Shell") shellSort(beg, end);
	else if (alg == "InsertionX") insertionSortX(beg, end);

	return timer.elapsedTime();
}



//int main()
//{
//	int N = 10000;
//	int T = 10;
//	double t1 = timeRondomInput(time2_1, "Insertion", N, T);
//	double t2 = timeRondomInput(time2_1, "InsertionX", N, T);
//	double t3 = timeRondomInput(time2_1, "Shell", N, T);
//	std::cout << t1 << '\t' << t2 << '\t' << t1 / t2 << '\n';
//	std::cout << t2 << '\t' << t3 << '\t' << t2 / t3 << '\n';
//	system("pause");
//}

/*************************************************************************************************************************
********************************************练习题*************************************************************************
*************************************************************************************************************************/

/**
 *2.1.14：出列排序:此题看似跟插入排序有关（纸牌游戏），却只能看最开始的两张牌->（我们能找出最大值）；
 *本质上是选择排序：找出最大值，然后放到指定的位置
 *对length，保持最上面的一张是最大值；然后移到最底下
 *对length-1，执行上述过程
 *循环不变式：对于第i次循环，最底下的i-1z张牌：从大到小排列。
 */

//两个操作：1.将首元素移到队尾；2.交换开头的两个元素
//为了方便起见，我们使用双向队列
#include<deque>
using std::deque;
void moveHeadToTail(std::deque<int>& dq)
{
	auto item = dq.front();
	dq.pop_front();
	dq.push_back(item);
}

void changeTwoHeadItems(std::deque<int>& dq)
{
	auto first = dq.front();
	dq.pop_front();
	auto second = dq.front();
	dq.pop_front();

	dq.push_front(first);
	dq.push_front(second);
}

void dequeueSort(std::deque<int>& dq)
{
	//保持第一张牌是最大的
	for (int length = dq.size(); length > 0; --length)
	{   //前length个元素，寻找最大值
		for (int i = 1; i < length; ++i)
		{
			if (dq[0] > dq[1])
			{
				changeTwoHeadItems(dq);
				moveHeadToTail(dq);
			}
			else {
				moveHeadToTail(dq);
			}
		}
		//此时，前size()-length+1个元素，为从小到大的排序
		for (int i = 0; i < dq.size() - length + 1; ++i)
			moveHeadToTail(dq);
	}
}

///*2.1.15:交换操作太昂贵，所以用选择排序，因为它交换次数最少*/
//
///**
// *2.1.27：各个排序算法的增长数量级
// */
//double timeTria2_1_27(int N)
//{
//	static Random r;
//	int MAX = 1000000;
//	std::vector<int> v;
//	for (int i = 0; i < N; ++i)
//		v.push_back(r.uniformInt(-MAX, MAX));
//
//	Stopwatch timer;
//	shellSort(v.begin(), v.end());
//	return timer.elapsedTime();
//}
//
