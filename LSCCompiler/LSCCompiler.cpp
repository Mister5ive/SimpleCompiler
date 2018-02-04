#include "stdafx.h"
#include "LSCCompiler.h"

#define SYNTAXANALYSIS 1

static LogFunction	g_LogMsg = NULL;
static LogFunction	g_LogError = NULL;
static LogFunction	g_LogWarning = NULL;
static LogFunction	g_LogLink = NULL;
static LogFunction	g_LogCompile = NULL;

LSCCompilerBase*  LSCCompilerCreate(void) {
	LSCCompilerImp* pInstance = new LSCCompilerImp();
	if (pInstance)
		return pInstance;
}
void  LSCCompilerDestroy(LSCCompilerBase* pInstance) {
	if (pInstance) {
		delete pInstance;
		pInstance = NULL;
	}

}

void SetLogFunction(LogFunction Logmsg, LogFunction Logerr, LogFunction LogWarnig, LogFunction LogLink, LogFunction LogCompile)
{
	g_LogMsg = Logmsg;
	g_LogError = Logerr;
	g_LogWarning = LogWarnig;
	g_LogLink = LogLink;
	g_LogCompile = LogCompile;
}

/***********************************************************
* func:		计算哈希地址
* key:		哈希关键字
* MAXKEY:	哈希表长度
**********************************************************/
int elf_hash(char *key)
{
	int h = 0, g;
	while (*key)
	{
		h = (h << 4) + *key++;
		g = h & 0xf0000000;
		if (g)
			h ^= g >> 24;
		h &= ~g;
	}
	return h % MAXKEY;
}

//动态字符串
/*************************************************************************************************************/
template<typename T>
SmartString<T>::SmartString() :m_SmartString(NULL) {}

template<typename T>
SmartString<T>::~SmartString() {
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
template<typename T>
int SmartString<T>::init(int initsize) {
	m_SmartString = new _SmartString<T>();
	if (m_SmartString != NULL) {
		//m_SmartString->data = new T[initsize];
		m_SmartString->data = (T*)malloc(sizeof(T)* initsize);
		m_SmartString->size = 0;
		m_SmartString->capacity = initsize;
	}
	return 0;
}

template<typename T>
int SmartString<T>::free() {
	if (m_SmartString != NULL) {
		if (m_SmartString->data != NULL) {
			//delete[](m_SmartString->data);//bug
			::free(m_SmartString->data);//bug
			m_SmartString->data = NULL;
			m_SmartString->capacity = 0;
			m_SmartString->size = 0;
		}
		return 0;
	}
}

template<typename T>
int SmartString<T>::realloc(int resetsize) {
	//if (m_SmartString != NULL) {
	//	if (m_SmartString->data != NULL) {
	//		//delete[](m_SmartString->data);
	//		::free(m_SmartString->data);
	//		m_SmartString->data = NULL;
	//		m_SmartString->capacity = 0;
	//		m_SmartString->size = 0;
	//	}
	//	if (m_SmartString->data == NULL) {
	//		//m_SmartString->data = new T[resetsize];
	//		m_SmartString->size = 0;
	//		m_SmartString->capacity = resetsize;
	//	}
	//}
	int capacity;
	T *data;
	capacity = m_SmartString->capacity;
	while (resetsize > capacity) capacity *= 2;
	data = (T*)::realloc(m_SmartString->data, capacity);
	if (!data)
		return -1;
	m_SmartString->capacity = capacity;
	m_SmartString->data = data;
	return 0;
}

template<typename T>
int SmartString<T>::reset(int newsize) {
	/*if (m_SmartString->data == NULL)
	return -1;
	int capacity;
	T *data = (T*)malloc(sizeof(T) * newsize);
	if (data == NULL)
	return -1;
	capacity = m_SmartString->capacity;

	while (capacity < newsize) capacity *= 2;

	memcpy(data, m_SmartString->data, m_SmartString->size);
	::free(m_SmartString->data);
	m_SmartString->data = data;
	m_SmartString->capacity = capacity;*/
	if (m_SmartString != NULL) {
		if (m_SmartString->data != NULL) {
			delete[](m_SmartString->data);
			m_SmartString->data = NULL;
			m_SmartString->capacity = 0;
			m_SmartString->size = 0;
		}

		m_SmartString->data = (T*)malloc(sizeof(T)* newsize);
		m_SmartString->size = 0;
		m_SmartString->capacity = newsize;
	}
	return 0;
}

template<typename T>
int SmartString<T>::append(T ch) {
	int count;
	count = m_SmartString->size + 1;
	if (count > m_SmartString->capacity)
		realloc(count);
	(m_SmartString->data)[count - 1] = ch;
	m_SmartString->size = count;
	return 0;
}
template<typename T>
void SmartString<T>::print() {
	if (m_SmartString != NULL) {
		for (int i = 0; i < m_SmartString->size; i++)
			//::printf("%c", m_SmartString->data[i]);
			LogMessage("SmartString.data[%d],%c", i, m_SmartString->data[i]);
	}
}

template<typename T>
int SmartString<T>::size() {
	if (m_SmartString != NULL) {
		return m_SmartString->size;
	}
	return -1;
}
template<typename T>
int SmartString<T>::capacity() {
	if (m_SmartString != NULL) {
		return m_SmartString->capacity;
	}
	return -1;
}
template<typename T>
T*  SmartString<T>::str() {
	return m_SmartString->data;
}
//动态数组
/*************************************************************************************************************/
template<typename T>
SmartArray<T>::SmartArray() :m_SmartArray(NULL), count(0) {}

template<typename T>
SmartArray<T>:: ~SmartArray() {
	if (m_SmartArray) {
		if (m_SmartArray->data) {
			::free(m_SmartArray->data);
			m_SmartArray->data = NULL;
		}
		delete m_SmartArray;
		m_SmartArray = NULL;
	}
}
template<typename T>
int SmartArray<T>::init(int initsize) {

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

template<typename T>
int SmartArray<T>::reinit(int newsize) {
	int capacity;
	T **data;
	capacity = m_SmartArray->capacity;
	while (newsize > capacity) capacity *= 2;
	data = (T**)realloc(m_SmartArray->data, capacity);
	if (!data)
		return -1;
	m_SmartArray->capacity = capacity;
	m_SmartArray->data = data;
}

template<typename T>
int SmartArray<T>::add(T *data) {
	if (data == NULL)
		return -1;
	count = m_SmartArray->size + 1;
	if (count * sizeof(void*) > m_SmartArray->capacity)
		reinit(count * sizeof(void*));
	m_SmartArray->data[count - 1] = data;
	m_SmartArray->size = count;
	return 0;
}

template<typename T>
int SmartArray<T>::research(T ch) {
	int ret;
	T **p;
	p = (T**)m_SmartArray->data;
	for (int i = 0; i < m_SmartArray->size; ++i, ++p)
		if (ch == **p)
			return i;
	return -1;
}
template<typename T>
int SmartArray<T>::search(T *str) {
	int ret;
	T **p;
	p = (T**)m_SmartArray->data;
	for (int i = 0; i < m_SmartArray->size; ++i, ++p)
		if (strcmp(str, *p) == 0)
			return i;
	return -1;
}
template<typename T>
int SmartArray<T>::free() {
	if (m_SmartArray->data) {
		::free(m_SmartArray->data);
		m_SmartArray->data = NULL;
		m_SmartArray->size = 0;
		m_SmartArray->capacity = 0;
		count = 0;
		return 0;
	}
}
template<typename T>
int SmartArray<T>::get_count() {
	return count;
}
template<typename T>
int SmartArray<T>::size() {
	if (m_SmartArray)
		return m_SmartArray->size;
	return -1;

}
template<typename T>
int SmartArray<T>::capacity() {
	if (m_SmartArray)
		return m_SmartArray->capacity;
	return -1;
}
template<typename T>
T* SmartArray<T>::str(int i) {
	return m_SmartArray->data[i];
}
template<typename T>
void SmartArray<T>::print() {
	if (m_SmartArray) {
		for (int i = 0; i < m_SmartArray->size; i++)
			//printf("%s\n", m_SmartArray->data[i]);
			LogMessage("SmartArray.data[%d]:%s", i, m_SmartArray->size);
	}
}

//单词表
/*************************************************************************************************************/
TkTable::TkTable() :m_Word(NULL) {
	m_tkTable.init(256);
}
TkTable::~TkTable() {
	if (m_Word != NULL) {
		::free(m_Word);
		m_Word = NULL;
	}
}

//init
int TkTable::init() {
	if (m_Word == NULL) {
		m_Word = new _TkWord*[TABLEMAX];
		for (int i = 0; i < TABLEMAX; i++) {
			ZERO_MEMORY(m_Word[i]);
		}
	}
	return 0;
}
//uninit
int TkTable::free() {
	if (m_Word != NULL) {
		::free(m_Word);
		m_Word = NULL;
	}
	int ret = -1;
	ret = m_tkTable.free();

	return ret;
}
//单词表初始化
void TkTable::init_lex() {
	_TkWord *tmp;// = new _TkWord;
				 //tmp->next = new _TkWord;
	m_tkTable.init(256);
	for (tmp = &keywords[0]; tmp->p_word != NULL; tmp++) {
		direct_insert(tmp);
	}
}
//direct insert key
_TkWord* TkTable::direct_insert(_TkWord *&tw) {
	int keynu;
	tw->sym_identifier = NULL;
	tw->sym_struct = NULL;
	m_tkTable.add(tw);

	keynu = elf_hash(tw->p_word);
	tw->next = m_Word[keynu];
	m_Word[keynu] = tw;
	return tw;
}
//find
_TkWord* TkTable::find(char *key) {
	int keynu;
	keynu = elf_hash(key);
	_TkWord *tp = NULL, *tp1;
	for (tp1 = m_Word[keynu]; tp1; tp1 = tp1->next) {
		if (strcmp(key, tp1->p_word) == 0) {
			//token = tp1->tkcode;
			tp = tp1;
			//break;
		}
	}
	return tp;
}
//direct
_TkWord* TkTable::insert(char* key) {

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
		tmpTkWord = new _TkWord[sizeof(_TkWord) + length + 1];
		tmpTkWord->next = m_Word[keynu];
		m_Word[keynu] = tmpTkWord;
		m_tkTable.add(tmpTkWord);
		tmpTkWord->tkcode = m_tkTable.get_count() - 1;
		s = (char*)tmpTkWord + sizeof(_TkWord);
		tmpTkWord->p_word = s;
		memcpy(s, key, length);
		s[length] = char('\0');
		tmpTkWord->sym_identifier = NULL;
		tmpTkWord->sym_struct = NULL;
	}
	return tmpTkWord;
}
//test
void TkTable::print() {
	if (m_Word != NULL) {
		for (int i = 0; i < TABLEMAX; i++) {
			_TkWord *tmp = m_Word[i];
			while (tmp != NULL) {
				//printf("TkTable key:%d, str:%s , token:%d\n", i, tmp->p_word, tmp->tkcode);
				LogMessage("key:%d, str:%s , token:%d\n", i, tmp->p_word, tmp->tkcode);
				tmp = tmp->next;
				if (tmp != NULL)
					//printf("-----conflict\n");
					LogMessage("conflict");
			}
		}
	}
}

int TkTable::array_size() {
	return m_tkTable.size();
}

char* TkTable::array_str(int token) {
	return m_tkTable.str(token)->p_word;
}

int TkTable::elf_hash(char *key) {
	int hash = 0;
	int x = 0;
	while (*key) {
		hash = (hash << 4) + (*key++);//hash左移4位，当前字符ASCII存入hash  
		if ((x = hash & 0xf0000000) != 0) {
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

/***********************************************************
* func:		计算字节对齐位置
* n:		未对齐前值
* align:    对齐粒度（几字节对齐）
* 原理：
对于2算法原理如下：
2字节对齐，要求地址位为2,4,6,8...，要求二进制位最后一位为0（2的1次方）
4字节对齐，要求地址位为4,8,12,16...，要求二进制位最后两位为0（2的2次方）
8字节对齐，要求地址位为8,16,24,32...，要求二进制位最后三位为0（2的3次方）
16字节对齐，要求地址位为16,32,48,64...，要求二进制位最后四位为0（2的4次方）
...
由此可见，我们只要对数据补齐对齐所需最少数据，然后将补齐位置0就可以实现对齐计算。
（1）(align-1)，表示对齐所需的对齐位，如：2字节对齐为1，4字节为11，8字节为111，16字节为1111...
（2）(n+(align-1))，表示n补齐对齐所需数据
（3）&~(align-1)，表示去除由于补齐造成的多余数据
（4） (n+(align-1))&~(align-1)，表示对齐后的数据

举个例子：如8字节对齐。起始地始是6
6 + （8 - 1）=0000 0110 + 0000 0111 = 0000 1101
0000 1101 & ~(0000 0111) = 0000 1000  //去除由于补齐造成的多余数据
**********************************************************/
inline unsigned int calc_align(unsigned int n, unsigned int align)
{
	return ((n + align - 1) & (~(align - 1)));
}


//词法分析类
/*************************************************************************************************************/
LSCCompilerImp::LSCCompilerImp() :token(-1), line_num(1), tkvalue(-1),
m_file(NULL), ch(EOF), syntax_state(SC_STATE_NULL), syntax_level(0)
, nsec_image(0)
{
	m_TkString = new SmartString<char>();
	
	m_SourceString = new SmartString<char>();
	m_TkHashTable = new TkTable();
}
LSCCompilerImp::~LSCCompilerImp() {
	release();
}
void LSCCompilerImp::release() {
	if (m_TkString) {
		m_TkString->free();
		delete m_TkString;
		m_TkString = NULL;
	}
	if (m_SourceString) {
		m_SourceString->free();//bug
		delete m_SourceString;
		m_SourceString = NULL;
	}
	if (m_TkHashTable) {
		m_TkHashTable->free();
		delete m_TkHashTable;
		m_TkHashTable = NULL;
	}
	//m_section release
	Section *sec;
	std::vector<Section*>::iterator iter = m_section.begin();
	for (; iter != m_section.end(); ++iter) {	
		sec = *iter;
		if (sec != NULL) {
			free(sec);
			sec = NULL;
		}			
	}
	m_section.clear();
}
void LSCCompilerImp::getch() {
	ch = getc(m_file);
}
bool LSCCompilerImp::is_digit(char c) {
	return c >= '0'&&c <= '9';
}
bool LSCCompilerImp::is_identifier(char c) {
	return (c == '_') ||
		(c >= 'A'&&c <= 'Z') ||
		(c >= 'a'&&c <= 'z');
}
void LSCCompilerImp::preprocess() {
	while (1) {
		if (ch == ' ' || ch == '\t' || ch == '\r')
			skip_white_space();
		else if (ch == '/') {
			//读下一个，看是否为/*
			getch();
			if (ch == '*')
				parse_comment();
			else {
				ungetc(ch, m_file);
				ch = '/';
				break;
			}
		}
		else
			break;
	}
}
void LSCCompilerImp::skip_white_space() {
	while (ch == ' ' || ch == '\t' || ch == '\r') {
		if (ch == '\r') {
			getch();
			if (ch != '\n')  return;
			line_num++;
		}
		printf("%c", ch);
		getch();
	}
}
void LSCCompilerImp::parse_comment() {
	getch();
	do {
		do {
			if (ch == '*' || ch == '\0' || ch == '\n')
				break;
			else
				getch();
		} while (1);
		if (ch == '*') {
			getch();
			if (ch == '/') {
				getch();
				return;
			}
		}
		else if (ch == '\n') {
			line_num++;
			getch();
		}
		else
		{
			//error
			return;
		}
	} while (1);
}
void LSCCompilerImp::parse_identifier() {
	m_TkString->reset();
	m_TkString->append(ch);
	getch();
	while (is_digit(ch) || is_identifier(ch)) {
		m_TkString->append(ch);
		getch();
	}
	m_TkString->append('\0');
}
void LSCCompilerImp::parse_num() {
	m_TkString->reset();
	m_SourceString->reset();
	do {
		m_TkString->append(ch);
		m_SourceString->append(ch);
		getch();
	} while (is_digit(ch));
	if (ch == '.')
	{
		do {
			m_TkString->append(ch);
			m_SourceString->append(ch);
			getch();
		} while (is_digit(ch));
	}
	m_TkString->append('\0');
	m_SourceString->append('\0');
	tkvalue = atoi(m_TkString->str());
}
void LSCCompilerImp::parse_string(char str) {
	char tmp;//存放转义字符
	m_TkString->reset();
	m_SourceString->reset();
	//	m_SourceString->append(str);
	getch();
	while (1) {
		if (ch == str)//字符串结束
			break;
		else if (ch == '\\') {//转义字符
			getch();
			switch (ch) {
			case '0':
				tmp = '\0';
				break;
			case 'a':
				tmp = '\a';
				break;
			case 'b':
				tmp = '\b';
				break;
			case 't':
				tmp = '\t';
				break;
			case 'n':
				tmp = '\n';
				break;
			case 'v':
				tmp = '\v';
				break;
			case 'f':
				tmp = '\f';
				break;
			case 'r':
				tmp = '\r';
				break;
			case '\"':
				tmp = '\"';
				break;
			case '\'':
				tmp = '\'';
				break;
			case '\\':
				tmp = '\\';
				break;
			default:
				tmp = ch;
				if (tmp >= '!' && tmp <= '~')
					//printf("%s,%s,%d Illegal Escape String:\'\\%c\'\n", tmp, __FILE__, __FUNCTION__, __LINE__);
					LogCompile("%s,%s,%d Illegal Escape String:\'\\%c\'", tmp, __FILE__, __FUNCTION__, __LINE__);
				else
					//printf("%s,%s,%d Illegal Escape String:\'\\0x%x\'\n", tmp, __FILE__, __FUNCTION__, __LINE__);
					LogCompile("%s,%s,%d Illegal Escape String:\'\\0x%x\'", tmp, __FILE__, __FUNCTION__, __LINE__);
				break;
			}

			//m_TkString->append(tmp);
			//m_SourceString->append(tmp);
			getch();
		}
		else {
			m_TkString->append(ch);
			m_SourceString->append(ch);
			getch();
		}

	}
	//m_TkString->append(str);

	m_TkString->append('\0');
	m_SourceString->append('\0');
	getch();
}
/***********************************************************
*  功能:	取单词
**********************************************************/
void LSCCompilerImp::get_token() {
	preprocess();
	switch (ch) {
	case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
	case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':
	case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':
	case 's':case 't':case 'u':case 'v':case 'w':case 'x':
	case 'y':case 'z':
	case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
	case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':
	case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':
	case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':
	case 'Y':case 'Z':
	case '_'://标识符
	{
		_TkWord *tp;
		parse_identifier();
		tp = m_TkHashTable->insert(m_TkString->str());//!!!!!!
		token = tp->tkcode;
		break;
	}
	case '0':case '1':case '2':case '3':case '4':case '5':
	case '6':case '7':case '8':case '9':
	{
		parse_num();
		token = TK_CINT;
		break;
	}
	case '+':
	{
		getch();
		token = TK_PLUS;
		break;
	}
	case '-':
	{
		getch();
		if (ch == '>') {
			token = TK_POINTSTO;
			getch();
		}
		else
			token = TK_MINUS;
		break;
	}
	case '/':
	{
		token = TK_DIVIDE;
		getch();
		break;
	}
	case '%':
	{
		token = TK_MOD;
		getch();
		break;
	}
	case '=':
	{
		getch();
		if (ch == '=') {
			token = TK_EQ;
			getch();
		}
		else
			token = TK_ASSIGN;
		break;
	}
	case '!':
	{
		getch();
		if (ch == '=') {
			token = TK_NEQ;
			getch();
		}
		else
			//error
			//printf("%s,%s,%d error\n", __FILE__, __FUNCTION__, __LINE__);
			LogCompile("%s,%s,%d Token not support ", __FILE__, __FUNCTION__, __LINE__);
		break;
	}
	case '>':
	{
		getch();
		if (ch == '=') {
			token = TK_GEQ;
			getch();
		}
		else
			token = TK_GT;
		break;
	}
	case '<':
	{
		getch();
		if (ch == '=') {
			token = TK_LEQ;
			getch();
		}
		else
			token = TK_LT;
		break;
	}
	case '.':
	{
		getch();
		if (ch == '.') {
			getch();
			if (ch != '.')
				//error
				//printf("%s,%s,%d error\n", __FILE__, __FUNCTION__, __LINE__);
				LogCompile("%s,%s,%d Token not support ", __FILE__, __FUNCTION__, __LINE__);
			else
				token = TK_ELLIPSIS;
			getch();

		}
		else
			token = TK_DOT;
		break;
	}
	case '&':
	{
		token = TK_AND;
		getch();
		break;
	}
	case ';':
	{
		token = TK_SEMICOLON;
		getch();
		break;
	}
	case ']':
	{
		token = TK_CLOSEBR;
		getch();
		break;
	}
	case '}':
	{
		token = TK_END;
		getch();
		break;
	}
	case ')':
	{
		token = TK_CLOSEPA;
		getch();
		break;
	}
	case '[':
	{
		token = TK_OPENBR;
		getch();
		break;
	}
	case '{':
	{
		token = TK_BEGIN;
		getch();
		break;
	}
	case '(':
	{
		token = TK_OPENPA;
		getch();
		break;
	}
	case ',':
	{
		token = TK_COMMA;
		getch();
		break;
	}
	case '*':
	{
		token = TK_STAR;
		getch();
		break;
	}
	case '\'':
	{
		//
		parse_string(ch);
		token = TK_CCHAR;
		break;
	}
	case '\"':
	{
		parse_string(ch);
		token = TK_CSTR;
		break;
	}
	case EOF:
	{
		token = TK_EOF;
		break;
	}
	default:
		//printf("%s,%s,%d error\n", __FILE__, __FUNCTION__, __LINE__);
		LogCompile("%s,%s,%d Token not support ", __FILE__, __FUNCTION__, __LINE__);
		getch();
		break;
	}
#ifdef SYNTAXANALYSIS
	//语法缩进
	syntax_indent();
#endif
}
void LSCCompilerImp::token_colored(int lex_state) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	char *pOut = NULL;
	switch (lex_state) {
	case LEX_NORMAL:
		if (token >= TK_IDENT)//标识符
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		else if (token >= KW_CHAR)//关键字
			SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN);
		else if (token >= TK_CINT)//常量
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN);
		else //运算符 分隔符
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		pOut = get_tkstr(token);
		if (token != TK_EOF)
		{
			if (token == TK_CSTR)
				printf("\"", pOut);
			if (token == TK_CCHAR)
				printf("\"", pOut);
			printf("%s", pOut);
			if (token == TK_CSTR)
				printf("\"", pOut);
			if (token == TK_CCHAR)
				printf("\"", pOut);
		}
		break;
	case LEX_LEP:
		printf("&c", ch);
		break;
	}

}
char* LSCCompilerImp::get_tkstr(int token) {
	if (token > m_TkHashTable->array_size())
		return NULL;
	else if (token >= TK_CINT && token <= TK_CSTR)
		return m_SourceString->str();
	else
		return m_TkHashTable->array_str(token);
}
void LSCCompilerImp::skip(int c) {
	if (token != c)
		printf("lack of %s\n", get_tkstr(c));
	get_token();
}
/***********************************************************
*func:			缩进n个tab键
*num:			缩进个数
**********************************************************/
void LSCCompilerImp::print_tab(int num) {
	for (int i = 0; i < num; ++i) {
		printf("\t");
	}
}
/***********************************************************
*func:			返回类型长度
*type:			数据类型指针
*align:			对齐值
*return:		类型长度
**********************************************************/
int  LSCCompilerImp::type_size(Type *type, int *align) {
	Symbol *sym;
	int bt;
	// 指针类型长度为4个字节
	int PTR_SIZE = 4;

	bt = type->t & T_BTYPE;

	switch (bt)
	{
	case T_STRUCT:
		sym = type->ref;
		*align = sym->reg;
		return sym->value;

	case T_PTR:
		if (type->t & T_ARRAY)
		{
			sym = type->ref;
			return type_size(&sym->type, align) * sym->value;
		}
		else
		{
			*align = PTR_SIZE;
			return PTR_SIZE;
		}

	case T_INT:
		*align = 4;
		return 4;

	case T_SHORT:
		*align = 2;
		return 2;

	default:			// char, void, function       
		*align = 1;
		return 1;
	}
}
/***********************************************************
* fun:	    生成指针类型
* type:		原数据类型
**********************************************************/
void LSCCompilerImp::mk_pointer(Type *type)
{
	Symbol *sym;
	sym = sym_push(SC_ANOM, type, 0, -1);
	type->t = T_PTR;
	type->ref = sym;
}
/***********************************************************
* LSCCompiler初始化
**********************************************************/
int LSCCompilerImp::init() {
	int ret = 0;
	if (m_TkString)
		ret = m_TkString->init(8);
	if (ret != SCP_ERROR_NONE)
		return SCP_INIT_FAILED;
	if (m_SourceString)
		ret = m_SourceString->init(8);
	if (ret != SCP_ERROR_NONE)
		return SCP_INIT_FAILED;
	
	if (m_TkHashTable)
		ret = m_TkHashTable->init();
	m_TkHashTable->init_lex();
	if (ret != SCP_ERROR_NONE)
		return SCP_INIT_FAILED;
	return ret;
}
int LSCCompilerImp::open(char* file_name) {
	if (file_name == NULL)
		return SCP_INVALID_PARAM;
	m_file = fopen(file_name, "rb+");
	//printf("%d\n", GetLastError());
	if (m_file == NULL)
		return SCP_OPEN_FAILED;
	return SCP_ERROR_NONE;
}
int LSCCompilerImp::close() {
	if (m_file == NULL)
		return SCP_CLOSE_FAILED;
	return fclose(m_file) == 0 ? SCP_ERROR_NONE : SCP_CLOSE_FAILED;
}

/***********************************************************
//词法分析函数
//注：单独使用需注释掉 宏SYNTAXANALYSIS
**********************************************************/
void LSCCompilerImp::LexicalAnalysis() {
	getch();
	do {
		get_token();
		token_colored(LEX_NORMAL);
	} while (token != TK_EOF);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut,
		BACKGROUND_GREEN |
		BACKGROUND_INTENSITY);
	printf("\n\n\n代码行数：%d\n\n\n", line_num);
	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

	LogMessage("\n\n\n代码行数：%d行", line_num);
}

//语法分析

/***********************************************************
//语法缩进
//注：与LexicalAnalysis()相比，多了语法状态判断
**********************************************************/
void LSCCompilerImp::syntax_indent() {
	switch (syntax_state) {
	case SC_STATE_NULL:
		token_colored(LEX_NORMAL);
		break;
	case SC_STATE_SP:
		printf(" ");
		token_colored(LEX_NORMAL);
		break;
	case SC_STATE_LF_HT:
		if (token == TK_END)//遇到},缩进减少一级
			syntax_level--;
		printf("\n");
		print_tab(syntax_level);
		token_colored(LEX_NORMAL);
		break;
	case SC_STATE_DELAY:
		break;
	}
	syntax_state = SC_STATE_NULL;
}
/**************************************************
//词法分析
**************************************************/

void LSCCompilerImp::SyntaxAnalysis() {

	getch();
	get_token();
	syntaxAnalysis_unit();
}
/**************************************************
//解析翻译模块
<syntaxAnalysis_unit>::={external_declaration}<TK_EOF>
**************************************************/
void LSCCompilerImp::syntaxAnalysis_unit() {
	while (token != TK_EOF) {
		external_declaration(SC_GLOBAL);
	}
}

/**************************************************
*func:			解析外部声明
*sType:			存储类型，SC_LOCAL or SC_GLOBAL

<external_declaration>::=
<type_specifier>(<TK_SEMICOLON>
|<declaration><funcbody>
|<declaration>[<TK_ASSIGN><initializer>]
{<TK_COMMA><declaration>[<TK_ASSIGN><initializer>]}<TK_SEMICOLON>)
<function_definition>|<declaration>

**************************************************/
void LSCCompilerImp::external_declaration(int sType) {

	Type btype, type;
	int v, has_init, r, addr = 0;
	Symbol *sym;
	Section *sec = NULL;

	if (!type_specifier(&btype)) {
		LogError("%s, %s 缺少<类型区分符> line: %d",__FILE__,__FUNCTION__,__LINE__);
	}

	if (btype.t == T_STRUCT && token == TK_SEMICOLON){
		get_token();
		return;
	}
	while (1) {
		type = btype;
		declarator(&type, &v, NULL);
		if (token == TK_BEGIN) {
			if (sType == SC_LOCAL)
				LogError("%s, %s 不支持函数嵌套定义 line: %d ", __FILE__, __FUNCTION__, __LINE__);
			
			if ((type.t & T_BTYPE) != T_FUNC)
				LogError("%s, %s 缺少函数定义 line: %d ", __FILE__, __FUNCTION__, __LINE__);
				
			sym = sym_search(v);
			if (sym)	// 函数前面声明过，现在给出函数定义
			{
				if ((sym->type.t & T_BTYPE) != T_FUNC)
					LogError("%s, %s 函数重定义 line: %d ", __FILE__, __FUNCTION__, __LINE__);
				sym->type = type;
			}
			else
			{
				sym = func_sym_push(v, &type);
			}
			sym->reg = SC_SYM | SC_GLOBAL;			
			funcbody(sym);
			break;
		}
		else {

			/*if (token == TK_ASSIGN) {
				get_token();
				initializer();
			}
			if (token == TK_COMMA) {
				get_token();
			}
			else {
				syntax_state = SC_STATE_LF_HT;
				skip(TK_SEMICOLON);
				break;
			}*/
			if ((type.t & T_BTYPE) == T_FUNC) // 函数声明
			{
				if (sym_search(v) == NULL)
				{
					sym_push(v, &type, SC_GLOBAL | SC_SYM, 0);
				}
			}
			else //变量声明
			{
				r = 0;
				if (!(type.t & T_ARRAY))
					r |= SC_LVAL;

				r |= sType;
				has_init = (token == TK_ASSIGN);

				if (has_init)
				{
					get_token(); //不能放到后面，char str[]="abc"情况，需要allocate_storage求字符串长度				    
				}

				//sec = allocate_storage(&type, r, has_init, v, &addr);
				sym = var_sym_push(&type, r, v, addr);
				/*if (sType == SC_GLOBAL)
					coffsym_add_update(sym, addr, sec->index, 0, IMAGE_SYM_CLASS_EXTERNAL);
*/
				if (has_init)
				{
					initializer(&type, addr, sec);
				}
			}
			if (token == TK_COMMA)
			{
				get_token();
			}
			else
			{
				syntax_state = SC_STATE_LF_HT;
				skip(TK_SEMICOLON);
				break;
			}
		}
	}
}

/***********************************************************
*func:		类型区分符
*type:		数据类型
*return:	是否发现合法的类型区分符

<type_specifier>::=<KW_INT> |<KW_CHAR>|<KW_SHORT>|<KW_VOID>|<struct_specifier>
***********************************************************/

int LSCCompilerImp::type_specifier(Type *type) {
	
	int t = 0,type_found = 0;
	Type _type;

	switch (token) {
	case KW_CHAR:
		t = T_CHAR;
		type_found = 1;
		syntax_state = SC_STATE_SP;
		get_token();
		break;
	case KW_INT:
		t = T_INT;
		type_found = 1;
		syntax_state = SC_STATE_SP;
		get_token();
		break;
	case KW_SHORT:
		t = T_SHORT;
		type_found = 1;
		syntax_state = SC_STATE_SP;
		get_token();
		break;
	case KW_VOID:
		t = T_VOID;
		type_found = 1;
		syntax_state = SC_STATE_SP;
		get_token();
		break;
	case KW_STRUCT:
		syntax_state = SC_STATE_SP;
		struct_specifier(&_type);
		type->ref = _type.ref;
		t = T_STRUCT;
		type_found = 1;
		break;
	default:
		break;
	}
	type->t = t;
	return type_found;
}

/**************************************************
*func:		结构体区分符
*type:		结构类型

<struct_specifier>::=
<KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END>
|<KW_STRUCT><IDENTIFIER>
**************************************************/

void LSCCompilerImp::struct_specifier(Type *type) {
	
	int v;
	Symbol *sym;
	Type _type;

	get_token();
	v = token;

	syntax_state = SC_STATE_DELAY;
	get_token();
	if (token == TK_BEGIN)				//结构体定义
		syntax_state = SC_STATE_LF_HT;
	else if (token == TK_OPENPA)			//sizeof(struct name)
		syntax_state = SC_STATE_NULL;
	else									//结构体声明
		syntax_state = SC_STATE_SP;
	syntax_indent();

	if (v < TK_IDENT)						//关键字不能作为结构体名称
		LogError("%s, %s 关键字不能作为结构名称 line: %d",__FILE__,__FUNCTION__,__LINE__);

	sym = struct_search(token);
	if (!sym) {
		_type.t = KW_STRUCT;
		// -1表示结构体未定义
		sym = sym_push(token | SC_STRUCT,&_type,0,-1);
		sym->reg = 0;
	}
	type->t = T_STRUCT;
	type->ref = sym;

	if (token == TK_BEGIN)
		struct_declaration_list(type);
}


/**************************************************
*func:		结构体声明符表
*type:		结构类型
<struct_declaration_list>::= <struct_declaration>{<struct_declaration>}
**************************************************/
void LSCCompilerImp::struct_declaration_list(Type *type) {
	
	int maxalign, offset;
	Symbol *sym, **psym;
	sym = type->ref;

	syntax_state = SC_STATE_LF_HT;	//第一个结构体成员另起一行
	syntax_level++;					//结构体成员变量声明，缩进增加一级
	get_token();

	if (sym->value != -1) {// -1表示结构体未定义
		LogError("结构体已定义!");
	}
	maxalign = 1;
	psym = &sym->next;
	offset = 0;
	while (token != TK_END) {
		struct_declaration(&maxalign, &offset, &psym);
	}
	skip(TK_END);
	syntax_state = SC_STATE_LF_HT;

	sym->value = calc_align(offset, maxalign);//结构体大小 ???
	sym->reg = maxalign; //结构体对齐
}

/**************************************************
*func:					结构体声明
* maxalign(输入,输出):	成员最大对齐粒度
* offset(输入,输出):	偏移量
* ps(输出):				结构定义符号

<struct_declaration>::=<type_specifier><declarator>{<TK_COMMA><declarator>}<TK_SEMICOLON>
**************************************************/

void LSCCompilerImp::struct_declaration(int *maxalign, int *offset, Symbol ***ps) {
	
	int v, size, align;
	Symbol *sym;
	Type _type, btype;
	int force_align;

	type_specifier(&btype);

	while (1) {

		v = 0;
		_type = btype;
		declarator(&_type, &v, &force_align);
		size = type_size(&_type, &align);

		if (force_align & ALIGN_SET)
			align = force_align & ~ALIGN_SET;

		*offset = calc_align(*offset, align);

		if (align > *maxalign)
			*maxalign = align;
		sym = sym_push(v | SC_MEMBER, &_type, 0, *offset);
		*offset += size;
		**ps = sym;
		*ps = &sym->next;

		if (token == TK_SEMICOLON || token == TK_EOF)//???
			break;
		skip(TK_COMMA);
	}
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);
}

/**************************************************
//函数调用约定
<function_calling_convebtion>::=<KW_CDECL>|<KW_STDCALL>
**************************************************/

void LSCCompilerImp::function_calling_convebtion(int &func) {
	func = KW_CDECL;
	if (token == KW_CDECL || token == KW_STDCALL) {
		func = token;
		syntax_state = SC_STATE_SP;
		get_token();
	}
}

/**************************************************
*func:			结构成员对齐（__align(n)） 这里只能为1、2、4
*force_align:	强制对齐粒度
<struct_member_aligment>::=<KW_ALIGN><TK_OPENPA><TK_CINT><TK_CLOSEPA>
**************************************************/

void LSCCompilerImp::struct_member_aligment(int *force_align) {
	
	int align = 1;
	if (token == KW_ALIGN) {
		
		get_token();
		skip(TK_OPENPA);
		if (token == TK_CINT) {
			get_token();
			align = tkvalue;
		}		
		else
			LogError("%s, %s 不是整型常量 line:%d",__FILE__,__FUNCTION__,__LINE__);
		skip(TK_CLOSEPA);
		if (align != 1 && align != 2 && align != 4)
			align = 1;
		align |= ALIGN_SET;
		*force_align = align;
	}
	else
		*force_align = 1;
}

/**************************************************
*func:			解析声明符
*type:			数据类型
*token:			单词编号
*force_align:	强制对其力度

<declarator>::={<TK_STAR>}[<struct_member_aligment>][<struct_member_aligment>]<direct_declarator>
**************************************************/

void LSCCompilerImp::declarator(Type *type, int *token, int *force_align) {
	
	int func;
	while (LSCCompilerImp::token == TK_STAR) {
		mk_pointer(type);
		get_token();
	}
	function_calling_convebtion(func);
	if(force_align)
		struct_member_aligment(force_align);
	direct_declarator(type,token,func);
}

/**************************************************
*func:			直接声明符
*type:			数据类型
*token:			单词编号
*func_call:		函数调用约定
<direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
**************************************************/

void LSCCompilerImp::direct_declarator(Type *type, int *token, int func_call) {
	if (LSCCompilerImp::token >= TK_IDENT){
		*token = LSCCompilerImp::token;
		get_token();
	}
		
	else {
		LogError("%s, %s 不是标识符, line:%d",__FILE__,__FUNCTION__,__LINE__);
	}
		
	direct_declarator_postfix(type, func_call);
}

/**************************************************
*func:			直接声明符后缀
*type:			数据类型
*func_call:		函数调用约定

<direct_declarator_postfix>::={<TK_OPENBR><TK_CINT><TK_CLOSEBR>  //[int]
|<TK_OPENBR><TK_CLOSEBR>                     //[]
|<TK_OPENPA><parameter_type_list><TK_CLOSEPA> //(...)
|<TK_OPENPA><TK_CLOSEPA>}						//()
**************************************************/

void LSCCompilerImp::direct_declarator_postfix(Type *type, int func_call) {
	int n;
	Symbol *sym;

	if (token == TK_OPENPA) {

		parameter_type_list(type, func_call);
	}		
	else if (token == TK_OPENBR) {//数组解析

		get_token();
		n = -1;
		if (token == TK_CINT) {

			get_token();
			n = tkvalue;
		}
		skip(TK_CLOSEBR);
		direct_declarator_postfix(type, func_call);
		sym = sym_push(SC_ANOM, type, 0, n);
		type->t = T_ARRAY | T_PTR;
		type->ref = sym;
	}
}

/**************************************************
*func:			解析形参类型表
*type:			数据类型
*func_call:		函数调用约定

<parameter_type_list>::=<type_specifier>{<declarator>}
{<TK_COMMA><type_specifier>{<declarator>}}<TK_COMMA><TK_ELLIPSIS>
**************************************************/


void LSCCompilerImp::parameter_type_list(Type *type, int func_call) {
	
	int n;
	Symbol **plast, *sym, *first;
	Type t;

	get_token();
	first = NULL;
	plast = &first;

	while (token != TK_CLOSEPA) {

		if (!type_specifier(&t))
			LogError("%s, %s 无效类型标识符 line:%d",__FILE__,__FUNCTION__,__LINE__);
		
		declarator(&t,&n,NULL);
		sym = sym_push(n | SC_PARAMS, &t, 0, 0);
		//作用：func(a,b,c) a->b->c
		*plast = sym;//1.first = sym;2之后. sym->next = sym
		plast = &sym->next;

		if (token == TK_CLOSEPA)
			break;
		skip(TK_COMMA);
		if (token == TK_ELLIPSIS) {

			func_call = KW_CDECL;
			get_token();
			break;
		}
	}
	syntax_state = SC_STATE_DELAY;
	skip(TK_CLOSEPA);
	if (token == TK_BEGIN)				//函数定义
		syntax_state = SC_STATE_LF_HT;
	else
		syntax_state = SC_STATE_NULL;	//函数声明
	syntax_indent();

	// 此处将函数返回类型存储，然后指向参数，最后将type设为函数类型，引用的相关信息放在ref中
	sym = sym_push(SC_ANOM, type, func_call, 0);
	sym->next = first;
	type->t = T_FUNC;
	type->ref = sym;
}

/**************************************************
*func:			函数体解析
* sym:			函数符号

<funcbody>::=<compound_statement>
**************************************************/
//???
void LSCCompilerImp::funcbody(Symbol *sym) {

	/*ind = sec_text->data_offset;
	coffsym_add_update(sym, ind, sec_text->index, CST_FUNC, IMAGE_SYM_CLASS_EXTERNAL);*/
	/* 放一匿名符号在局部符号表中 */
	sym_push_direct(&m_local_sym_stack, SC_ANOM, &int_type, 0);
	/*gen_prolog(&sym->type);
	rsym = 0;*/
	compound_statement(NULL,NULL);
	//backpatch(rsym, ind);
	//gen_epilog();
	//sec_text->data_offset = ind;
	sym_pop(&m_local_sym_stack, NULL); /* 清空局部符号栈*/
}

/**************************************************
*func:			解析初值符
*type:			变量类型
*value:			变量相关值
*sec:			变量所在节

<initializer>::=<assignment_expression>
**************************************************/

void LSCCompilerImp::initializer(Type *type, int value, Section *sec) {
	
	if (type->t & T_ARRAY && sec){

		memcpy(sec->data + value, m_TkString->str(), m_TkString->size());
		get_token();
	}
	else {
		
		assignment_expression();
		//init_variable(type, sec, c, 0);
	}
	
}

/**************************************************
*func:			语句解析
*bsym:			break跳转位置
*csym:			continue跳转位置

<statement>::=<compound_statement> |<if_statement> |<return_statement>|<break_statement>|<continue_statement>|<for_statement>|<expression_statement>
**************************************************/

void LSCCompilerImp::statement(int *bsym, int *csym)
{
	switch (token)
	{
	case TK_BEGIN:
		compound_statement(bsym, csym);
		break;
	case KW_IF:
		if_statement(bsym, csym);
		break;
	case KW_RETURN:
		return_statement();
		break;
	case KW_BREAK:
		break_statement(bsym);
		break;
	case KW_CONTINUE:
		continue_statement(csym);
		break;
	case KW_FOR:
		for_statement(bsym, csym);
		break;
	default:
		expression_statement();
		break;
	}
}

/**************************************************
*func:			复合语句
*bsym:			break跳转位置
*csym:			continue跳转位置

<compound_statement>::=<TK_BEGIN>{<declaration>}{statement}<TK_END>
**************************************************/

void LSCCompilerImp::compound_statement(int *bsym, int *csym) {

	Symbol *sym;
	sym = &m_local_sym_stack.top();
	syntax_state = SC_STATE_LF_HT;
	syntax_level++;

	get_token();
	while (is_type_specifier(token)) {

		external_declaration(SC_LOCAL);
	}
	while (token != TK_END) {

		statement(bsym, csym);
	}
	sym_pop(&m_local_sym_stack,sym);
	syntax_state = SC_STATE_LF_HT;
	get_token();
}

/**************************************************
*判断是否为类型区分符
**************************************************/

int LSCCompilerImp::is_type_specifier(int v) {
	switch (v) {
	case KW_CHAR:
	case KW_INT:
	case KW_SHORT:
	case KW_VOID:
	case KW_STRUCT:
		return 1;
	default:
		break;
	}
	return 0;
}

/**************************************************
*表达式语句
<expression_statement>::=<TK_SEMICOLON>|<expression><TK_SEMICOLON>
**************************************************/

void LSCCompilerImp::expression_statement() {

	while (token != TK_SEMICOLON) {
		expression();
	}
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);
}

/**************************************************
*func:			if语句
*bsym:			break跳转位置
*csym:			continue跳转位置

<if_statement>::=<KW_IF><TK_OPENPA><expression><TK_CLOSEPA>
<statement>[<KW_ELSE><statement>]
**************************************************/

void LSCCompilerImp::if_statement(int *bsym, int *csym) {

	syntax_state = SC_STATE_SP;
	get_token();
	skip(TK_OPENPA);
	expression();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_CLOSEPA);
	statement(bsym, csym);
	if (token == KW_ELSE) {
		syntax_state = SC_STATE_LF_HT;
		get_token();
		statement(bsym, csym);
	}
}

/**************************************************
*func:				解析for语句
*bsym:				break跳转位置
*csym:				continue跳转位置
<for_statement>::=<KW_FOR><TK_OPENPA><expression_statement><expression_statement><expression><TK_CLOSEPA><statement>
**************************************************/

void LSCCompilerImp::for_statement(int *bsym, int *csym) {
	get_token();
	skip(TK_OPENPA);
	if (token != TK_SEMICOLON)
		expression();
	skip(TK_SEMICOLON);
	if (token != TK_SEMICOLON)
		expression();
	skip(TK_SEMICOLON);
	if (token != TK_SEMICOLON)
		expression();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_CLOSEPA);
	statement(bsym,csym);
}

/**************************************************
*func:				continue语句
*csym:				continue跳转位置

<continue_statement>::=<KW_CONTINUE><TK_SEMICOLON>
注：当continue不在循环内时，编译也可以通过，语义分析阶段加以处理这种情况
**************************************************/

void LSCCompilerImp::continue_statement(int *csym) {

	if (!csym)
		LogError("%S, %s 此处不能用continue. line:%d",__FILE__,__FUNCTION__,__LINE__);

	get_token();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);
}

/**************************************************
*func:				break语句
*bsym:				break跳转位置
<break_statement>::=<KW_BREAK><TK_SEMICOLON>
**************************************************/
void LSCCompilerImp::break_statement(int *bsym) {

	if (!bsym)
		LogError("%S, %s 此处不能用break. line:%d", __FILE__, __FUNCTION__, __LINE__);

	get_token();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);

}

/**************************************************
*func:			return

<return_statement>::=<KW_RETURN><TK_SEMICOLON>|
<KW_RETURN><expression><TK_SEMICOLON>
**************************************************/

void LSCCompilerImp::return_statement() {
	syntax_state = SC_STATE_DELAY;
	get_token();
	if (token == TK_SEMICOLON)
		syntax_state = SC_STATE_LF_HT;
	else
		syntax_state = SC_STATE_SP;
	syntax_indent();////////?????????

	if (token != TK_SEMICOLON)
		expression();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);

}

/**************************************************
*表达式
<expression>::=<assignment_expression>{<TK_COMMA><assignment_expression>}
**************************************************/

void LSCCompilerImp::expression() {
	while (1) {
		assignment_expression();
		if (token != TK_COMMA)
			break;
		get_token();
	}
}

/**************************************************
*赋值表达式
<assignment_expression>::=<equality_expression>
{<TK_ASSIGN><assignment_expression>};
**************************************************/
void LSCCompilerImp::assignment_expression() {
	equality_expression();
	if (token == TK_ASSIGN) {
		get_token();
		assignment_expression();
	}
}

/**************************************************
*相等类表达式
<equality_expression>::=<relational_expression>
{<TK_EQ><relational_expression>
| <TK_NEQ><relational_expression>}
**************************************************/

void LSCCompilerImp::equality_expression() {
	relational_expression();
	while (token == TK_EQ || token == TK_NEQ) {
		get_token();
		relational_expression();
	}
}

/**************************************************
*关系表达式
<relational_expression>::=<additive_expression>{
<TK_LT><additive_expression>
|<TK_GT><additive_expression>
|<TK_LEQ><additive_expression>
|<TK_GEQ><additive_expression>
}
**************************************************/

void LSCCompilerImp::relational_expression() {
	additive_expression();
	while (token == TK_LT || token == TK_LT || token == TK_LT || token == TK_LT) {
		get_token();
		additive_expression();
	}
}

/**************************************************
*加减类
<additive_expression>::=<multiplicative_expression>{
<TK_PLUS><multiplicative_expression>
<TK_MINUS><multiplicative_expression>
}
**************************************************/
void LSCCompilerImp::additive_expression() {
	multiplicative_expression();
	while (token == TK_PLUS || token == TK_MINUS) {
		get_token();
		multiplicative_expression();
	}
}

/**************************************************
*乘除类
<multiplicative_expression>::=<unary_expression>{
<TK_STAR><unary_expression>
|<TK_DIVIDE><unary_expression>
|<TK_MOD><unary_expression>
}
**************************************************/

void LSCCompilerImp::multiplicative_expression() {
	unary_expression();
	while (token == TK_STAR || token == TK_DIVIDE || token == TK_MOD) {
		get_token();
		unary_expression();
	}
}

/**************************************************
*一元表达式
<unary_expression>::=<postfix_expression>
|<TK_AND><unary_expression>
|<TK_STAR><unary_expression>
|<TK_PLUS><unary_expression>
|<TK_MINUS><unary_expression>
|<sizeof_expression>
**************************************************/

void LSCCompilerImp::unary_expression() {
	switch (token) {
	case TK_AND:
		get_token();
		unary_expression();
		break;
	case TK_STAR:
		get_token();
		unary_expression();
		break;
	case TK_PLUS:
		get_token();
		unary_expression();
		break;
	case TK_MINUS:
		get_token();
		unary_expression();
		break;
	case KW_SIZEOF:
		sizeof_expression();
		break;
	default:
		postfix_expression();
		break;
	}
}

/**************************************************
*func:			解析sizeof

<sizeof_expression>::=<KW_SIZEOF><TK_OPENPA><type_specifier><TK_CLOSEPA>
**************************************************/
void LSCCompilerImp::sizeof_expression() {

	int align, size;
	Type type;

	get_token();
	skip(TK_OPENPA);
	type_specifier(&type);//类型区分符
	skip(TK_CLOSEPA);

	size = type_size(&type, &align);
	if (size < 0)
		LogError("%s, %s sizeof计算类型尺寸失败 line: %d",__FILE__,__FUNCTION__,__LINE__);
	//operand_push(&int_type, SC_GLOBAL, size);
}

/**************************************************
后缀表达式
<postfix_expression>::=<primary_expression>
{<TK_OPENPA><TK_CLOSEPA>
|<TK_OPENBR><expression><TK_CLOSEBR>
|<TK_OPENPA><argument_expression_list><TK_CLOSEPA>
|<TK_DOT><IDENTIFIER>
|<TK_POINTSTO><IDENTIFIER>
}
**************************************************/

void LSCCompilerImp::postfix_expression() {
	primary_expression();
	while (1) {
		if (token == TK_DOT || token == TK_POINTSTO) {
			get_token();
			token |= SC_MEMBER;
			get_token();
		}
		else if (token == TK_OPENBR) {
			get_token();
			expression();
			skip(TK_CLOSEBR);
		}
		else if (token == TK_OPENPA) {
			argument_expression_list();
		}
		else
			break;
	}
}
/**************************************************
*func:			初值表达式

<primary_expression>::=<IDENTIFIER> | <TK_CINT>
| <TK_CSTR> | <TK_CCHAR> | <TK_OPENPA><expression><TK_CLOSEPA>
**************************************************/
void LSCCompilerImp::primary_expression() {

	int t,r,addr = 0;
	Type type;
	Symbol *sym;
	Section *sec = NULL;

	switch (token) {

	case TK_CINT:
	case TK_CCHAR:
		//operand_push(&int_type, SC_GLOBAL, tkvalue);
		get_token();
		break;
	case TK_CSTR:
		t = T_CHAR;
		type.t = t;
		mk_pointer(&type);
		type.t |= T_ARRAY;
		//sec = allocate_storage(&type, SC_GLOBAL, 2, 0, &addr);
		var_sym_push(&type, SC_GLOBAL, 0, addr);
		initializer(&type, addr, sec);
		break;
	case TK_OPENPA:
		get_token();
		expression();
		skip(TK_CLOSEPA);
		break;
	default:
		t = token;
		get_token();
		if (t < TK_IDENT)
			LogError("%s, %s 缺少标识符或常量 line:%d",__FILE__,__FUNCTION__,__LINE__);
		sym = sym_search(t);
		if (!sym) {

			if (token != TK_OPENPA)
				LogError("%s, %s  %s未声明 line:%d", __FILE__, __FUNCTION__, get_tkstr(t),__LINE__);
			
			sym = func_sym_push(t, &default_func_type);//允许函数不声明，直接引用
			sym->reg = SC_GLOBAL | SC_SYM;
		}
		r = sym->reg;
		//operand_push(&s->type, r, s->c);
		/* 符号引用，操作数必须记录符号地址 */
		//if (optop->r & SC_SYM)
		//{
		//	optop->sym = s;
		//	optop->value = 0;  //用于函数调用，及全局变量引用 printf("g_cc=%c\n",g_cc);
		//}
		break;
	}
}

/**************************************************
实参表达式表
<argument_expression_list>::=<assignment_expression>
{<TK_COMMA><assignment_expression>}
**************************************************/
void LSCCompilerImp::argument_expression_list() {
	get_token();
	if (token != TK_CLOSEPA) {
		for (;;) {
			assignment_expression();
			if (token == TK_CLOSEPA)
				break;
			skip(TK_COMMA);
		}
	}
	skip(TK_CLOSEPA);
}

/*************************************************************************************************************/



Symbol* LSCCompilerImp::sym_push_direct(std::stack<Symbol> *ss, int token, Type *type, int value) {
	Symbol s, *p = NULL;
	s.value = value;
	s.next = NULL;
	s.token = token;
	s.type.ref = type->ref;
	s.type.t = type->t;
	ss->push(s);
	p = &(ss->top());
	return p;
}
//将符号放在符号栈中,动态判断是放入全局符号栈还是局部符号栈
Symbol* LSCCompilerImp::sym_push(int token, Type *type, int reg, int value) {
	Symbol *ps, **pps;
	_TkWord *tw;
	std::stack<Symbol> *ss;

	if (!m_local_sym_stack.empty()){
		ss = &m_local_sym_stack;
	}
	else {
		ss = &m_global_sym_stack;
	}
	ps = sym_push_direct(ss,token,type,value);
	ps->reg = reg;

	// 不记录结构体成员及匿名符号
	//？？？
	if ((token & SC_STRUCT) || token < SC_ANOM) {
		// 更新单词sym_struct或sym_identifier字段
		tw = (_TkWord*)m_TkHashTable->m_tkTable.str(token & ~SC_STRUCT);
		if (token & SC_STRUCT)
			pps = &tw->sym_struct;
		else
			pps = &tw->sym_identifier;
		ps->prev_tok = *pps;
		*pps = ps;
	}
	return ps;

}
//将函数符号放入全局符号表中
Symbol* LSCCompilerImp::func_sym_push(int token, Type *type) {
	Symbol *ps, **pps;
	ps = sym_push_direct(&m_global_sym_stack,token,type,0);

	pps = &((_TkWord*)m_TkHashTable->m_tkTable.str(token))->sym_identifier;
	// 同名符号，函数符号放在最后-> ->...s 
	while (*pps != NULL){
		pps = &(*pps)->prev_tok;
	}
	ps->prev_tok = NULL;
	*pps = ps;
	return ps;
}

//将变量放入全局符号表中
Symbol* LSCCompilerImp::var_sym_push(Type *type, int reg, int token, int value) {
	Symbol *sym = NULL;
	if ((reg & SC_VALMASK) == SC_LOCAL) {//局部变量
		sym = sym_push(token, type, reg, value);
	}
	else if (token && (reg & SC_VALMASK) == SC_GLOBAL) {//全局变量
		sym = sym_search(token);
		if (sym) {
			LogError("%s, %s %s重定义 line %d",__FILE__,__FUNCTION__, ((_TkWord*)m_TkHashTable->m_tkTable.str(token))->p_word,__LINE__);
		}
		else {
			sym = sym_push(token,type, reg | SC_SYM,0);
		}
	
	}
	return sym;
}
//将节名称放入全局符号表中
Symbol* LSCCompilerImp::sec_sym_push(char* sec, int value) {
	_TkWord* tp;
	Symbol *s;
	Type type;

	type.t = T_INT;

	tp = m_TkHashTable->insert(sec);
	token = tp->tkcode;
	s = sym_push(token,&type,SC_GLOBAL,value);
	return s;
}
//弹出栈中符号直到栈顶符号为'b'
void LSCCompilerImp::sym_pop(std::stack<Symbol> *ptop, Symbol *b) {
	Symbol *s, **ps;
	_TkWord *ts;
	int v;

	s = &ptop->top();
	while (s != b) {
		v = s->token;
		// 更新单词表中sym_struct sym_identifier
		if ((v & SC_STRUCT) || v < SC_ANOM) {
			ts = (_TkWord*)m_TkHashTable->m_tkTable.str(v & ~SC_STRUCT);
			if (v & SC_STRUCT)
				ps = &ts->sym_struct;
			else
				ps = &ts->sym_identifier;
			*ps = s->prev_tok;
		}
		ptop->pop();
		s = &ptop->top();
	}
}

//查找结构定义
Symbol *LSCCompilerImp::struct_search(int token) {
	if (token >= m_TkHashTable->m_tkTable.size())
		return NULL;
	else
		return ((_TkWord*)m_TkHashTable->m_tkTable.str(token))->sym_struct;
}
//查找结构定义
Symbol *LSCCompilerImp::sym_search(int token) {
	if (token >= m_TkHashTable->m_tkTable.size())
		return NULL;
	else
		return ((_TkWord*)m_TkHashTable->m_tkTable.str(token))->sym_identifier;
}


/***********************************************************
*func:				创建Section
*name:				Section名称
*Characteristics:	Section属性
*return:			创建的Section
**********************************************************/
Section* LSCCompilerImp::section_create(char *name, int Characteristics) {

	Section *sec = NULL;
	int initsize = 8;

	sec = (Section *)malloc(sizeof(Section));
	if (sec == NULL) {
		sec = (Section *)malloc(sizeof(Section));
		if (sec == NULL)
			LogError("%s, %s Section create fail. line: %d",__FILE__,__FUNCTION__,__LINE__);
	}
	memset(sec,0, sizeof(Section));
	memcpy(sec->sh.Name, name, strlen(name) * sizeof(char));

	sec->sh.Characteristics = Characteristics;
	sec->index = m_section.size() + 1;
	sec->data = (char*)malloc(initsize * sizeof(char));
	memset(sec->data,0, initsize * sizeof(char));
	sec->data_allocated = initsize;
	//IMAGE_SCN_LNK_REMOVE:此节点不会成为最终形成的映像文件的一部分。
	//此标志只对目标文件合法，通过这个标志可以判断该节点是否需要放进映像文件
	//如果需要放入映像文件中，nsec_image加一
	//在所有节构建完成后，nsec_image变量记录了映像文件的节个数
	if (!(Characteristics & IMAGE_SCN_LNK_REMOVE))		
		nsec_image++;
	m_section.push_back(sec);
	return sec;
}

/***********************************************************
*func:				给节数据预留至少increment大小的内存空间
*sec:				预留内存空间的节
*increment:			预留的空间大小
*return:			预留内存空间的首地址
**********************************************************/
void* LSCCompilerImp::section_malloc(Section *sec, int increment) {

	int offset, offset1;
	offset = sec->data_offset;
	offset1 = offset + increment;
	if (offset1 > sec->data_allocated)
		section_realloc(sec, offset1);
	sec->data_offset = offset1;
	return sec->data + offset;
}

/***********************************************************
*func:				给节数据重新分配内存,并将内容初始化为0
*sec:				重新分配内存的节
*new_size:			节数据新长度
return:				返回错误值
**********************************************************/
int LSCCompilerImp::section_realloc(Section *sec, int new_size) {

	int size;
	char *data;

	size = sec->data_allocated;
	while (size < new_size)
		size = size * 2;
	data = (char*)realloc(sec->data, size);
	if (!data)
	{
		LogError("%s %s Section realloc failed. line: %d ", __FILE__, __FUNCTION__, __LINE__);
		return SCP_MALLOC_FAILED;
	}
	memset(data + sec->data_allocated, 0, size - sec->data_allocated);/* 新分配的内存数据初始化为0 */
	sec->data = data;
	sec->data_allocated = size;
	return SCP_ERROR_NONE;
}

/***********************************************************
* func:					新建存储COFF符号表的节
* symtab_name:			COFF符号表名
* Characteristics:		节属性
* strtab_name:			与符号表相关的字符串表,字符串节负责保存符号名称
* return:				存储COFF符号表的节
**********************************************************/
Section *LSCCompilerImp::coffsym_section_create(char *symtab_name, int Characteristics, char *strtab_name) {

	Section *sec;
	sec = section_create(symtab_name, Characteristics);
	sec->link = section_create(strtab_name, Characteristics);
	sec->hashtab = (int*)malloc(sizeof(int)*MAXKEY);
	memset(sec->hashtab,0, sizeof(int)*MAXKEY);
	return sec;
}

/***********************************************************
* funv:				增加COFF符号
* symtab:			保存COFF符号表的节
* name:				符号名称
* val:				与符号相关的值
* sec_index:		定义此符号的节
* type:				Coff符号类型
* StorageClass:		Coff符号存储类别
* return:			符号COFF符号表中序号
**********************************************************/
int LSCCompilerImp::coffsym_add(Section *symtab, char* name, int val, int sec_index, short type, char StorageClass) {
	
	CoffSym *cfsym;
	int cs, keyno;
	char *csname;
	Section *strtab = symtab->link;

	int *hashtab;
	hashtab = symtab->hashtab;
	cs = coffsym_search(symtab, name);

	if (cs == 0) {
		
		cfsym = (CoffSym *)section_malloc(symtab,sizeof(CoffSym));
		csname = coffstr_add(strtab,name);

		cfsym->Name = csname - strtab->data;//偏移量
		cfsym->Value = val;
		cfsym->Section = sec_index;
		cfsym->Type = type;
		cfsym->StorageClass = StorageClass;
		keyno = elf_hash(name);
		cfsym->Next = hashtab[keyno];

		cs = cfsym - (CoffSym*)symtab->data;
		hashtab[keyno] = cs;
	}
	return cs;
}

/***********************************************************
* func:				增加或更新COFF符号,更新只适用于函数先声明后定义的情况
* sym:				符号指针
* val:				符号值
* sec_index:		定义此符号的节
* type:				Coff符号类型
* StorageClass:		Coff符号存储类别
**********************************************************/
void LSCCompilerImp::coffsym_add_update(Symbol *sym, int val, int sec_index, short type, char StorageClass) {

	char *name;
	CoffSym *cfsym;

	if (sym->value){//函数先声明后定义

		cfsym = (CoffSym *)m_section_data.sec_symtab->data[sym->value];
		cfsym->Value = val;
		cfsym->Section = sec_index;
	}
	else {
	
		name = ((_TkWord*)m_TkHashTable->m_tkTable.str(sym->token))->p_word;
		sym->value = coffsym_add(m_section_data.sec_symtab,name,val,sec_index,type,StorageClass);
	}
}
/***********************************************************
* func:			查找COFF符号
* symtab:		保存COFF符号表的节
* name:			符号名称
* return:		符号COFF符号表中序号
**********************************************************/
int LSCCompilerImp::coffsym_search(Section *symtab, char *name) {

	CoffSym *cfsym;
	int cs, keyno;
	char* csname = NULL;
	Section* strtab;

	keyno = elf_hash(name);
	strtab = symtab->link;
	cs = symtab->hashtab[keyno];

	while (cs) {
	
		cfsym = (CoffSym*)symtab->data + cs;
		csname = strtab->data + cfsym->Name;
		/*此处如果用stricmp外部调用的函数源代码中可不区分大小写，但生成的pe中引用的函数名称大小写还是正确的
		例如:printf误写为Printf编译运行结果依然是正确的,此处这么改了可能有些问题，由于elf_hash哈希函数对字母
		区分大小写，如果printf及PrintF的哈希值不同，这样会有问题，如果此处要改，必须将哈希函数改为不区分大小写，
		即在哈希函数的开头用_strupr处理一下
		*/
		if (!strcmp(name, csname))
			return cs;
		cs = cfsym->Next;
	}
	return cs;

}

/***********************************************************
* func:			增加COFF符号名字符串
* strtab:		保存COFF字符串表的节
* name:			符号名称字符串
* return:		新增COFF字符串
**********************************************************/
char* LSCCompilerImp::coffstr_add(Section *strtab, char* name) {

	int len = strlen(name);
	char *pstr = (char*)section_malloc(strtab,len+1);

	memcpy(pstr,name,len);
	return pstr;
}

/***********************************************************
* func:			增加COFF重定位信息
* offset:		需要进行重定位的代码或数据在其相应节的偏移位置
* cfsym:		符号表的索引
* section:		符号所在节
* type:			重定位类型
**********************************************************/
void LSCCompilerImp::coffreloc_direct_add(int offset, int cfsym, char section, char type){

	CoffReloc *rel;
	rel = (CoffReloc *)section_malloc(m_section_data.sec_rel, sizeof(CoffReloc));
	rel->offset = offset;
	rel->cfsym = cfsym;
	rel->section = section;
	rel->type = type;
}

/***********************************************************
* func:				增加重定位条目
* section:			符号所在节
* sym:				符号指针
* offset:			需要进行重定位的代码或数据在其相应节的偏移位置
* type:				重定位类型
**********************************************************/

void LSCCompilerImp::coffreloc_add(Section *sec, Symbol *sym, int offset, char type) {

	int cfsym;
	char* name;
	if (!sym->value) {	
		coffsym_add_update(sym, 0, IMAGE_SYM_UNDEFINED, CST_FUNC, IMAGE_SYM_CLASS_EXTERNAL);
	}
	name = ((_TkWord*)m_TkHashTable->m_tkTable.str(sym->value))->p_word;
	cfsym = coffsym_search(m_section_data.sec_symtab, name);
	coffreloc_direct_add(offset, cfsym, sec->index, type);
}

/***********************************************************
* func:	COFF初始化
**********************************************************/
void LSCCompilerImp::coff_init() {

	m_section_data.sec_text = section_create(".text",
		IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE);
	m_section_data.sec_data = section_create(".data",
		IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE |
		IMAGE_SCN_CNT_INITIALIZED_DATA);
	m_section_data.sec_rdata = section_create(".rdata",
		IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA);
	m_section_data.sec_idata = section_create(".idata",
		IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE |
		IMAGE_SCN_CNT_INITIALIZED_DATA);
	m_section_data.sec_bss = section_create(".bss",
		IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE |
		IMAGE_SCN_CNT_UNINITIALIZED_DATA);
	m_section_data.sec_rel = section_create(".rel",
		IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ);


	m_section_data.sec_symtab = coffsym_section_create(".symtab",
		IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ, ".strtab");

	m_section_data.sec_dynsymtab = coffsym_section_create(".dynsym",
		IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ, ".dynstr");

	coffsym_add(m_section_data.sec_symtab, "", 0, 0, 0, IMAGE_SYM_CLASS_NULL);
	coffsym_add(m_section_data.sec_symtab, ".data", 0, m_section_data.sec_data->index, 0, IMAGE_SYM_CLASS_STATIC);
	coffsym_add(m_section_data.sec_symtab, ".bss", 0, m_section_data.sec_bss->index, 0, IMAGE_SYM_CLASS_STATIC);
	coffsym_add(m_section_data.sec_symtab, ".rdata", 0, m_section_data.sec_rdata->index, 0, IMAGE_SYM_CLASS_STATIC);
	coffsym_add(m_section_data.sec_dynsymtab, "", 0, 0, 0, IMAGE_SYM_CLASS_NULL);
}

/***********************************************************
* func:				从当前读写位置到new_pos位置用0填补文件内容
* fp:				文件指针
* new_pos:			填补终点位置
* pad:				填补数字
**********************************************************/
void LSCCompilerImp::fpad(FILE *fp, int new_pos, int pad)
{
	int curpos = ftell(fp);
	while (++curpos <= new_pos)
		fputc(pad, fp);

}

/***********************************************************
* func:	输出目标文件
* name:	目标文件名
**********************************************************/
void LSCCompilerImp::write_obj(char *name)
{
	int file_offset;
	FILE *fout = fopen(name, "wb");
	int i, sh_size, nsec_obj = 0;
	IMAGE_FILE_HEADER *fh;

	nsec_obj = m_section.size() - 2;
	sh_size = sizeof(IMAGE_SECTION_HEADER);
	file_offset = sizeof(IMAGE_FILE_HEADER) + nsec_obj*sh_size;
	fpad(fout, file_offset,0);
	fh = (IMAGE_FILE_HEADER *)malloc(sizeof(IMAGE_FILE_HEADER));
	memset(fh,0, sizeof(IMAGE_FILE_HEADER));

	for (i = 0; i < nsec_obj; i++)
	{
		Section *sec = m_section[i];
		if (sec->data == NULL) continue;
		fwrite(sec->data, 1, sec->data_offset, fout);
		sec->sh.PointerToRawData = file_offset;
		sec->sh.SizeOfRawData = sec->data_offset;
		file_offset += sec->data_offset;
	}
	fseek(fout, SEEK_SET, 0);
	fh->Machine = IMAGE_FILE_MACHINE_I386;
	fh->NumberOfSections = nsec_obj;
	fh->PointerToSymbolTable = m_section_data.sec_symtab->sh.PointerToRawData;
	fh->NumberOfSymbols = m_section_data.sec_symtab->sh.SizeOfRawData / sizeof(CoffSym);
	fwrite(fh, 1, sizeof(IMAGE_FILE_HEADER), fout);
	for (i = 0; i < nsec_obj; i++)
	{
		Section *sec = m_section[i];
		fwrite(sec->sh.Name, 1, sh_size, fout);
	}

	free(fh);
	fclose(fout);
}

void LSCCompilerImp::operand_push(Type *type, int r, int value){

}
void LSCCompilerImp::operand_pop(){

}
void LSCCompilerImp::operand_swap(){

}
/***********************************************************
* func:			操作数赋值
* t:			操作数数据类型
* r:			操作数存储类型
* value:		操作数值
**********************************************************/
void LSCCompilerImp::operand_assign(Operand *opd, int t, int r, int v){
	
	opd->type.t = t;
	opd->r = r;
	opd->value = v;
}
void LSCCompilerImp::cancel_lvalue(){

}
/***********************************************************
func:	间接寻址，这里的 * 号是指针间接寻址运算符(Indirection Operator)
**********************************************************/
void LSCCompilerImp::indirection(){

	if ((m_optop.type.t & T_BTYPE) != T_PTR) {
	
		if ((m_optop.type.t & T_BTYPE) == T_FUNC)
			return;
		LogMessage("%s, %s, 缺少指针line: %d",__FILE__,__FUNCTION__,__LINE__);
	}

}
/***********************************************************
* func:		向代码节写入一个字节
* c:		字节值
**********************************************************/
void LSCCompilerImp::gen_byte(char c) {

	int l_ind = ind + 1;
	if (l_ind > m_section_data.sec_text->data_allocated)
		section_realloc(m_section_data.sec_text,l_ind);
	m_section_data.sec_text->data[ind] = c;
	ind = l_ind;
}
/***********************************************************
指令前缀
指令前缀分为四组，每组都有一组允许前缀码：
1、锁定和重复前缀
2、段重写
3、operand-size重写
4、address-size重写

* func:			生成指令前缀
* opcode:		指令前缀编码
**********************************************************/
void LSCCompilerImp::gen_prefix(char opcode) {

	gen_byte(opcode);
}
/***********************************************************
* func:			生成单字节指令
* opcode:		指令编码
**********************************************************/
void LSCCompilerImp::gen_opcode1(char opcode) {

	gen_byte(opcode);
}

/***********************************************************
* func:			生成双字节指令
* first:		指令第一个字节
* second:		指令第二个字节
**********************************************************/
void LSCCompilerImp::gen_opcode2(char first, char second) {

	gen_byte(first);
	gen_byte(second);
}
/***********************************************************
ModRM结构表
ModRM.mod [7:6] 提供寻址模式: 11 = register  !11 = memory
ModRM.reg [5:3] 提供 register 或者对 Opcode 进行补充
ModRM.r/m [2:0] 提供 register 或者 memory 依赖于 ModRM.mod

* func:		生成指令寻址方式字节ModR/M,
* sym:		符号指针
* c:		符号关联值
**********************************************************/
void LSCCompilerImp::gen_modrm(int mod, int reg_opcode, int r_m, Symbol *sym, int c) {
	
	mod <<= 6;
	reg_opcode <<= 3;
	if (mod == 0xc0) {// mod=11 寄存器寻址 89 E5(11 reg_opcode=100ESP r=101EBP) MOV EBP,ESP
		
		gen_byte(mod | reg_opcode | (r_m & SC_VALMASK));
	}
	else if ((r_m & SC_VALMASK) == SC_GLOBAL) {
	
		// mod=00 r=101 disp32  8b 05 50 30 40 00  MOV EAX,DWORD PTR DS:[403050]
		// 8B /r MOV r32,r/m32 Move r/m32 to r32
		// mod=00 r=101 disp32  89 05 14 30 40 00  MOV DWORD PTR DS:[403014],EAX
		// 89 /r MOV r/m32,r32 Move r32 to r/m32
		gen_byte(0x05 | reg_opcode); //直接寻址
		gen_addr32(r_m, sym, c);
	}
	else if ((r_m & SC_VALMASK) == SC_LOCAL) {
	
		if (c == (char)c)
		{
			// mod=01 r=101 disp8[EBP] 89 45 fc MOV DWORD PTR SS:[EBP-4],EAX
			gen_byte(0x45 | reg_opcode);
			gen_byte(c);
		}
		else
		{
			// mod=10 r=101 disp32[EBP]
			gen_byte(0x85 | reg_opcode);
			gen_dword(c);
		}
	}
	else{

		// mod=00 89 01(00 reg_opcode=000 EAX r=001ECX) MOV DWORD PTR DS:[ECX],EAX
		gen_byte(0x00 | reg_opcode | (r_m & SC_VALMASK));
	}
}
/***********************************************************
* func:			生成4字节操作数
* c:			4字节操作数
**********************************************************/
void LSCCompilerImp::gen_dword(unsigned int c) {

	gen_byte(c);
	gen_byte(c >> 8);
	gen_byte(c >> 16);
	gen_byte(c >> 24);
}
/***********************************************************
* func:			生成全局符号地址,并增加COFF重定位记录
* output constant with relocation if 'r & SC_SYM' is true
* r:			符号存储类型
* sym:			符号指针
* c:			符号关联值
**********************************************************/
void LSCCompilerImp::gen_addr32(int r, Symbol *sym, int c) {

	if(r & SC_SYM)//全局符号在代码生成时不知道符号地址，增加重定位记录一下
		coffreloc_add(m_section_data.sec_text, sym, ind, IMAGE_REL_I386_DIR32);
	gen_dword(c);
}

/***********************************************************
* func:			将操作数opd加载到寄存器r中
* r:			符号存储类型
* opd:			操作数指针
**********************************************************/
void LSCCompilerImp::operand_load(int r, Operand *opd) {

	int v, ft, fc, fr;
	ft = opd->type.t;
	fr = opd->r;
	fc = opd->value;

	v = fr & SC_VALMASK;
	if (fr & SC_LVAL) {//左值
		
		if ((ft & T_BTYPE) == T_CHAR)
			gen_opcode2(0x0f, 0xbe);//MOVSX
		else if ((ft & T_BTYPE) == T_SHORT)
			gen_opcode2(0x0f, 0xbf);//MOVSX
		else
			gen_opcode1(0x8b);

		gen_modrm(ADDR_OTHER, r, fr, opd->sym, fc);
	}
	else {
	
	
	}
}
//Log
/*************************************************************************************************************/
void LogMessage(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogMsg == NULL)
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);
	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogMsg(msg);
	}
	va_end(arg);
}

void LogError(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogError == NULL)
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);

	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogError(msg);
	}
	va_end(arg);
}

void LogWarning(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogWarning == NULL)
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);

	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogWarning(msg);
	}
	va_end(arg);
}

void LogLink(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogLink == NULL)
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);

	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogLink(msg);
	}
	va_end(arg);
}

void LogCompile(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogCompile == NULL)
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);

	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogCompile(msg);
	}
	va_end(arg);
}

