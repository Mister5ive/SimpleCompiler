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
*��̬�ַ���
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
	T*  str();
};

/*
*��̬����
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
*���ʱ�
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
	//���ʱ��ʼ��
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


//�ʷ�������
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
	SmartString<char> *m_TkString;//�����ַ���
	SmartString<char> *m_SourceString;//����Դ���ַ���
	TkTable *m_TkHashTable;//���ʹ�ϣ��
	FILE *m_file;//Դ���ļ�
	char ch;//��ǰ���ļ���get�ĵ��ַ�
	int token;//���ʱ���
	int line_num;//�к�
	int tkvalue;//����ֵ
};
#endif

