#ifndef  SYNTAXSTRUCT_H_
#define  SYNTAXSTRUCT_H_
#include<iostream>
#include"CompilerDef.h"

//dynamic string

template<typename T>
class SmartString
{
private:
	_SmartString<T> *m_SmartString;
	

public:
	SmartString() :m_SmartString(NULL) {}
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
	int init(int initsize) {
		m_SmartString = new _SmartString<T>();
		if (m_SmartString != NULL){
			m_SmartString->data = new T[initsize];
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
			m_SmartString->data = new T[resetsize];
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
		char *data = new T[newsize];
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

	int append(T ch){
		int count;
		count = m_SmartString->size + 1;
		if (count > m_SmartString->capacity)
			reinit(count);
		(m_SmartString->data)[count - 1] = ch;
		m_SmartString->size = count;
		return 0;
	}
	void print() {
		if (m_SmartString != NULL){
			for (int i = 0; i < m_SmartString->size; i++)
				::printf("%c", m_SmartString->data[i]);
		}
	}

	int size() {
		if (m_SmartString != NULL) {
			return m_SmartString->size;
		}
		return -1;
	}
	int capacity() {
		if (m_SmartString != NULL) {
			return m_SmartString->capacity;
		}
		return -1;
	}
};

template<typename T>
class SmartArray {
private:
	_SmartArray<T> *m_SmartArray;
	int count;
public:
	SmartArray() :m_SmartArray(NULL), count(0){}
	~SmartArray() {
		if (m_SmartArray){
			if (m_SmartArray->data) {
			/*	T **p;
				for (p = m_SmartArray->data; m_SmartArray->size > 0; ++p, --m_SmartArray->size)
					if (*p)
						::free(*p);*/
				::free(m_SmartArray->data);
				m_SmartArray->data = NULL;
			}
			delete m_SmartArray;
			m_SmartArray = NULL;
		}
	}
public:
	int init(int initsize) {
		
		if (m_SmartArray == NULL) {
			m_SmartArray = new _SmartArray<T>();
			if (m_SmartArray == NULL)
				return -1;
			m_SmartArray->data = (T**)malloc(sizeof(T*)* initsize);
		
			m_SmartArray->capacity = initsize;
			m_SmartArray->size = 0;
		}
		return 0;
	}
	
	int reinit(int newsize) {
		int capacity;
		T **data;
		capacity = m_SmartArray->capacity;
		while (newsize > capacity) capacity *= 2;
		data = (T**)realloc(m_SmartArray->data,capacity);
		if (!data)
			return -1;
		m_SmartArray->capacity = capacity;
		m_SmartArray->data = data;
	}

	int add(T *data) {
		if (data == NULL)
			return -1;
		count = m_SmartArray->size + 1;
		if (count * sizeof(void*) > m_SmartArray->capacity)
			reinit(count * sizeof(void*));
		m_SmartArray->data[count - 1] = data;
		m_SmartArray->size = count;
		return 0;
	}

	int research(T ch) {	
		int ret;
		T **p;
		p = (T**)m_SmartArray->data;
		for (int i = 0; i < m_SmartArray->size; ++i, ++p)
			if (ch == **p)
				return i;
		return -1;
	}
	int search(T *str) {
		int ret;
		T **p;
		p = (T**)m_SmartArray->data;
		for (int i = 0; i < m_SmartArray->size; ++i, ++p)
			if (strcmp(str,*p) == 0)
				return i;
		return -1;
	}
	int free(){
		if (m_SmartArray->data) {
			//void **p;
			/*for (p = m_SmartArray->data; m_SmartArray->size; ++p, --m_SmartArray->size)
				if (*p)
					::free(*p);*/
			::free(m_SmartArray->data);
			m_SmartArray->data = NULL;
			m_SmartArray->size = 0;
			m_SmartArray->capacity = 0;
			count = 0;
			return 0;
		}
	}
	int get_count() {
		return count;
	}
	int size() {
		if(m_SmartArray)
			return m_SmartArray->size;
		return -1;

	}
	int capacity() {
		if (m_SmartArray)
			return m_SmartArray->capacity;
		return -1;
	}
	void print() {
		if (m_SmartArray) {
			for(int i = 0;i < m_SmartArray->size;i++)
				printf("%s\n", m_SmartArray->data[i]);
		}
		
	}
};


class TkTable{
public:
	TkTable() :m_Word(NULL){
		m_tkTable.init(256);
		
	}
	~TkTable(){
		
		if (m_Word != NULL) {
			free(m_Word);
			m_Word = NULL;
		}
	}

	//init
	int init() {
		if (m_Word == NULL) {
			m_Word = new _TkWord*[TABLEMAX];
			for (int i = 0; i < TABLEMAX; i++) {
				ZERO_MEMORY(m_Word[i]);
			}
				
		}
		return 0;
	}

	//单词表初始化
	void init_lex() {
		_TkWord *tmp;// = new _TkWord;
		//tmp->next = new _TkWord;
		for (tmp = &keywords[0]; tmp->p_word != NULL; tmp++) {
			direct_insert(tmp);
		}
	}
	//direct insert key
	_TkWord* direct_insert(_TkWord *&tw) {
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
			if (strcmp(key, tp1->p_word) == 0) {
				//token = tp1->tkcode;
				tp = tp1;
				break;
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
		_TkWord *tmpTkWord = NULL;
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
	//test
	void print() {
		if (m_Word != NULL) {
			for (int i = 0; i < TABLEMAX; i++) {
				_TkWord *tmp = m_Word[i];
				while ( tmp != NULL) {
					printf("key:%d, str:%s , token:%d\n",i,tmp->p_word,tmp->tkcode);
					tmp = tmp->next;
					if (tmp != NULL)
						printf("-----conflict\n");
				}
			}
		}
	}

private:
	_TkWord **m_Word;//hash map
	SmartArray<_TkWord> m_tkTable;//dynamic array
	
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
		return (hash % TABLEMAX);
	}




};

#endif // ! SYNTAXSTRUCT_H_