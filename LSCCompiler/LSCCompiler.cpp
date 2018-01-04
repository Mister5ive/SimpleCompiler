#include "stdafx.h"
#include "LSCCompiler.h"


static LogFunction	g_LogMsg = NULL;
static LogFunction	g_LogError = NULL;
static LogFunction	g_LogWarning = NULL;
static LogFunction	g_LogLink = NULL;
static LogFunction	g_LogCompile = NULL;

LSCCompilerBase* LSCCompilerImpCreate(void) {
	LSCCompilerImp* pInstance = new LSCCompilerImp();
	if (pInstance)
		return pInstance;
}
void LSCCompilerImpDestroy(LSCCompilerBase* pInstance) {
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

//�ʷ�������
/*************************************************************************************************************/
LSCCompilerImp::LSCCompilerImp() :token(-1), line_num(1), tkvalue(-1),
m_file(NULL), ch(EOF), syntax_state(SC_STATE_NULL), syntax_level(0){
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
void LSCCompilerImp::print_tab(int num) {
	for (int i = 0; i < num; ++num) {
		printf("\t");
	}
}

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

//�﷨����
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
	{
		if (token == TK_END)//����},��������һ��
			syntax_level--;
		printf("\n");
		print_tab(syntax_level);
	}
	token_colored(LEX_NORMAL);
	break;
	case SC_STATE_DELAY:
		break;
	}
	syntax_state = SC_STATE_NULL;
}
/**************************************************
<translate>::={external_declaration}<TK_EOF>
**************************************************/
void LSCCompilerImp::translation_unit() {
	while (token != TK_EOF) {
		external_declaration(SC_GLOBAL);
	}
}

/**************************************************
<external_declaration>::=
<type_specifier>(<TK_SEMICOLON>
|<declaration><funcbody>
|<declaration>[<TK_ASSIGN><initializer>]
{<TK_COMMA><declaration>[<TK_ASSIGN><initializer>]}<TK_SEMICOLON>)
<function_definition>|<declaration>

**************************************************/
void LSCCompilerImp::external_declaration(int sType) {
	if (!type_specifier()) {
		printf("<�������ַ�>");
	}
	if (token == TK_SEMICOLON) {
		get_token();
		return;
	}
	while (1) {
		declarator();
		if (token == TK_BEGIN) {
			if (sType == SC_LOCAL)
				printf("��֧�ֺ���Ƕ�׶���");
			funcbody();
			break;
		}
		else {
			if (token == TK_ASSIGN) {
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
			}
		}
	}
}

/**************************************************
*�������ַ�
<type_specifier>::=<KW_INT> |<KW_CHAR>|<KW_SHORT>|<KW_VOID>|<struct_specifier>
**************************************************/

int LSCCompilerImp::type_specifier() {
	int type = 0;
	switch (token) {
	case KW_CHAR:
	case KW_INT:
	case KW_SHORT:
	case KW_VOID:
		type = 1;
		syntax_state = SC_STATE_SP;
		get_token();
		break;
	case KW_STRUCT:
		syntax_state = SC_STATE_SP;
		struct_specifier();
		type = 1;
		break;
	default:
		break;
	}
	return type;
}

/**************************************************
*�ṹ���ַ�
<struct_specifier>::=
<KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END>
|<KW_STRUCT><IDENTIFIER>
**************************************************/

void LSCCompilerImp::struct_specifier() {
	int v;
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
		printf("struct");
	if (token == TK_BEGIN)
		struct_declaration_list();
}


/**************************************************
*�ṹ���ַ�
<struct_declaration_list>::= <struct_declaration>{<struct_declaration>}
**************************************************/
void LSCCompilerImp::struct_declaration_list() {
	//int maxalign, offset;

	syntax_state = SC_STATE_LF_HT;	//��һ���ṹ���Ա����һ��
	syntax_level++;					//�ṹ���Ա������������������һ��

	get_token();
	while (token != TK_END) {
		//struct_declaration(&maxalign, &offset);
		struct_declaration();
	}
	skip(TK_END);
	syntax_state = SC_STATE_LF_HT;
}

/**************************************************
<struct_declaration>::=<type_specifier><declarator>{<TK_COMMA><declarator>}<TK_SEMICOLON>
**************************************************/

void LSCCompilerImp::struct_declaration() {
	type_specifier();
	while (1) {
		declarator();
		if (token == TK_SEMICOLON)
			break;
		skip(TK_COMMA);
	}
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);
}

/**************************************************
*��������Լ��
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
*�ṹ��Ա����
<struct_member_aligment>::=<KW_ALIGN><TK_OPENPA><TK_CINT><TK_CLOSEPA>
**************************************************/

void LSCCompilerImp::struct_member_aligment() {
	if (token == KW_ALIGN) {
		get_token();
		skip(TK_OPENPA);
		if (token == TK_CINT)
			get_token();
		else
			printf("���ͳ���");
		skip(TK_CLOSEPA);
	}
}

/**************************************************
*������
<declarator>::={<TK_STAR>}[<struct_member_aligment>][<struct_member_aligment>]<direct_declarator>
**************************************************/

void LSCCompilerImp::declarator() {
	int func;
	while (token == TK_STAR) {
		get_token();
	}
	function_calling_convebtion(func);
	struct_member_aligment();
	direct_declarator();
}

/**************************************************
ֱ��������
<direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
**************************************************/

void LSCCompilerImp::direct_declarator() {
	if (token >= TK_IDENT)
		get_token();
	else
		printf("��ʶ��");
	direct_declarator_postfix();
}

/**************************************************
ֱ����������׺
<direct_declarator_postfix>::={<TK_OPENBR><TK_CINT><TK_CLOSEBR>  //[int]
|<TK_OPENBR><TK_CLOSEBR>                     //[]
|<TK_OPENPA><parameter_type_list><TK_CLOSEPA> //(...)
|<TK_OPENPA><TK_CLOSEPA>}						//()
**************************************************/

void LSCCompilerImp::direct_declarator_postfix() {
	int n = -1;
	if (token == TK_OPENPA)
		parameter_type_list(n);// may wrong!!!!!!!!!!
	else if (token == TK_OPENBR) {
		get_token();
		if (token == TK_CINT) {
			get_token();
			n = tkvalue;
		}
		skip(TK_CLOSEBR);
		direct_declarator_postfix();
	}
}

/**************************************************
�����β����ͱ�
<parameter_type_list>::=<type_specifier>{<declarator>}
{<TK_COMMA><type_specifier>{<declarator>}}<TK_COMMA><TK_ELLIPSIS>
**************************************************/


void LSCCompilerImp::parameter_type_list(int func_call) {
	get_token();
	while (token != TK_CLOSEPA) {
		if (!type_specifier())
			printf("��Ч���ͱ�ʶ��");
		declarator();
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

}

/**************************************************
*������
<funcbody>::=<compound_statement>
**************************************************/

void LSCCompilerImp::funcbody() {
	compound_statement();
}

/**************************************************
*��ֵ��
<initializer>::=<assignment_expression>
**************************************************/

void LSCCompilerImp::initializer() {
	assignment_expression();
}

/**************************************************
*���
<statement>::=<compound_statement> |<if_statement> |<return_statement>|<break_statement>|<continue_statement>|<for_statement>|<expression_statement>
**************************************************/

void LSCCompilerImp::statement()
{
	switch (token)
	{
	case TK_BEGIN:
		compound_statement();
		break;
	case KW_IF:
		if_statement();
		break;
	case KW_RETURN:
		return_statement();
		break;
	case KW_BREAK:
		break_statement();
		break;
	case KW_CONTINUE:
		continue_statement();
		break;
	case KW_FOR:
		for_statement();
		break;
	default:
		expression_statement();
		break;
	}
}

/**************************************************
*�������
<compound_statement>::=<TK_BEGIN>{<declaration>}{statement}<TK_END>
**************************************************/

void LSCCompilerImp::compound_statement() {
	syntax_state = SC_STATE_LF_HT;
	syntax_level++;

	get_token();
	while (is_type_specifier(token)) {
		external_declaration(SC_LOCAL);
	}
	while (token != TK_END) {
		statement();
	}
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
*ѡ�����
<if_statement>::=<KW_IF><TK_OPENPA><expression><TK_CLOSEPA>
<statement>[<KW_ELSE><statement>]
**************************************************/

void LSCCompilerImp::if_statement() {
	syntax_state = SC_STATE_SP;
	get_token();
	skip(TK_OPENPA);
	expression();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_CLOSEPA);
	statement();
	if (token == KW_ELSE) {
		syntax_state = SC_STATE_LF_HT;
		get_token();
		statement();
	}
}

/**************************************************
*for
<for_statement>::=<KW_FOR><TK_OPENPA><expression_statement><expression_statement><expression><TK_CLOSEPA><statement>
**************************************************/

void LSCCompilerImp::for_statement() {
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
	statement();
}

/**************************************************
*continue
<continue_statement>::=<KW_CONTINUE><TK_SEMICOLON>
ע����continue����ѭ����ʱ������Ҳ����ͨ������������׶μ��Դ����������
**************************************************/

void LSCCompilerImp::continue_statement() {
	get_token();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);
}

/**************************************************
break
<break_statement>::=<KW_BREAK><TK_SEMICOLON>
**************************************************/
void LSCCompilerImp::break_statement() {
	get_token();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);

}

/**************************************************
return
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
sizeof()
<sizeof_expression>::=<KW_SIZEOF><TK_OPENPA><type_specifier><TK_CLOSEPA>
**************************************************/
void LSCCompilerImp::sizeof_expression() {
	get_token();
	skip(TK_OPENPA);
	type_specifier();//�������ַ�
	skip(TK_CLOSEPA);
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
��ֵ���ʽ
<primary_expression>::=<IDENTIFIER> | <TK_CINT>
| <TK_CSTR> | <TK_CCHAR> | <TK_OPENPA><expression><TK_CLOSEPA>
**************************************************/
void LSCCompilerImp::primary_expression() {
	int t;
	switch (token) {
	case TK_CINT:
	case TK_CCHAR:
		get_token();
		break;
	case TK_CSTR:
		get_token();
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
			printf("��ʶ������");
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