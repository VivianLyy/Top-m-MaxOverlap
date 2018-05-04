
#ifndef SET_H
#define SET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qsort.h"
#include "set.h"
#include "rtree.h"
#include <map>
// template <class Key, class T, class Compare = less<Key>, class Alloc = alloc>
// 第一个参数Key是关键字类型，第二个参数T是值类型，第三个参数Compare是比较函数（仿函数），第四个参数是内存配置对象

// map内部存储机制实际是以红黑树为基础，红黑树在插入节点时，必须依照大小比对之后在一个合适的位置上执行插入动作。所以作为关键字，
// 起码必须有“<”这个比较操作符。我们知道，int，float，enum，size_t等等简单关键字，都有内置的比较函数，与map搭配无论是插入还是查找，
// 都没什么问题。但是作为复杂数据类型，如果没有明确定义“<”比较操作符，就不能与map直接搭配使用，除非我们自己定义第三个参数。
// 在选择map的关键字时，注意以下两点，同时这两点也是改错的方法：
// a） 关键字明确定义“<”比较操作符
// b） 没有“<”比较操作符，自定义仿函数替代第三个参数Compare，该仿函数实现“()”操作符，提供比较功能。插入时各节点顺序以该仿函数为纲。

// R_TYPE *query;
// mySet *S;

struct myMap_m
{
	R_TYPE *q;
	mySet *S;
};

typedef struct myMap_m myMap;


myMap *myMap_new();
void myMap_insert(mySet *aSet, int aElt);
void myMap_free(myMap **aSet);



#endif
