#include"st.h"
#include<string>
#include<iostream>
using std::string;

/**
 *3.3.29 最优存储：实现无需为结点颜色的存储使用额外空间：要将结点标记为红色，只需要交换它的左右链接

 *
 */
//int main()
//{
//	BST<int, string> st;
//	st.put(1, "wangyan");
//	st.put(2, "zhangpengcheng");
//	st.put(3, "wangshanshi");
//	st.put(4, "april");
//
//	std::cout << st.get(4)->second << std::endl;
//	std::cout << st.floor(3)->second << std::endl;
//	std::cout << st.ceiling(2)->second << std::endl;
//
//	std::cout << st.rank(3) << std::endl;
//
//
//	st.deleteKey(3);
//
//
//	for (auto item:st)
//		std::cout << item.first << ":" << item.second << std::endl;
//
//	system("pause");
//}

//class A
//{
//public:
//	void f() { ff(); }
//	virtual void ff() { std::cout << "AA" << std::endl; }
//};
//
//class B :public A
//{
//public:
//	void f() { A::f(); }
//	virtual void ff() { std::cout << "BB" << std::endl; }
//};
//
//
//int main()
//{
//	B b;
//	b.f();
//	system("pause");
//}

//int main()
//{
//	RedBlackBST<int, string> st;
//	st.put(1, "wangyan");
//	st.put(2, "zhangpengcheng");
//	st.put(3, "wangshanshi");
//	st.put(4, "april");
//
//	std::cout << st.get(4)->second << std::endl;
//	std::cout << st.floor(3)->second << std::endl;
//	std::cout << st.ceiling(2)->second << std::endl;
//
//	std::cout << st.rank(3) << std::endl;
//
//
//	//if (st.is23Tree()) 
//	{
//		for (auto item : st) { std::cout << item.first << ":" << item.second << std::endl; }
//
//	}
//
//
//	system("pause");
//}

#include<vector>
using std::vector;
class A
{
public:
	vector<int> a = { 1,2,3 };
	
	void f()
	{
		for (auto item : a)
			std::cout << item;
	}
};

class B :public A
{
public:
	vector<string> a = { "a","b","c" };
};

int main()
{
	B b;
	b.f();
	system("pause");
}