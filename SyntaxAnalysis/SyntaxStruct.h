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
	SmartArray() :m_SmartArray(NULL) {}
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
		int count;
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
};

class TkTable{
public:
	TkTable(){
		ZERO_MEMORY(m_TkTable);
		ZERO_MEMORY(m_SmartArray);

	}
	~TkTable(){
	}
private:
	_TkTable m_TkTable;
	_SmartArray m_SmartArray;
private:
	int elf_hash(char *key){
		unsigned int hash = 0;
		unsigned int x = 0;
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



};

#endif // ! SYNTAXSTRUCT_H_