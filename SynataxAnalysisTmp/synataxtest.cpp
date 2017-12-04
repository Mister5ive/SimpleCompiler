#include"synataxtest.h"
#include"stdio.h"

int token;
void external_declaration(int sType);
/*
<translate>::={external_declaration}<TK_EOF>
*/
void translate() {
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
		expect("<类型区分符>");
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
				syntax_state = SNTX_LF_HT;
				skip(TK_SEMICOLON);
				break;
			}
		}
	}
}