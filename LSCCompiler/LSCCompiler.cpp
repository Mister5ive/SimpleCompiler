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
* func:		�����ϣ��ַ
* key:		��ϣ�ؼ���
* MAXKEY:	��ϣ����
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

//��̬�ַ���
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
//��̬����
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

//���ʱ�
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
//���ʱ��ʼ��
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
	//û�У�����
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
		hash = (hash << 4) + (*key++);//hash����4λ����ǰ�ַ�ASCII����hash  
		if ((x = hash & 0xf0000000) != 0) {
			//�����ߵ���λ��Ϊ0����˵���ַ�����7��������������ټӵھŸ��ַ�ʱ����һ���ַ��ᱻ�Ƴ������Ҫ�����´���  
			//�ô���������λΪ0���ͻ����Ӱ��5-8λ�������Ӱ��5-31λ����ΪC����ʹ�õ�������λ  
			//��Ϊ1-4λ�ոմ洢���¼��뵽�ַ������Բ���>>28 
			hash ^= (x >> 24);//���
							  //���28-31λ��  
			hash &= ~x;
		}
	}
	return (hash % TABLEMAX);
}

/***********************************************************
* func:		�����ֽڶ���λ��
* n:		δ����ǰֵ
* align:    �������ȣ����ֽڶ��룩
* ԭ��
����2�㷨ԭ�����£�
2�ֽڶ��룬Ҫ���ַλΪ2,4,6,8...��Ҫ�������λ���һλΪ0��2��1�η���
4�ֽڶ��룬Ҫ���ַλΪ4,8,12,16...��Ҫ�������λ�����λΪ0��2��2�η���
8�ֽڶ��룬Ҫ���ַλΪ8,16,24,32...��Ҫ�������λ�����λΪ0��2��3�η���
16�ֽڶ��룬Ҫ���ַλΪ16,32,48,64...��Ҫ�������λ�����λΪ0��2��4�η���
...
�ɴ˿ɼ�������ֻҪ�����ݲ�����������������ݣ�Ȼ�󽫲���λ��0�Ϳ���ʵ�ֶ�����㡣
��1��(align-1)����ʾ��������Ķ���λ���磺2�ֽڶ���Ϊ1��4�ֽ�Ϊ11��8�ֽ�Ϊ111��16�ֽ�Ϊ1111...
��2��(n+(align-1))����ʾn���������������
��3��&~(align-1)����ʾȥ�����ڲ�����ɵĶ�������
��4�� (n+(align-1))&~(align-1)����ʾ����������

�ٸ����ӣ���8�ֽڶ��롣��ʼ��ʼ��6
6 + ��8 - 1��=0000 0110 + 0000 0111 = 0000 1101
0000 1101 & ~(0000 0111) = 0000 1000  //ȥ�����ڲ�����ɵĶ�������
**********************************************************/
inline unsigned int calc_align(unsigned int n, unsigned int align)
{
	return ((n + align - 1) & (~(align - 1)));
}


//�ʷ�������
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
			//����һ�������Ƿ�Ϊ/*
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
	char tmp;//���ת���ַ�
	m_TkString->reset();
	m_SourceString->reset();
	//	m_SourceString->append(str);
	getch();
	while (1) {
		if (ch == str)//�ַ�������
			break;
		else if (ch == '\\') {//ת���ַ�
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
*  ����:	ȡ����
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
	case '_'://��ʶ��
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
	//�﷨����
	syntax_indent();
#endif
}
void LSCCompilerImp::token_colored(int lex_state) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	char *pOut = NULL;
	switch (lex_state) {
	case LEX_NORMAL:
		if (token >= TK_IDENT)//��ʶ��
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		else if (token >= KW_CHAR)//�ؼ���
			SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN);
		else if (token >= TK_CINT)//����
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN);
		else //����� �ָ���
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
*func:			����n��tab��
*num:			��������
**********************************************************/
void LSCCompilerImp::print_tab(int num) {
	for (int i = 0; i < num; ++i) {
		printf("\t");
	}
}
/***********************************************************
*func:			�������ͳ���
*type:			��������ָ��
*align:			����ֵ
*return:		���ͳ���
**********************************************************/
int  LSCCompilerImp::type_size(Type *type, int *align) {
	Symbol *sym;
	int bt;
	// ָ�����ͳ���Ϊ4���ֽ�
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
* fun:	    ����ָ������
* type:		ԭ��������
**********************************************************/
void LSCCompilerImp::mk_pointer(Type *type)
{
	Symbol *sym;
	sym = sym_push(SC_ANOM, type, 0, -1);
	type->t = T_PTR;
	type->ref = sym;
}
/***********************************************************
* LSCCompiler��ʼ��
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
//�ʷ���������
//ע������ʹ����ע�͵� ��SYNTAXANALYSIS
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
	printf("\n\n\n����������%d\n\n\n", line_num);
	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

	LogMessage("\n\n\n����������%d��", line_num);
}

//�﷨����

/***********************************************************
//�﷨����
//ע����LexicalAnalysis()��ȣ������﷨״̬�ж�
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
		if (token == TK_END)//����},��������һ��
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
//�ʷ�����
**************************************************/

void LSCCompilerImp::SyntaxAnalysis() {

	getch();
	get_token();
	syntaxAnalysis_unit();
}
/**************************************************
//��������ģ��
<syntaxAnalysis_unit>::={external_declaration}<TK_EOF>
**************************************************/
void LSCCompilerImp::syntaxAnalysis_unit() {
	while (token != TK_EOF) {
		external_declaration(SC_GLOBAL);
	}
}

/**************************************************
*func:			�����ⲿ����
*sType:			�洢���ͣ�SC_LOCAL or SC_GLOBAL

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
		LogError("%s, %s ȱ��<�������ַ�> line: %d",__FILE__,__FUNCTION__,__LINE__);
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
				LogError("%s, %s ��֧�ֺ���Ƕ�׶��� line: %d ", __FILE__, __FUNCTION__, __LINE__);
			
			if ((type.t & T_BTYPE) != T_FUNC)
				LogError("%s, %s ȱ�ٺ������� line: %d ", __FILE__, __FUNCTION__, __LINE__);
				
			sym = sym_search(v);
			if (sym)	// ����ǰ�������������ڸ�����������
			{
				if ((sym->type.t & T_BTYPE) != T_FUNC)
					LogError("%s, %s �����ض��� line: %d ", __FILE__, __FUNCTION__, __LINE__);
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
			if ((type.t & T_BTYPE) == T_FUNC) // ��������
			{
				if (sym_search(v) == NULL)
				{
					sym_push(v, &type, SC_GLOBAL | SC_SYM, 0);
				}
			}
			else //��������
			{
				r = 0;
				if (!(type.t & T_ARRAY))
					r |= SC_LVAL;

				r |= sType;
				has_init = (token == TK_ASSIGN);

				if (has_init)
				{
					get_token(); //���ܷŵ����棬char str[]="abc"�������Ҫallocate_storage���ַ�������				    
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
*func:		�������ַ�
*type:		��������
*return:	�Ƿ��ֺϷ����������ַ�

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
*func:		�ṹ�����ַ�
*type:		�ṹ����

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
	if (token == TK_BEGIN)				//�ṹ�嶨��
		syntax_state = SC_STATE_LF_HT;
	else if (token == TK_OPENPA)			//sizeof(struct name)
		syntax_state = SC_STATE_NULL;
	else									//�ṹ������
		syntax_state = SC_STATE_SP;
	syntax_indent();

	if (v < TK_IDENT)						//�ؼ��ֲ�����Ϊ�ṹ������
		LogError("%s, %s �ؼ��ֲ�����Ϊ�ṹ���� line: %d",__FILE__,__FUNCTION__,__LINE__);

	sym = struct_search(token);
	if (!sym) {
		_type.t = KW_STRUCT;
		// -1��ʾ�ṹ��δ����
		sym = sym_push(token | SC_STRUCT,&_type,0,-1);
		sym->reg = 0;
	}
	type->t = T_STRUCT;
	type->ref = sym;

	if (token == TK_BEGIN)
		struct_declaration_list(type);
}


/**************************************************
*func:		�ṹ����������
*type:		�ṹ����
<struct_declaration_list>::= <struct_declaration>{<struct_declaration>}
**************************************************/
void LSCCompilerImp::struct_declaration_list(Type *type) {
	
	int maxalign, offset;
	Symbol *sym, **psym;
	sym = type->ref;

	syntax_state = SC_STATE_LF_HT;	//��һ���ṹ���Ա����һ��
	syntax_level++;					//�ṹ���Ա������������������һ��
	get_token();

	if (sym->value != -1) {// -1��ʾ�ṹ��δ����
		LogError("�ṹ���Ѷ���!");
	}
	maxalign = 1;
	psym = &sym->next;
	offset = 0;
	while (token != TK_END) {
		struct_declaration(&maxalign, &offset, &psym);
	}
	skip(TK_END);
	syntax_state = SC_STATE_LF_HT;

	sym->value = calc_align(offset, maxalign);//�ṹ���С ???
	sym->reg = maxalign; //�ṹ�����
}

/**************************************************
*func:					�ṹ������
* maxalign(����,���):	��Ա����������
* offset(����,���):	ƫ����
* ps(���):				�ṹ�������

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
//��������Լ��
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
*func:			�ṹ��Ա���루__align(n)�� ����ֻ��Ϊ1��2��4
*force_align:	ǿ�ƶ�������
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
			LogError("%s, %s �������ͳ��� line:%d",__FILE__,__FUNCTION__,__LINE__);
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
*func:			����������
*type:			��������
*token:			���ʱ��
*force_align:	ǿ�ƶ�������

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
*func:			ֱ��������
*type:			��������
*token:			���ʱ��
*func_call:		��������Լ��
<direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
**************************************************/

void LSCCompilerImp::direct_declarator(Type *type, int *token, int func_call) {
	if (LSCCompilerImp::token >= TK_IDENT){
		*token = LSCCompilerImp::token;
		get_token();
	}
		
	else {
		LogError("%s, %s ���Ǳ�ʶ��, line:%d",__FILE__,__FUNCTION__,__LINE__);
	}
		
	direct_declarator_postfix(type, func_call);
}

/**************************************************
*func:			ֱ����������׺
*type:			��������
*func_call:		��������Լ��

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
	else if (token == TK_OPENBR) {//�������

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
*func:			�����β����ͱ�
*type:			��������
*func_call:		��������Լ��

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
			LogError("%s, %s ��Ч���ͱ�ʶ�� line:%d",__FILE__,__FUNCTION__,__LINE__);
		
		declarator(&t,&n,NULL);
		sym = sym_push(n | SC_PARAMS, &t, 0, 0);
		//���ã�func(a,b,c) a->b->c
		*plast = sym;//1.first = sym;2֮��. sym->next = sym
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
	if (token == TK_BEGIN)				//��������
		syntax_state = SC_STATE_LF_HT;
	else
		syntax_state = SC_STATE_NULL;	//��������
	syntax_indent();

	// �˴��������������ʹ洢��Ȼ��ָ����������type��Ϊ�������ͣ����õ������Ϣ����ref��
	sym = sym_push(SC_ANOM, type, func_call, 0);
	sym->next = first;
	type->t = T_FUNC;
	type->ref = sym;
}

/**************************************************
*func:			���������
* sym:			��������

<funcbody>::=<compound_statement>
**************************************************/
//???
void LSCCompilerImp::funcbody(Symbol *sym) {

	/*ind = sec_text->data_offset;
	coffsym_add_update(sym, ind, sec_text->index, CST_FUNC, IMAGE_SYM_CLASS_EXTERNAL);*/
	/* ��һ���������ھֲ����ű��� */
	sym_push_direct(&m_local_sym_stack, SC_ANOM, &int_type, 0);
	/*gen_prolog(&sym->type);
	rsym = 0;*/
	compound_statement(NULL,NULL);
	//backpatch(rsym, ind);
	//gen_epilog();
	//sec_text->data_offset = ind;
	sym_pop(&m_local_sym_stack, NULL); /* ��վֲ�����ջ*/
}

/**************************************************
*func:			������ֵ��
*type:			��������
*value:			�������ֵ
*sec:			�������ڽ�

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
*func:			������
*bsym:			break��תλ��
*csym:			continue��תλ��

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
*func:			�������
*bsym:			break��תλ��
*csym:			continue��תλ��

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
*�ж��Ƿ�Ϊ�������ַ�
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
*���ʽ���
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
*func:			if���
*bsym:			break��תλ��
*csym:			continue��תλ��

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
*func:				����for���
*bsym:				break��תλ��
*csym:				continue��תλ��
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
*func:				continue���
*csym:				continue��תλ��

<continue_statement>::=<KW_CONTINUE><TK_SEMICOLON>
ע����continue����ѭ����ʱ������Ҳ����ͨ������������׶μ��Դ����������
**************************************************/

void LSCCompilerImp::continue_statement(int *csym) {

	if (!csym)
		LogError("%S, %s �˴�������continue. line:%d",__FILE__,__FUNCTION__,__LINE__);

	get_token();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);
}

/**************************************************
*func:				break���
*bsym:				break��תλ��
<break_statement>::=<KW_BREAK><TK_SEMICOLON>
**************************************************/
void LSCCompilerImp::break_statement(int *bsym) {

	if (!bsym)
		LogError("%S, %s �˴�������break. line:%d", __FILE__, __FUNCTION__, __LINE__);

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
*���ʽ
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
*��ֵ���ʽ
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
*�������ʽ
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
*��ϵ���ʽ
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
*�Ӽ���
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
*�˳���
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
*һԪ���ʽ
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
*func:			����sizeof

<sizeof_expression>::=<KW_SIZEOF><TK_OPENPA><type_specifier><TK_CLOSEPA>
**************************************************/
void LSCCompilerImp::sizeof_expression() {

	int align, size;
	Type type;

	get_token();
	skip(TK_OPENPA);
	type_specifier(&type);//�������ַ�
	skip(TK_CLOSEPA);

	size = type_size(&type, &align);
	if (size < 0)
		LogError("%s, %s sizeof�������ͳߴ�ʧ�� line: %d",__FILE__,__FUNCTION__,__LINE__);
	//operand_push(&int_type, SC_GLOBAL, size);
}

/**************************************************
��׺���ʽ
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
*func:			��ֵ���ʽ

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
			LogError("%s, %s ȱ�ٱ�ʶ������ line:%d",__FILE__,__FUNCTION__,__LINE__);
		sym = sym_search(t);
		if (!sym) {

			if (token != TK_OPENPA)
				LogError("%s, %s  %sδ���� line:%d", __FILE__, __FUNCTION__, get_tkstr(t),__LINE__);
			
			sym = func_sym_push(t, &default_func_type);//��������������ֱ������
			sym->reg = SC_GLOBAL | SC_SYM;
		}
		r = sym->reg;
		//operand_push(&s->type, r, s->c);
		/* �������ã������������¼���ŵ�ַ */
		//if (optop->r & SC_SYM)
		//{
		//	optop->sym = s;
		//	optop->value = 0;  //���ں������ã���ȫ�ֱ������� printf("g_cc=%c\n",g_cc);
		//}
		break;
	}
}

/**************************************************
ʵ�α��ʽ��
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
//�����ŷ��ڷ���ջ��,��̬�ж��Ƿ���ȫ�ַ���ջ���Ǿֲ�����ջ
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

	// ����¼�ṹ���Ա����������
	//������
	if ((token & SC_STRUCT) || token < SC_ANOM) {
		// ���µ���sym_struct��sym_identifier�ֶ�
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
//���������ŷ���ȫ�ַ��ű���
Symbol* LSCCompilerImp::func_sym_push(int token, Type *type) {
	Symbol *ps, **pps;
	ps = sym_push_direct(&m_global_sym_stack,token,type,0);

	pps = &((_TkWord*)m_TkHashTable->m_tkTable.str(token))->sym_identifier;
	// ͬ�����ţ��������ŷ������-> ->...s 
	while (*pps != NULL){
		pps = &(*pps)->prev_tok;
	}
	ps->prev_tok = NULL;
	*pps = ps;
	return ps;
}

//����������ȫ�ַ��ű���
Symbol* LSCCompilerImp::var_sym_push(Type *type, int reg, int token, int value) {
	Symbol *sym = NULL;
	if ((reg & SC_VALMASK) == SC_LOCAL) {//�ֲ�����
		sym = sym_push(token, type, reg, value);
	}
	else if (token && (reg & SC_VALMASK) == SC_GLOBAL) {//ȫ�ֱ���
		sym = sym_search(token);
		if (sym) {
			LogError("%s, %s %s�ض��� line %d",__FILE__,__FUNCTION__, ((_TkWord*)m_TkHashTable->m_tkTable.str(token))->p_word,__LINE__);
		}
		else {
			sym = sym_push(token,type, reg | SC_SYM,0);
		}
	
	}
	return sym;
}
//�������Ʒ���ȫ�ַ��ű���
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
//����ջ�з���ֱ��ջ������Ϊ'b'
void LSCCompilerImp::sym_pop(std::stack<Symbol> *ptop, Symbol *b) {
	Symbol *s, **ps;
	_TkWord *ts;
	int v;

	s = &ptop->top();
	while (s != b) {
		v = s->token;
		// ���µ��ʱ���sym_struct sym_identifier
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

//���ҽṹ����
Symbol *LSCCompilerImp::struct_search(int token) {
	if (token >= m_TkHashTable->m_tkTable.size())
		return NULL;
	else
		return ((_TkWord*)m_TkHashTable->m_tkTable.str(token))->sym_struct;
}
//���ҽṹ����
Symbol *LSCCompilerImp::sym_search(int token) {
	if (token >= m_TkHashTable->m_tkTable.size())
		return NULL;
	else
		return ((_TkWord*)m_TkHashTable->m_tkTable.str(token))->sym_identifier;
}


/***********************************************************
*func:				����Section
*name:				Section����
*Characteristics:	Section����
*return:			������Section
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
	//IMAGE_SCN_LNK_REMOVE:�˽ڵ㲻���Ϊ�����γɵ�ӳ���ļ���һ���֡�
	//�˱�־ֻ��Ŀ���ļ��Ϸ���ͨ�������־�����жϸýڵ��Ƿ���Ҫ�Ž�ӳ���ļ�
	//�����Ҫ����ӳ���ļ��У�nsec_image��һ
	//�����нڹ�����ɺ�nsec_image������¼��ӳ���ļ��Ľڸ���
	if (!(Characteristics & IMAGE_SCN_LNK_REMOVE))		
		nsec_image++;
	m_section.push_back(sec);
	return sec;
}

/***********************************************************
*func:				��������Ԥ������increment��С���ڴ�ռ�
*sec:				Ԥ���ڴ�ռ�Ľ�
*increment:			Ԥ���Ŀռ��С
*return:			Ԥ���ڴ�ռ���׵�ַ
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
*func:				�����������·����ڴ�,�������ݳ�ʼ��Ϊ0
*sec:				���·����ڴ�Ľ�
*new_size:			�������³���
return:				���ش���ֵ
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
	memset(data + sec->data_allocated, 0, size - sec->data_allocated);/* �·�����ڴ����ݳ�ʼ��Ϊ0 */
	sec->data = data;
	sec->data_allocated = size;
	return SCP_ERROR_NONE;
}

/***********************************************************
* func:					�½��洢COFF���ű�Ľ�
* symtab_name:			COFF���ű���
* Characteristics:		������
* strtab_name:			����ű���ص��ַ�����,�ַ����ڸ��𱣴��������
* return:				�洢COFF���ű�Ľ�
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
* funv:				����COFF����
* symtab:			����COFF���ű�Ľ�
* name:				��������
* val:				�������ص�ֵ
* sec_index:		����˷��ŵĽ�
* type:				Coff��������
* StorageClass:		Coff���Ŵ洢���
* return:			����COFF���ű������
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

		cfsym->Name = csname - strtab->data;//ƫ����
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
* func:				���ӻ����COFF����,����ֻ�����ں�����������������
* sym:				����ָ��
* val:				����ֵ
* sec_index:		����˷��ŵĽ�
* type:				Coff��������
* StorageClass:		Coff���Ŵ洢���
**********************************************************/
void LSCCompilerImp::coffsym_add_update(Symbol *sym, int val, int sec_index, short type, char StorageClass) {

	char *name;
	CoffSym *cfsym;

	if (sym->value){//��������������

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
* func:			����COFF����
* symtab:		����COFF���ű�Ľ�
* name:			��������
* return:		����COFF���ű������
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
		/*�˴������stricmp�ⲿ���õĺ���Դ�����пɲ����ִ�Сд�������ɵ�pe�����õĺ������ƴ�Сд������ȷ��
		����:printf��дΪPrintf�������н����Ȼ����ȷ��,�˴���ô���˿�����Щ���⣬����elf_hash��ϣ��������ĸ
		���ִ�Сд�����printf��PrintF�Ĺ�ϣֵ��ͬ�������������⣬����˴�Ҫ�ģ����뽫��ϣ������Ϊ�����ִ�Сд��
		���ڹ�ϣ�����Ŀ�ͷ��_strupr����һ��
		*/
		if (!strcmp(name, csname))
			return cs;
		cs = cfsym->Next;
	}
	return cs;

}

/***********************************************************
* func:			����COFF�������ַ���
* strtab:		����COFF�ַ�����Ľ�
* name:			���������ַ���
* return:		����COFF�ַ���
**********************************************************/
char* LSCCompilerImp::coffstr_add(Section *strtab, char* name) {

	int len = strlen(name);
	char *pstr = (char*)section_malloc(strtab,len+1);

	memcpy(pstr,name,len);
	return pstr;
}

/***********************************************************
* func:			����COFF�ض�λ��Ϣ
* offset:		��Ҫ�����ض�λ�Ĵ��������������Ӧ�ڵ�ƫ��λ��
* cfsym:		���ű������
* section:		�������ڽ�
* type:			�ض�λ����
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
* func:				�����ض�λ��Ŀ
* section:			�������ڽ�
* sym:				����ָ��
* offset:			��Ҫ�����ض�λ�Ĵ��������������Ӧ�ڵ�ƫ��λ��
* type:				�ض�λ����
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
* func:	COFF��ʼ��
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
* func:				�ӵ�ǰ��дλ�õ�new_posλ����0��ļ�����
* fp:				�ļ�ָ��
* new_pos:			��յ�λ��
* pad:				�����
**********************************************************/
void LSCCompilerImp::fpad(FILE *fp, int new_pos, int pad)
{
	int curpos = ftell(fp);
	while (++curpos <= new_pos)
		fputc(pad, fp);

}

/***********************************************************
* func:	���Ŀ���ļ�
* name:	Ŀ���ļ���
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
* func:			��������ֵ
* t:			��������������
* r:			�������洢����
* value:		������ֵ
**********************************************************/
void LSCCompilerImp::operand_assign(Operand *opd, int t, int r, int v){
	
	opd->type.t = t;
	opd->r = r;
	opd->value = v;
}
void LSCCompilerImp::cancel_lvalue(){

}
/***********************************************************
func:	���Ѱַ������� * ����ָ����Ѱַ�����(Indirection Operator)
**********************************************************/
void LSCCompilerImp::indirection(){

	if ((m_optop.type.t & T_BTYPE) != T_PTR) {
	
		if ((m_optop.type.t & T_BTYPE) == T_FUNC)
			return;
		LogMessage("%s, %s, ȱ��ָ��line: %d",__FILE__,__FUNCTION__,__LINE__);
	}

}
/***********************************************************
* func:		������д��һ���ֽ�
* c:		�ֽ�ֵ
**********************************************************/
void LSCCompilerImp::gen_byte(char c) {

	int l_ind = ind + 1;
	if (l_ind > m_section_data.sec_text->data_allocated)
		section_realloc(m_section_data.sec_text,l_ind);
	m_section_data.sec_text->data[ind] = c;
	ind = l_ind;
}
/***********************************************************
ָ��ǰ׺
ָ��ǰ׺��Ϊ���飬ÿ�鶼��һ������ǰ׺�룺
1���������ظ�ǰ׺
2������д
3��operand-size��д
4��address-size��д

* func:			����ָ��ǰ׺
* opcode:		ָ��ǰ׺����
**********************************************************/
void LSCCompilerImp::gen_prefix(char opcode) {

	gen_byte(opcode);
}
/***********************************************************
* func:			���ɵ��ֽ�ָ��
* opcode:		ָ�����
**********************************************************/
void LSCCompilerImp::gen_opcode1(char opcode) {

	gen_byte(opcode);
}

/***********************************************************
* func:			����˫�ֽ�ָ��
* first:		ָ���һ���ֽ�
* second:		ָ��ڶ����ֽ�
**********************************************************/
void LSCCompilerImp::gen_opcode2(char first, char second) {

	gen_byte(first);
	gen_byte(second);
}
/***********************************************************
ModRM�ṹ��
ModRM.mod [7:6] �ṩѰַģʽ: 11 = register  !11 = memory
ModRM.reg [5:3] �ṩ register ���߶� Opcode ���в���
ModRM.r/m [2:0] �ṩ register ���� memory ������ ModRM.mod

* func:		����ָ��Ѱַ��ʽ�ֽ�ModR/M,
* sym:		����ָ��
* c:		���Ź���ֵ
**********************************************************/
void LSCCompilerImp::gen_modrm(int mod, int reg_opcode, int r_m, Symbol *sym, int c) {
	
	mod <<= 6;
	reg_opcode <<= 3;
	if (mod == 0xc0) {// mod=11 �Ĵ���Ѱַ 89 E5(11 reg_opcode=100ESP r=101EBP) MOV EBP,ESP
		
		gen_byte(mod | reg_opcode | (r_m & SC_VALMASK));
	}
	else if ((r_m & SC_VALMASK) == SC_GLOBAL) {
	
		// mod=00 r=101 disp32  8b 05 50 30 40 00  MOV EAX,DWORD PTR DS:[403050]
		// 8B /r MOV r32,r/m32 Move r/m32 to r32
		// mod=00 r=101 disp32  89 05 14 30 40 00  MOV DWORD PTR DS:[403014],EAX
		// 89 /r MOV r/m32,r32 Move r32 to r/m32
		gen_byte(0x05 | reg_opcode); //ֱ��Ѱַ
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
* func:			����4�ֽڲ�����
* c:			4�ֽڲ�����
**********************************************************/
void LSCCompilerImp::gen_dword(unsigned int c) {

	gen_byte(c);
	gen_byte(c >> 8);
	gen_byte(c >> 16);
	gen_byte(c >> 24);
}
/***********************************************************
* func:			����ȫ�ַ��ŵ�ַ,������COFF�ض�λ��¼
* output constant with relocation if 'r & SC_SYM' is true
* r:			���Ŵ洢����
* sym:			����ָ��
* c:			���Ź���ֵ
**********************************************************/
void LSCCompilerImp::gen_addr32(int r, Symbol *sym, int c) {

	if(r & SC_SYM)//ȫ�ַ����ڴ�������ʱ��֪�����ŵ�ַ�������ض�λ��¼һ��
		coffreloc_add(m_section_data.sec_text, sym, ind, IMAGE_REL_I386_DIR32);
	gen_dword(c);
}

/***********************************************************
* func:			��������opd���ص��Ĵ���r��
* r:			���Ŵ洢����
* opd:			������ָ��
**********************************************************/
void LSCCompilerImp::operand_load(int r, Operand *opd) {

	int v, ft, fc, fr;
	ft = opd->type.t;
	fr = opd->r;
	fc = opd->value;

	v = fr & SC_VALMASK;
	if (fr & SC_LVAL) {//��ֵ
		
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

