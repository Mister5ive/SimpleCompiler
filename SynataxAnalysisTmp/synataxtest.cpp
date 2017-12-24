#include"synataxtest.h"
#include"stdio.h"

int token;
int tkvalue;
int syntax_state;
int syntax_level;
void external_declaration(int sType);
int type_specifier();
void struct_specifier();
void get_token();
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
		expect("<�������ַ�>");
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

/*
*�������ַ�
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
*�ṹ���ַ�
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
	if (token == TK_BEGIN)				//�ṹ�嶨��
		syntax_state = SC_STATE_LF_HT;
	else if (token == TK_OPENPA)			//sizeof(struct name)
		syntax_state = SC_STATE_NULL;
	else									//�ṹ������
		syntax_state = SC_STATE_SP;
	syntax_indent();

	if (v < TK_IDENT)						//�ؼ��ֲ�����Ϊ�ṹ������
		expect("struct");
	if (token == TK_BEGIN)
		struct_declaration_list();
}


/*
<struct_declaration_list>::= <struct_declaration>{<struct_declaration>}
*/
void struct_declaration_list() {
	int maxalign, offset;

	syntax_state = SC_STATE_LF_HT;	//��һ���ṹ���Ա����һ��
	syntax_level++;					//�ṹ���Ա������������������һ��

	get_token();
	while (token !=TK_END) {
		struct_declaration(&maxalign,&offset);
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
*��������Լ��
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
*�ṹ��Ա����
<struct_member_aligment>::=<KW_ALIGN><TK_OPENPA><TK_CINT><TK_CLOSEPA>
*/

void struct_member_aligment() {
	if (token == KW_ALIGN) {
		get_token();
		skip(TK_OPENPA);
		if (token == TK_CINT)
			get_token();
		else
			expect("���ͳ���");
		skip(TK_CLOSEPA);
	}
}

/*
*������
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
ֱ��������
<direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
*/

void direct_declarator() {
	if (token >= TK_IDENT)
		get_token();
	else
		expect("��ʶ��");
	direct_declarator_postfix();
}

/*
ֱ����������׺
<direct_declarator_postfix>::={<TK_OPENBR><TK_CINT><TK_CLOSEBR>  //[int]
					|<TK_OPENBR><TK_CLOSEBR>                     //[]
					|<TK_OPENPA><parameter_type_list><TK_CLOSEPA> //(...)
					|<TK_OPENPA><TK_CLOSEPA>}						//()
*/

void direct_declarator_postfix() {
	int n;
	if (token == TK_OPENPA)
		parameter_type_list();
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
�����β����ͱ�
<parameter_type_list>::=<type_specifier>{<declarator>}
						{<TK_COMMA><type_specifier>{<declarator>}}<TK_COMMA><TK_ELLIPSIS>
*/


void parameter_type_list(int func_call) {
	get_token();
	while (token != TK_CLOSEPA) {
		if (!type_specifier())
			expect("��Ч���ͱ�ʶ��");
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

/*
*������
<funcbody>::=<compound_statement>
*/

void funcbody() {
	compound_statement();
}

/*
*��ֵ��
<initializer>::=<assignment_expression>
*/

void initializer() {
	assignment_expression();
}

/*
*���
<statement>::=<compound_statement> |<if_statement> |<return_statement>|<break_statement>|<continue_statement>|<for_statement>|<expression_statement>
*/

void statement(int &bsym, int &csym) {
	switch (token)
	{
	case TK_BEGIN:
		compound_statement(bsym.csym);
		break;
	case KW_IF:
		if_statement(bsym.csym);
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
		for_statement(bsym.csym);
		break;
	default:
		expression_statement();
		break;
	}
}

/*
�������
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
�ж��Ƿ�Ϊ�������ַ�
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
���ʽ���
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
ѡ�����
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
for��
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
ע����continue����ѭ����ʱ������Ҳ����ͨ������������׶μ��Դ����������
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
<return_statement>::=
*/