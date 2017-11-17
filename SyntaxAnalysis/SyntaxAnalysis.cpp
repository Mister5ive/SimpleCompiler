#include"SyntaxStruct.h"
//{}
int token;
int line_num = 0;
char ch;
void skip_white_space();
void getch();
void parse_comment();
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
				ungetc(ch, fin);
				ch = '/';
				break;
			}
		}
		else
			break;
		
	}
}
void skip_white_space() {
	while(ch == ' '|| ch =='\t' || ch == '\r') {
		if(ch == '\r') {
			getch();
			if (ch != '\n')  return;
			line_num++;
		}
		//printf();
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
		if(ch =='*') {
			getch();
			if(ch == '/') {
				getch();
				return;
			}
		}
		else if(ch =='\n') {
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
	
}
void process() {
	preprocess();
	switch(ch) {
	case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
	case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':
	case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':
	case 's':case 't':case 'u':case 'v':case 'w':case 'x':
	case 'y':case 'z':
	case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
	case 'G':case 'H':case 'I':case 'G':case 'K':case 'L':
	case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':
	case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':
	case 'Y':case 'Z':
	case '_'://标识符
	{
		break;
	}
	case '0':case '1':case '2':case '3':case '4':case '5':
	case '6':case '7':case '8':case '9':
	{
		parse_num();
		token = TK_CINT;
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
		if(ch == '=') {
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
				printf();
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
		token = TK_OPENPA;
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
	case '\':
	{
		token = TK_BEGIN;
		getch();
		break;
	}
	case EOF:
	{
		token = TK_EOF;
		break;
	}
	default:
		error();
		getch();
		break;
	}
}
int main()
{
	//SmartString Test
	/*SmartString<char> smartstring;
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
	system("pause");*/

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

	/*TkTable tktable;
	tktable.init();
	tktable.init_lex();
	tktable.insert("wc");
	tktable.insert("wc");
	tktable.insert("wd");
	tktable.insert("wc");
	tktable.insert("wd");

	tktable.print();
	system("pause");*/


}