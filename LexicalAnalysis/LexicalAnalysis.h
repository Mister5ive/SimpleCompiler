#ifndef LEXICALANALYSIS_H
#define LEXICALANALYSIS_H
#include "LexicalAnalysisAPI.h"
#include "../include/CompilerDef.h"

/*
*动态字符串
*/
template<typename T>
class SmartString
{
private:
	_SmartString<T> *m_SmartString;


public:
	SmartString();
	~SmartString();
	int init(int initsize);

	int free();

	int reset(int resetsize = 8);
	int reinit(int newsize);

	int append(T ch);
	void print();

	int size();
	int capacity();
};

/*
*动态数组
*/
template<typename T>
class SmartArray {
private:
	_SmartArray<T> *m_SmartArray;
	int count;
public:
	SmartArray();
	~SmartArray();
	int init(int initsize);
	int reinit(int newsize);
	int add(T *data);
	int research(T ch);
	int search(T *str);
	int free();
	int get_count();
	int size();
	int capacity();
	void print();
};

/*
*单词表
*/

class TkTable {

private:
	_TkWord **m_Word;//hash map
	SmartArray<_TkWord> m_tkTable;//dynamic array
public:
	TkTable();
	~TkTable();
	//init
	int init();
	//单词表初始化
	void init_lex();
	//direct insert key
	_TkWord* direct_insert(_TkWord *&tw);
	//find
	_TkWord* find(char *key);
	//direct
	_TkWord* insert(char* key);
	//test
	void print();
private:
	int elf_hash(char *key);
#endif

