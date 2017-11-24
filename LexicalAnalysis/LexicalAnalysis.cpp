
#include "stdafx.h"
#include "LexicalAnalysis.h"

static LogFunction	g_LogMsg = NULL;
static LogFunction	g_LogError = NULL;
static LogFunction	g_LogWarning = NULL;
static LogFunction	g_LogLink = NULL;
static LogFunction	g_LogCompile = NULL;

LexicalAnalysis* LexicalAnalysisCreate(void) {
	LexicalAnalysis* pInstance = new LexicalAnalysisImp[sizeof(class LexicalAnalysisImp)];
	if (pInstance)
		return pInstance;
}
void LexicalAnalysisDestroy(LexicalAnalysis* pInstance) {
	if (pInstance)
		delete pInstance;
}

void SetLogFunction(LogFunction Logmsg, LogFunction Logerr, LogFunction LogWarnig, LogFunction LogLink, LogFunction LogCompile)
{
	g_LogMsg = Logmsg;
	g_LogError = Logerr;
	g_LogWarning = LogWarnig;
	g_LogLink = LogLink;
	g_LogCompile = LogCompile;
}


//动态字符串
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
		m_SmartString->data = (T*) malloc(sizeof(T)* initsize);
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
int SmartString<T>::reset(int resetsize) {
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
	data = (T*)realloc(m_SmartString->data, capacity);
	if (!data)
		return -1;
	m_SmartString->capacity = capacity;
	m_SmartString->data = data;
	return 0;
}

template<typename T>
int SmartString<T>::reinit(int newsize) {
	if (m_SmartString->data == NULL)
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
	m_SmartString->capacity = capacity;
	return 0;
}

template<typename T>
int SmartString<T>::append(T ch) {
	int count;
	count = m_SmartString->size + 1;
	if (count > m_SmartString->capacity)
		reinit(count);
	(m_SmartString->data)[count - 1] = ch;
	m_SmartString->size = count;
	return 0;
}
template<typename T>
void SmartString<T>::print() {
	if (m_SmartString != NULL) {
		for (int i = 0; i < m_SmartString->size; i++)
			//::printf("%c", m_SmartString->data[i]);
			LogMessage("SmartString.data[%d],%c",i, m_SmartString->data[i]);
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

template<typename T>
SmartArray<T>::SmartArray():m_SmartArray(NULL), count(0) {}

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
			LogMessage("SmartArray.data[%d]:%s",i, m_SmartArray->size);
	}
}

//单词表
TkTable::TkTable():m_Word(NULL) {
	m_tkTable.init(256);
}
TkTable::~TkTable() {
	/*if (m_Word != NULL) {
		::free(m_Word);
		m_Word = NULL;
	}*/
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
	for (tmp = &keywords[0]; tmp->p_word != NULL; tmp++) {
		direct_insert(tmp);
	}
}
//direct insert key
_TkWord* TkTable::direct_insert(_TkWord *&tw) {
	int keynu;
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

//词法分析类
LexicalAnalysisImp::LexicalAnalysisImp():token(-1), line_num(1), tkvalue(-1),
					m_file(NULL), ch(EOF) {
	m_TkString = new SmartString<char>();
	m_SourceString = new SmartString<char>();
	m_TkHashTable = new TkTable();
}
LexicalAnalysisImp::~LexicalAnalysisImp() {
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
}
void LexicalAnalysisImp::getch() {
	ch = getc(m_file);
}
bool LexicalAnalysisImp::is_digit(char c) {
	return c >= '0'&&c <= '9';
}
bool LexicalAnalysisImp::is_identifier(char c) {
	return (c == '_') ||
		(c >= 'A'&&c <= 'Z') ||
		(c >= 'a'&&c <= 'z');
}
void LexicalAnalysisImp::preprocess() {
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
void LexicalAnalysisImp::skip_white_space() {
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
void LexicalAnalysisImp::parse_comment() {
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
void LexicalAnalysisImp::parse_identifier() {
	m_TkString->reset();
	m_TkString->append(ch);
	getch();
	while (is_digit(ch) || is_identifier(ch)) {
		m_TkString->append(ch);
		getch();
	}
	m_TkString->append('\0');
}
void LexicalAnalysisImp::parse_num() {
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
void LexicalAnalysisImp::parse_string(char str) {
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
void LexicalAnalysisImp::process() {
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
		tp = m_TkHashTable->insert(m_TkString->str());
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
}
void LexicalAnalysisImp::token_colored(int lex_state) {
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
char* LexicalAnalysisImp::get_tkstr(int token) {
	if (token > m_TkHashTable->array_size())
		return NULL;
	else if (token >= TK_CINT && token <= TK_CSTR)
		return m_SourceString->str();
	else
		return m_TkHashTable->array_str(token);
}
void LexicalAnalysisImp::skip(int c) {
	if (token != c)
		printf("lack of %s\n", get_tkstr(c));
	process();
}

int LexicalAnalysisImp::init() {
	int ret = 0;
	if (m_TkString)
		ret = m_TkString->init(8);
	if (ret != SCP_ERROR_NONE)
		return SCP_INIT_FAILED;
	if (m_SourceString)
		ret = m_SourceString->init(8);
	if (ret != SCP_ERROR_NONE)
		return SCP_INIT_FAILED;
	/*if (m_TkArray)
	ret = m_TkArray->init(256);*/
	if (ret != SCP_ERROR_NONE)
		return SCP_INIT_FAILED;
	if (m_TkHashTable)
		ret = m_TkHashTable->init();
	m_TkHashTable->init_lex();
	if (ret != SCP_ERROR_NONE)
		return SCP_INIT_FAILED;
	return ret;
}
int LexicalAnalysisImp::open(char* file_name) {
	if (file_name == NULL)
		return SCP_INVALID_PARAM;
	m_file = fopen(file_name, "rb+");
	//printf("%d\n", GetLastError());
	if (m_file == NULL)
		return SCP_OPEN_FAILED;
	return SCP_ERROR_NONE;
}
int LexicalAnalysisImp::close() {
	if (m_file == NULL)
		return SCP_CLOSE_FAILED;
	return fclose(m_file) == 0 ? SCP_ERROR_NONE : SCP_CLOSE_FAILED;
}
void LexicalAnalysisImp::run() {
	getch();
	do {
		process();
		token_colored(LEX_NORMAL);
	} while (token != TK_EOF);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut,
		BACKGROUND_GREEN |
		BACKGROUND_INTENSITY);
	printf("\n\n\n代码行数：%d行", line_num);
	LogMessage("\n\n\n代码行数：%d行", line_num);
}

//Log
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