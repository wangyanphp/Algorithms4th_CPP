#include"string.h"
#include"trie.h"

#include<iostream>
#include<fstream>
#include"test.h"

int main()
{
	/*
	Alphabet alp("abcdefghijklmnopqrstuvwxyz");
	TrieST<int> trie(alp, -1);
	trie.put("wang", 1);
	
	trie.put("zhang", 2);
	trie.put("huang", 3);
	std::cout << trie.size()<<std::endl;
	*/

	TST<int> tst(-1);
	tst.put("wang", 1);
	tst.put("zhang", 2);
	tst.put("huang", 3);

	for (auto s : tst.keys())
		std::cout << s << std::endl;



	system("pause");
}