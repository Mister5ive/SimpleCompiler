#ifndef  SYNTAXSTRUCT_H_
#define  SYNTAXSTRUCT_H_
#include<iostream>
#include"CompilerDef.h"

//dynamic string


class SmartString
{
private:
	m_SmartString *p_SmartString;
	SmartString() :p_SmartString(NULL){}
	~SmartString() {
		if (p_SmartString != NULL) {
			if (p_SmartString->data != NULL) {
				delete[](p_SmartString->data);
				p_SmartString->data = NULL;
				p_SmartString->capacity = 0;
				p_SmartString->size = 0;
			}
		}
		delete p_SmartString;
		p_SmartString = NULL;
	}

public:

	int init(int initsize) {
		p_SmartString = new m_SmartString();
		if (p_SmartString != NULL){
			p_SmartString->data = new char[initsize];
			p_SmartString->size = 0;
			p_SmartString->capacity = initsize;
		}
		return 1;
	}

	int free(){
		if (p_SmartString != NULL){
			if (p_SmartString->data != NULL) {
				delete[](p_SmartString->data);
				p_SmartString->data = NULL;
				p_SmartString->capacity = 0;
				p_SmartString->size = 0;
			}
			return 1;
		}
	}

	int reset(int resetsize = 8){
		if (p_SmartString != NULL) {
			if (p_SmartString->data != NULL) {
				delete[](p_SmartString->data);
				p_SmartString->data = NULL;
				p_SmartString->capacity = 0;
				p_SmartString->size = 0;
			}
			if (p_SmartString->data == NULL){
			p_SmartString->data = new char[resetsize];
			p_SmartString->size = 0;
			p_SmartString->capacity = resetsize;
			}
		}
		return 1;
	}

	int reinit(int newsize) {
		if (p_SmartString->data == NULL)
			return -1;
		int capacity;
		char *data = new char[newsize];
		if (data == NULL)
			return -1;
		capacity = p_SmartString->capacity;

		while (capacity < newsize) capacity *= 2;

		memcpy(data, p_SmartString->data, p_SmartString->size);
		delete[]p_SmartString->data;
		p_SmartString->data = data;
		p_SmartString->capacity = capacity;
		return 0;
	}

	int append(char ch){
		int count;
		count = p_SmartString->size + 1;
		if (count > p_SmartString->capacity)
			reinit(count);
		(p_SmartString->data)[count - 1] = ch;
		p_SmartString->size = count;
		return 0;
	}
};

class SmartArray {
private:
	m_SmartArray *p_SmartArray;
	SmartArray() :p_SmartArray(NULL) {}
	~SmartArray() {
		if (p_SmartArray){
			if (p_SmartArray->data) {
				void **p;
				for (p = p_SmartArray->data; p_SmartArray->size > 0; ++p, --p_SmartArray->size)
					if (*p)
						::free(*p);
				::free(p_SmartArray->data);
				p_SmartArray->data = NULL;
			}
			delete p_SmartArray;
			p_SmartArray = NULL;
		}
	}
public:
	//p_SmartArray->data[initnum][totalsize / initnum]
	int init(int initsize) {
		
		if (p_SmartArray == NULL) {
			p_SmartArray = new m_SmartArray();
			if (p_SmartArray == NULL)
				return -1;
			p_SmartArray->data = (void**)malloc(sizeof(char)* initsize);
		
			p_SmartArray->capacity = initsize;
			p_SmartArray->size = 0;
		}
		return 0;
	}
	
	int reinit(int newsize) {
		int capacity;
		void **data;
		capacity = p_SmartArray->capacity;
		while (newsize > capacity) capacity *= 2;
		data = (void**)realloc(p_SmartArray->data,capacity);
		if (!data)
			return -1;
		p_SmartArray->capacity = capacity;
		p_SmartArray->data = data;
	}

	int add(void *data) {
		int count;
		count = p_SmartArray->size + 1;
		if (count * sizeof(void*) > p_SmartArray->capacity)
			reinit(count * sizeof(void*));
		p_SmartArray->data[count - 1] = data;
		p_SmartArray->size = count;
		return 0;
	}

	int research(char ch) {
		int ret;
		char **p;
		p = (char**)p_SmartArray->data;
		for (int i = 0; i < p_SmartArray->size; ++i, ++p)
			if (ch == **p)
				return i;
		return -1;
	}

	int free(){
		if (p_SmartArray->data) {
			void **p;
			for (p = p_SmartArray->data; p_SmartArray->size > 0; ++p, --p_SmartArray->size)
				if (*p)
					::free(*p);
			::free(p_SmartArray->data);
			p_SmartArray->data = NULL;
			return 0;
		}
	}
};

#endif // ! SYNTAXSTRUCT_H_