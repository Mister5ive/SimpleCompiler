#include"SyntaxStruct.h"




class LexicalAnalysis {
private:
	SmartString<char> *m_TkString;//单词字符串
	SmartString<char> *m_SourceString;//单词源码字符串
	//SmartArray<_TkWord> *m_TkArray;//标识符数组
	TkTable *m_TkHashTable;//单词哈希表
	FILE *m_file;//源码文件
	char ch;//当前从文件中get的到字符
	int token;//单词编码
	int line_num;//行号
	int tkvalue;//单词值
private:
	void getch() {
		ch = getc(m_file);
	}
	bool is_digit(char c) {
		return c >= '0'&&c <= '9';
	}
	bool is_identifier(char c) {
		return (c == '_') ||
			(c >= 'A'&&c <= 'Z') ||
			(c >= 'a'&&c <= 'z');
	}
	void preprocess() {
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
	void skip_white_space() {
		while (ch == ' ' || ch == '\t' || ch == '\r') {
			if (ch == '\r') {
				getch();
				if (ch != '\n')  return;
				line_num++;
			}
			printf("%c",ch);
			getch();
		}
	}
	void parse_comment() {
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

	void parse_identifier() {
		m_TkString->reset();
		m_TkString->append(ch);
		getch();
		while (is_digit(ch) || is_identifier(ch)) {
			m_TkString->append(ch);
			getch();
		}
		m_TkString->append('\0');
	}

	void parse_num() {
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

	void parse_string(char str) {

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
						printf("%s,%s,%d Illegal Escape String:\'\\%c\'\n",tmp, __FILE__, __FUNCTION__, __LINE__);
					else
						printf("%s,%s,%d Illegal Escape String:\'\\0x%x\'\n", tmp, __FILE__, __FUNCTION__, __LINE__);
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
	void process() {
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
				printf("%s,%s,%d error\n", __FILE__, __FUNCTION__, __LINE__);
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
					printf("%s,%s,%d error\n", __FILE__, __FUNCTION__, __LINE__);
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
			printf("%s,%s,%d error\n", __FILE__, __FUNCTION__, __LINE__);
			getch();
			break;
		}
	}
	void token_colored(int lex_state) {
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		char *pOut = NULL;
		switch (lex_state) {
		case LEX_NORMAL:
			if (token >= TK_IDENT)//标识符
				SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
			else if(token >= KW_CHAR)//关键字
				SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN);
			else if (token >= TK_CINT)//常量
				SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN);
			else //运算符 分隔符
				SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
			pOut = get_tkstr(token);
			if (token != TK_EOF)
			{
				if(token == TK_CSTR)
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

	char* get_tkstr(int token) {
		if (token > m_TkHashTable->array_size())
			return NULL;
		else if (token >= TK_CINT && token <= TK_CSTR)
			return m_SourceString->str();
		else
			return m_TkHashTable->array_str(token);
	}
	void skip(int c) {
		if (token != c)
			printf("lack of %s\n",get_tkstr(c));
		process();
	}
public:
	LexicalAnalysis() :token(-1), line_num(1), tkvalue(-1),
						m_file(NULL), ch(EOF){
		m_TkString = new SmartString<char>();
		m_SourceString = new SmartString<char>();
		//m_TkArray = new SmartArray<_TkWord>();
		m_TkHashTable = new TkTable();

		

	}
	~LexicalAnalysis() {
	
	}
	int init() {
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
	int open(char* file_name) {
		if (file_name == NULL)
			return SCP_INVALID_PARAM;
		m_file = fopen(file_name,"rb+");
	
		//printf("%d\n", GetLastError());
		if (m_file == NULL)
			return SCP_OPEN_FAILED;
		return SCP_ERROR_NONE;
	}
	int close() {
		if (m_file == NULL)
			return SCP_CLOSE_FAILED;
			return fclose(m_file) == 0 ? SCP_ERROR_NONE: SCP_CLOSE_FAILED;
	}
	void run() {
		getch();
		do {
			process();
			token_colored(LEX_NORMAL);
		} while (token!=TK_EOF);
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hOut,
			BACKGROUND_GREEN |
			BACKGROUND_INTENSITY);
		printf("\n\n\n代码行数：%d行",line_num);
	}

};
int main(int argc,char** argv)
{
	/*if (argc != 2)
		return -1;
	char* file = argv[1];
	LexicalAnalysis test;
	test.init();
	test.open(file);
	test.run();
	test.close();
	system("pause");*/
	//SmartString Test
	SmartString<char> smartstring;
	smartstring.init(10);
	smartstring.append('w');
	smartstring.append('d');
	smartstring.append('3');
	smartstring.print();
	printf("\n%d\n", smartstring.size());
	printf("%d\n", smartstring.capacity());
	system("pause");
	smartstring.reinit(15);
	smartstring.append('s');
	smartstring.append('d');
	smartstring.print();
	printf("\n%d\n", smartstring.size());
	printf("%d\n", smartstring.capacity());
	system("pause");
	smartstring.reset();
	smartstring.append('1');
	smartstring.append('2');
	smartstring.print();
	printf("\n%d\n", smartstring.size());
	printf("%d\n", smartstring.capacity());
	system("pause");
	smartstring.free();
	printf("\n%d\n", smartstring.size());
	printf("%d\n", smartstring.capacity());
	system("pause");

	//SmartArray Test
	//SmartArray<char> smartarray;
	//smartarray.init(20);
	//smartarray.add("MMP");
	//smartarray.add("NMB");
	//smartarray.add("MLGB");
	//printf("~~~%d\n",smartarray.get_count());
	//printf("%d\n", smartarray.size());
	//printf("%d\n", smartarray.capacity());
	//smartarray.print();
	////system("pause");
	//smartarray.reinit(50);
	//smartarray.add("WQNMLGB");
	//printf("~~~~%d\n", smartarray.get_count());
	//printf("%d\n", smartarray.size());
	//printf("%d\n", smartarray.capacity());
	//smartarray.print();
	////system("pause");
	//printf("%d\n", smartarray.research('N'));
	//printf("***%d\n", smartarray.search("123"));
	////system("pause");

	//smartarray.free();
	//printf("%d\n", smartarray.get_count());
	//printf("%d\n", smartarray.size());
	//printf("%d\n", smartarray.capacity());
	//smartarray.print();
	//system("pause");

	//TkTable tktable;
	//tktable.init();
	//tktable.init_lex();
	//tktable.insert("int");
	//tktable.insert("wc");
	//tktable.insert("wd");
	//tktable.insert("wc");
	//tktable.insert("wd");

	//tktable.print();
	//system("pause");


}