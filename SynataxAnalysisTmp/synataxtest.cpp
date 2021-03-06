#include"synataxtest.h"
#include"stdio.h"
#include <windows.h>

int token;
int tkvalue;
int syntax_state;
int syntax_level;
void external_declaration(int sType);
int type_specifier();
void struct_specifier();

void token_colored(int lex_state) {
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

void get_token() {

	syntax_indent();

}

void skip(int ch) {
	if (token != ch)
		printf("缺少%d",ch);
	get_token();
}
//语法缩进
void syntax_indent() {
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
		if (token == TK_END)//遇到},缩进减少一级
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

//缩进n个Tab键
void print_tab(int num) {
	for (int i = 0; i < num;++num) {
		printf("\t");
	}
}

/*
<translate>::={external_declaration}<TK_EOF>
*/
void translation_unit() {
	while (token != TK_EOF) {
		external_declaration(SC_GLOBAL);
	}
}

/*
<external_declaration>::=
<type_specifier>(<TK_SEMICOLON>
	|<declaration><funcbody>
	|<declaration>[<TK_ASSIGN><initializer>]
	{<TK_COMMA><declaration>[<TK_ASSIGN><initializer>]}<TK_SEMICOLON>)
<function_definition>|<declaration>

*/
void external_declaration(int sType){
	if (!type_specifier()) {
		printf("<类型区分符>");
	}
	if (token == TK_SEMICOLON) {
		get_token();
		return;
	}
	while (1) {
		declarator();
		if (token == TK_BEGIN) {
			if (sType == SC_LOCAL)
				printf("不支持函数嵌套定义");
			funcbody();
			break;
		}
		else {
			if (token == TK_ASSIGN) {
				get_token();
				initializer();
			}
			if(token == TK_COMMA) {
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

/*
*类型区分符
<type_specifier>::=<KW_INT> |<KW_CHAR>|<KW_SHORT>|<KW_VOID>|<struct_specifier>
*/

int type_specifier() {
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

/*
*结构区分符
<struct_specifier>::=
	<KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END>
	|<KW_STRUCT><IDENTIFIER>
*/

void struct_specifier() {
	int v;
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
		printf("struct");
	if (token == TK_BEGIN)
		struct_declaration_list();
}


/*
<struct_declaration_list>::= <struct_declaration>{<struct_declaration>}
*/
void struct_declaration_list() {
	//int maxalign, offset;

	syntax_state = SC_STATE_LF_HT;	//第一个结构体成员另起一行
	syntax_level++;					//结构体成员变量声明，缩进增加一级

	get_token();
	while (token !=TK_END) {
		//struct_declaration(&maxalign, &offset);
		struct_declaration();
	}
	skip(TK_END);
	syntax_state = SC_STATE_LF_HT;
}

/*
<struct_declaration>::=<type_specifier><declarator>{<TK_COMMA><declarator>}<TK_SEMICOLON>
*/

void struct_declaration() {
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

/*
*函数调用约定
<function_calling_convebtion>::=<KW_CDECL>|<KW_STDCALL>
*/

void function_calling_convebtion(int &func) {
	func = KW_CDECL;
	if (token == KW_CDECL || token == KW_STDCALL) {
		func = token;
		syntax_state = SC_STATE_SP;
		get_token();
	}
}

/*
*结构成员对齐
<struct_member_aligment>::=<KW_ALIGN><TK_OPENPA><TK_CINT><TK_CLOSEPA>
*/

void struct_member_aligment() {
	if (token == KW_ALIGN) {
		get_token();
		skip(TK_OPENPA);
		if (token == TK_CINT)
			get_token();
		else
			printf("整型常量");
		skip(TK_CLOSEPA);
	}
}

/*
*声明符
<declarator>::={<TK_STAR>}[<struct_member_aligment>][<struct_member_aligment>]<direct_declarator>
*/

void declarator() {
	int func;
	while (token == TK_STAR) {
		get_token();
	}
	function_calling_convebtion(func);
	struct_member_aligment();
	direct_declarator();
}

/*
直接声明符
<direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
*/

void direct_declarator() {
	if (token >= TK_IDENT)
		get_token();
	else
		printf("标识符");
	direct_declarator_postfix();
}

/*
直接声明符后缀
<direct_declarator_postfix>::={<TK_OPENBR><TK_CINT><TK_CLOSEBR>  //[int]
					|<TK_OPENBR><TK_CLOSEBR>                     //[]
					|<TK_OPENPA><parameter_type_list><TK_CLOSEPA> //(...)
					|<TK_OPENPA><TK_CLOSEPA>}						//()
*/

void direct_declarator_postfix() {
	int n;
	if (token == TK_OPENPA)
		parameter_type_list(n);// may wrong
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

/*
解析形参类型表
<parameter_type_list>::=<type_specifier>{<declarator>}
						{<TK_COMMA><type_specifier>{<declarator>}}<TK_COMMA><TK_ELLIPSIS>
*/


void parameter_type_list(int func_call) {
	get_token();
	while (token != TK_CLOSEPA) {
		if (!type_specifier())
			printf("无效类型标识符");
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
	if (token == TK_BEGIN)				//函数定义
		syntax_state = SC_STATE_LF_HT;
	else
		syntax_state = SC_STATE_NULL;	//函数声明
	syntax_indent();

}

/*
*函数体
<funcbody>::=<compound_statement>
*/

void funcbody() {
	compound_statement();
}

/*
*初值符
<initializer>::=<assignment_expression>
*/

void initializer() {
	assignment_expression();
}

/*
*语句
<statement>::=<compound_statement> |<if_statement> |<return_statement>|<break_statement>|<continue_statement>|<for_statement>|<expression_statement>
*/

void statement()
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

/*
复合语句
<compound_statement>::=<TK_BEGIN>{<declaration>}{statement}<TK_END>
*/

void compound_statement() {
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

/*
判断是否为类型区分符
*/

int is_type_specifier(int v) {
	switch (v) {
	case KW_CHAR:
	case KW_INT:
	case KW_SHORT:
	case KW_VOID:
	case KW_STRUCT:
		return 1;
	default:
		return;
	}
	return 0;
}

/*
表达式语句
<expression_statement>::=<TK_SEMICOLON>|<expression><TK_SEMICOLON>
*/

void expression_statement() {
	
	while (token != TK_SEMICOLON){
		expression();
	}
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);
}

/*
选择语句
<if_statement>::=<KW_IF><TK_OPENPA><expression><TK_CLOSEPA>
				<statement>[<KW_ELSE><statement>]
*/

void if_statement(){
	syntax_state = SC_STATE_SP;
	get_token();
	skip(TK_OPENPA);
	expression();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_CLOSEPA);
	statement();
	if (token == KW_ELSE){
		syntax_state = SC_STATE_LF_HT;
		get_token();
		statement();
	}
}

/*
for：
<for_statement>::=<KW_FOR><TK_OPENPA><expression_statement><expression_statement><expression><TK_CLOSEPA><statement>
*/

void for_statement(){
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

/*
continue
<continue_statement>::=<KW_CONTINUE><TK_SEMICOLON>
注：当continue不在循环内时，编译也可以通过，语义分析阶段加以处理这种情况
*/

void continue_statement(){
	get_token();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);
}

/*
break
<break_statement>::=<KW_BREAK><TK_SEMICOLON>
*/
void break_statement(){
	get_token();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);

}

/*
return
<return_statement>::=<KW_RETURN><TK_SEMICOLON>|
				<KW_RETURN><expression><TK_SEMICOLON>
*/

void return_statement() {
	syntax_state = SC_STATE_DELAY;
	get_token();
	if(token == TK_SEMICOLON)
		syntax_state = SC_STATE_LF_HT;
	else
		syntax_state = SC_STATE_SP;
	syntax_indent();////////?????????

	if (token != TK_SEMICOLON)
		expression();
	syntax_state = SC_STATE_LF_HT;
	skip(TK_SEMICOLON);

}

/*
表达式
<expression>::=<assignment_expression>{<TK_COMMA><assignment_expression>}
*/

void expression() {
	while (1) {
		assignment_expression();
		if (token != TK_COMMA)
			break;
		get_token();
	}
}

/*
赋值表达式
<assignment_expression>::=<equality_expression>
					{<TK_ASSIGN><assignment_expression>};
*/
void assignment_expression() {
	equality_expression();
	if (token == TK_ASSIGN) {
		get_token();
		assignment_expression();
	}
}

/*
相等类表达式
<equality_expression>::=<relational_expression>
		{<TK_EQ><relational_expression>
		 | <TK_NEQ><relational_expression>}
*/

void equality_expression() {
	relational_expression();
	while (token == TK_EQ || token == TK_NEQ) {
		get_token();
		relational_expression();
	}
}

/*
关系表达式
	<relational_expression>::=<additive_expression>{
		<TK_LT><additive_expression>
		|<TK_GT><additive_expression>
		|<TK_LEQ><additive_expression>
		|<TK_GEQ><additive_expression>
	}
*/

void relational_expression() {
	additive_expression();
	while (token == TK_LT || token == TK_LT || token == TK_LT || token == TK_LT) {
		get_token();
		additive_expression();
	}
}

/*
加减类
<additive_expression>::=<multiplicative_expression>{
	<TK_PLUS><multiplicative_expression>
	<TK_MINUS><multiplicative_expression>
}
*/
void additive_expression() {
	multiplicative_expression();
	while (token == TK_PLUS || token == TK_MINUS) {
		get_token();
		multiplicative_expression();
	}
}

/*
乘除类
<multiplicative_expression>::=<unary_expression>{
	<TK_STAR><unary_expression>
	|<TK_DIVIDE><unary_expression>
	|<TK_MOD><unary_expression>
}
*/

void multiplicative_expression() {
	unary_expression();
	while (token == TK_STAR || token == TK_DIVIDE || token == TK_MOD){
		get_token();
		unary_expression();
	}
}

/*
一元表达式
<unary_expression>::=<postfix_expression>
		|<TK_AND><unary_expression>
		|<TK_STAR><unary_expression>
		|<TK_PLUS><unary_expression>
		|<TK_MINUS><unary_expression>
		|<sizeof_expression>
*/

void unary_expression() {
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

/*
sizeof()
<sizeof_expression>::=<KW_SIZEOF><TK_OPENPA><type_specifier><TK_CLOSEPA>
*/
void sizeof_expression() {
	get_token();
	skip(TK_OPENPA);
	type_specifier();//类型区分符
	skip(TK_CLOSEPA);
}

/*
后缀表达式
<postfix_expression>::=<primary_expression>
	{<TK_OPENPA><TK_CLOSEPA>
	|<TK_OPENBR><expression><TK_CLOSEBR>
	|<TK_OPENPA><argument_expression_list><TK_CLOSEPA>
	|<TK_DOT><IDENTIFIER>
	|<TK_POINTSTO><IDENTIFIER>
	}
*/

void postfix_expression() {
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

/*
初值表达式
<primary_expression>::=<IDENTIFIER> | <TK_CINT>
| <TK_CSTR> | <TK_CCHAR> | <TK_OPENPA><expression><TK_CLOSEPA>
*/

void primary_expression() {
	int t;
	switch (token){
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
			printf("标识符或常量");
		break;
	}
}

/*
实参表达式表
<argument_expression_list>::=<assignment_expression>
	{<TK_COMMA><assignment_expression>}
*/

void argument_expression_list() {
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
