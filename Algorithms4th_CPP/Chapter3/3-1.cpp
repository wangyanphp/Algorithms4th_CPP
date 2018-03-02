#include"ST2017.h"
#include<iostream>
#include<string>

/**
 *统计每个单词出现的次数
 */

void countFrequency(std::istream& in, int minLen)
{

	BST<std::string, int> st;
	std::string word;
	while (in >> word)
	{
		if (word.length() < minLen) continue;//忽略较短的单词
		if (!st.contains(word)) st.put(word, 1);
		else st.put(word, st.get(word) + 1);
	}

	auto q = st.toArray();
	word = q.begin()->key;
	int max = q.begin()->value;

	for(auto item:q)
		if (item.value > max)
		{
			word = item.key;
			max = item.value;
		}

	std::cout << word << '\t' << max << std::endl;
	

			
}
//
///**
// *3.1.25:缓存
// *if (!st.contains(word)) st.put(word, 1);
//	else st.put(word, st.get(word) + 1);
// *这种代码会在内循环中将同一个键查找两三遍
// *为了提高这种用例代码的效率，我们可以使用一种叫做缓存的技术，即：将访问最频繁的键的位置保存在一个变量中。
// *
// *对于SequentialSearchST，我们可以使用3.1.22自组织查找，这是一种将数组元素重新排序使得被访问频率较高的元素更容易被找到的查找算法
// *这样的话，在每次查找命中时，将被找到的键值对移动到数组的开头即可
// *
// *对于BinarySearchST，，，，，
// */
//
///**
// *3.1.28 有序插入，修改BinarySearchST，使得插入一个比当前所有键都大的键只需要常数时间
// *如此，则有序构造符号表时，只需要线性时间
// */
//	//std::ifstream in("E:\\tale.txt");
//
//	//countFrequency(in, 8);
//	
//	/*
//	std::vector<int> a{ 6, 3, 8, 1, 2, 4, 5, 7, 9, 0 };
//	BST<int, int> st;
//	for (int i = 0; i < a.size(); ++i)
//		st.put(a[i], a[i]);
//
//	st.deleteMin();
//	st.deleteMax();
//	st.deleteKey(6);
//	auto v = st.toArray();
//
//	for (auto item : v)
//		std::cout << item.key << '\t' << item.value << std::endl;
//		*/
