#ifndef  SYNTAXSTRUCT_H_
#define  SYNTAXSTRUCT_H_
#include<iostream>
#include"CompilerDef.h"

//dynamic string


class SmartString
{
private:
	_SmartString *m_SmartString;
	SmartString() :m_SmartString(NULL){}
	~SmartString() {
		if (m_SmartString != NULL) {
			if (m_SmartString->data != NULL) {
				delete[](m_SmartString->data);
				m_SmartString->data = NULL;
				m_SmartString->capacity = 0;
				m_SmartString->size = 0;
			}
		}
		delete m_SmartString;
		m_SmartString = NULL;
	}

public:

	int init(int initsize) {
		m_SmartString = new _SmartString();
		if (m_SmartString != NULL){
			m_SmartString->data = new char[initsize];
			m_SmartString->size = 0;
			m_SmartString->capacity = initsize;
		}
		return 1;
	}

	int free(){
		if (m_SmartString != NULL){
			if (m_SmartString->data != NULL) {
				delete[](m_SmartString->data);
				m_SmartString->data = NULL;
				m_SmartString->capacity = 0;
				m_SmartString->size = 0;
			}
			return 1;
		}
	}

	int reset(int resetsize = 8){
		if (m_SmartString != NULL) {
			if (m_SmartString->data != NULL) {
				delete[](m_SmartString->data);
				m_SmartString->data = NULL;
				m_SmartString->capacity = 0;
				m_SmartString->size = 0;
			}
			if (m_SmartString->data == NULL){
			m_SmartString->data = new char[resetsize];
			m_SmartString->size = 0;
			m_SmartString->capacity = resetsize;
			}
		}
		return 1;
	}

	int reinit(int newsize) {
		if (m_SmartString->data == NULL)
			return -1;
		int capacity;
		char *data = new char[newsize];
		if (data == NULL)
			return -1;
		capacity = m_SmartString->capacity;

		while (capacity < newsize) capacity *= 2;

		memcpy(data, m_SmartString->data, m_SmartString->size);
		delete[]m_SmartString->data;
		m_SmartString->data = data;
		m_SmartString->capacity = capacity;
		return 0;
	}

	int append(char ch){
		int count;
		count = m_SmartString->size + 1;
		if (count > m_SmartString->capacity)
			reinit(count);
		(m_SmartString->data)[count - 1] = ch;
		m_SmartString->size = count;
		return 0;
	}
};

class SmartArray {
private:
	_SmartArray *m_SmartArray;
	int count;
public:
	SmartArray() :m_SmartArray(NULL), count(0){}
	~SmartArray() {
		if (m_SmartArray){
			if (m_SmartArray->data) {
				void **p;
				for (p = m_SmartArray->data; m_SmartArray->size > 0; ++p, --m_SmartArray->size)
					if (*p)
						::free(*p);
				::free(m_SmartArray->data);
				m_SmartArray->data = NULL;
			}
			delete m_SmartArray;
			m_SmartArray = NULL;
		}
	}
public:
	//m_SmartArray->data[initnum][totalsize / initnum]
	int init(int initsize) {
		
		if (m_SmartArray == NULL) {
			m_SmartArray = new _SmartArray();
			if (m_SmartArray == NULL)
				return -1;
			m_SmartArray->data = (void**)malloc(sizeof(char)* initsize);
		
			m_SmartArray->capacity = initsize;
			m_SmartArray->size = 0;
		}
		return 0;
	}
	
	int reinit(int newsize) {
		int capacity;
		void **data;
		capacity = m_SmartArray->capacity;
		while (newsize > capacity) capacity *= 2;
		data = (void**)realloc(m_SmartArray->data,capacity);
		if (!data)
			return -1;
		m_SmartArray->capacity = capacity;
		m_SmartArray->data = data;
	}

	int add(void *data) {
		
		count = m_SmartArray->size + 1;
		if (count * sizeof(void*) > m_SmartArray->capacity)
			reinit(count * sizeof(void*));
		m_SmartArray->data[count - 1] = data;
		m_SmartArray->size = count;
		return 0;
	}

	int research(char ch) {
		int ret;
		char **p;
		p = (char**)m_SmartArray->data;
		for (int i = 0; i < m_SmartArray->size; ++i, ++p)
			if (ch == **p)
				return i;
		return -1;
	}

	int free(){
		if (m_SmartArray->data) {
			void **p;
			for (p = m_SmartArray->data; m_SmartArray->size > 0; ++p, --m_SmartArray->size)
				if (*p)
					::free(*p);
			::free(m_SmartArray->data);
			m_SmartArray->data = NULL;
			return 0;
		}
	}
	int get_count() {
		return count;
	}
};

class TkTable{
public:
	TkTable() :m_Word(NULL),tmpTkWord(NULL) {
		m_tkTable.init(8);
		init_lex();
	}
	~TkTable(){
		if (tmpTkWord != NULL) {
			free(tmpTkWord);
		}
	}
	//init
	int init() {
		if (m_Word == NULL) {
			m_Word = new _TkWord*[TABLEMAX];
			for(int i = 0;i<TABLEMAX;i++)
				ZERO_MEMORY(m_Word[i]);
		}
		m_tkTable.init(10);
		return 0;
	}
	//direct insert key
	_TkWord* direct_insert(_TkWord * tw) {
		int keynu;
		m_tkTable.add(tw);
		keynu = elf_hash(tw->p_word);
		tw->next = m_Word[keynu];
		m_Word[keynu] = tw;
		return tw;
	}
	//find
	_TkWord* find(char *key) {
		int keynu;
		keynu = elf_hash(key);
		_TkWord *tp = NULL, *tp1;
		for (tp1 = m_Word[keynu]; tp1;tp1->next) {
			if (!strcmp(key, tp1->p_word)) {
				//token = tp1->tkcode;
				tp = tp1;
			}
		}
		return tp;
	}
	//direct
	_TkWord* insert(char* key) {
		
		int keynu;
		char *s;
		char *end;
		int length;
		if (tmpTkWord != NULL) {
			free(tmpTkWord);
			tmpTkWord = NULL;
		}
		keynu = elf_hash(key);
		tmpTkWord = find(key);
		//没有，插入
		if (tmpTkWord == NULL) {
			length = strlen(key);
			tmpTkWord = new _TkWord[sizeof(_TkWord) + length +1];
			tmpTkWord->next = m_Word[keynu];
			m_Word[keynu] = tmpTkWord;
			m_tkTable.add(tmpTkWord);
			tmpTkWord->tkcode = m_tkTable.get_count() - 1;
			s = (char*)tmpTkWord + sizeof(_TkWord);
			tmpTkWord->p_word = s;
			memcpy(s,key,length);
			s[length] = char('\0');
		}
		return tmpTkWord;
	}


private:
	_TkWord **m_Word;
	SmartArray m_tkTable;
	_TkWord *tmpTkWord;//中间使用的用于返回到指针
private:
	int elf_hash(char *key){
		int hash = 0;
		int x = 0;
		while (*key){
			hash = (hash << 4) + (*key++);//hash左移4位，当前字符ASCII存入hash  
			if ((x = hash & 0xf0000000) != 0){
				//如果最高的四位不为0，则说明字符多于7个，如果不处理，再加第九个字符时，第一个字符会被移出，因此要有如下处理。  
				//该处理，如果最高位为0，就会仅仅影响5-8位，否则会影响5-31位，因为C语言使用的算数移位  
				//因为1-4位刚刚存储了新加入到字符，所以不能>>28 
				hash ^= (x >> 24);//异或
				//清空28-31位。  
				hash &= ~x;
			}
		}
		return (hash & 0x7FFFFFFF);
	}

//单词表初始化
	void init_lex() {
		_TkWord *tmp;
		static _TkWord keywords[] = {
			{ TK_PLUS,		NULL	,"+",			NULL,	NULL },
			{ TK_MINUS,		NULL	,"-",			NULL,	NULL },
			{ TK_STAR,		NULL	,"*",			NULL,	NULL },
			{ TK_DIVIDE,	NULL	,"/",			NULL,	NULL },
			{ TK_MOD,		NULL	,"%",			NULL,	NULL },
			{ TK_EQ,		NULL	,"==",			NULL,	NULL },
			{ TK_NEQ,		NULL	,"!=",			NULL,	NULL },
			{ TK_LT,		NULL	,"<",			NULL,	NULL },
			{ TK_LEQ,		NULL	,"<=",			NULL,	NULL },
			{ TK_GT,		NULL	,">",			NULL,	NULL },
			{ TK_GEQ,		NULL	,">=",			NULL,	NULL },
			{ TK_ASSIGN,	NULL	,"=",			NULL,	NULL },
			{ TK_EPOINTSTO,	NULL	,"->",			NULL,	NULL },
			{ TK_DOT,		NULL	,".",			NULL,	NULL },
			{ TK_AND,		NULL	,"&",			NULL,	NULL },
			{ TK_OPENPA,	NULL	,"(",			NULL,	NULL },
			{ TK_CLOSEPA,	NULL	,")",			NULL,	NULL },
			{ TK_OPENBR,	NULL	,"[",			NULL,	NULL },
			{ TK_CLOSEBR,	NULL	,"]",			NULL,	NULL },
			{ TK_BEGIN,		NULL	,"{",			NULL,	NULL },
			{ TK_END,		NULL	,"}",			NULL,	NULL },
			{ TK_SEMICOLON,	NULL	,";",			NULL,	NULL },
			{ TK_COMMA,		NULL	,",",			NULL,	NULL },
			{ TK_ELLIPSIS,	NULL	,"...",			NULL,	NULL },
			{ TK_EOF,		NULL	,"End_Of_File",	NULL,	NULL },
			{ TK_CINT,		NULL	,"整形常量",	NULL,	NULL },
			{ TK_CCHAR,		NULL	,"字符常量",	NULL,	NULL },
			{ TK_CSTR,		NULL	,"字符串常量",	NULL,	NULL },
			{ KW_CHAR,		NULL	,"char",		NULL,	NULL },
			{ KW_SHORT,		NULL	,"short",		NULL,	NULL },
			{ KW_INT,		NULL	,"int",			NULL,	NULL },
			{ KW_VOID,		NULL	,"void",		NULL,	NULL },
			{ KW_STRUCT,	NULL	,"struct",		NULL,	NULL },
			{ KW_IF,		NULL	,"if",			NULL,	NULL },
			{ KW_ELSE,		NULL	,"else",		NULL,	NULL },
			{ KW_FOR,		NULL	,"for",			NULL,	NULL },
			{ KW_CONTINUE,	NULL	,"continue",	NULL,	NULL },
			{ KW_BREAK,		NULL	,"break",		NULL,	NULL },
			{ KW_RETURN,	NULL	,"return",		NULL,	NULL },
			{ KW_SIZEOF,	NULL	,"sizeof",		NULL,	NULL },
			{ KW_ALIGN,		NULL	,"__align",		NULL,	NULL },
			{ KW_CDECL,		NULL	,"__cdecl",		NULL,	NULL },
			{ KW_STDCALL,	NULL	,"__stdcall",	NULL,	NULL },
			{ 0,			NULL	,NULL,			NULL,	NULL },
		};
		for (tmp = &keywords[0]; tmp->p_word != NULL; tmp++ ) {
			direct_insert(tmp);
		}
	}


};

#endif // ! SYNTAXSTRUCT_H_