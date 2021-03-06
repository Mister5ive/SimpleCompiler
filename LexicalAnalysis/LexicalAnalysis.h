#ifndef LEXICALANALYSIS_H
#define LEXICALANALYSIS_H
#include "stdafx.h"
#include "LexicalAnalysisAPI.h"
#include "../include/CompilerDef.h"

/*
*Log
*/

void LogCompile(const char *fmt, ...);
void LogLink(const char *fmt, ...);
void LogWarning(const char *fmt, ...);
void LogError(const char *fmt, ...);
void LogMessage(const char* fmt, ...);
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
	int realloc(int newsize);
	int append(T ch);
	void print();
	int size();
	int capacity();
	T*  str();
};

/*
*动态数组
*/
template<typename T>
class SmartArray {
public:
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
	T* str(int i);
	void print();
};

/*
*单词表
*/

class TkTable {
public:
	_TkWord **m_Word;//hash map
	SmartArray<_TkWord> m_tkTable;//dynamic array
public:
	TkTable();
	~TkTable();
	//init
	int init();
	//uninit
	int free();
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
	int array_size();
	char* array_str(int token);
private:
	int elf_hash(char *key);
};


//词法分析类
class LexicalAnalysisImp:public LexicalAnalysis {
public:
	void getch();
	bool is_digit(char c);
	bool is_identifier(char c);
	void preprocess();
	void skip_white_space();
	void parse_comment();
	void parse_identifier();
	void parse_num();
	void parse_string(char str);
	void process();
	void token_colored(int lex_state);
	char* get_tkstr(int token);
	void skip(int c);
public:
	LexicalAnalysisImp();
	~LexicalAnalysisImp();
	int init();
	int open(char* file_name);
	int close();
	void run();
public:
	SmartString<char> *m_TkString;//单词字符串
	SmartString<char> *m_SourceString;//单词源码字符串
	TkTable *m_TkHashTable;//单词哈希表
	FILE *m_file;//源码文件
	char ch;//当前从文件中get的到字符
	int token;//单词编码
	int line_num;//行号
	int tkvalue;//单词值
};
#endif

