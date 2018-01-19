#ifndef LSCCOMPILER_H
#define LSCCOMPILER_H
#include "stdafx.h"
#include "LSCCompilerAPI.h"
#include "../include/CompilerDef.h"
#include<stack>

/*
*Log
*/

void LogCompile(const char *fmt, ...);
void LogLink(const char *fmt, ...);
void LogWarning(const char *fmt, ...);
void LogError(const char *fmt, ...);
void LogMessage(const char* fmt, ...);
///*
//*动态字符串
//*/
template<typename T>
class SmartString
{
private:
	_SmartString<T> *m_SmartString;
public:
	SmartString();
	~SmartString();
	int init(int initsize);
	int free();
	int reset(int resetsize = 8);
	int realloc(int newsize);
	int append(T ch);
	void print();
	int size();
	int capacity();
	T*  str();
};

/*
*动态数组
*/
template<typename T>
class SmartArray {
public:
	_SmartArray<T> *m_SmartArray;
	int count;
public:
	SmartArray();
	~SmartArray();
	int init(int initsize);
	int reinit(int newsize);
	int add(T *data);
	int research(T ch);
	int search(T *str);
	int free();
	int get_count();
	int size();
	int capacity();
	T* str(int i);
	void print();
};

/*
*单词表
*/

class TkTable {
public:
	_TkWord **m_Word;//hash map
	SmartArray<_TkWord> m_tkTable;//dynamic array
public:
	TkTable();
	~TkTable();
	//init
	int init();
	//uninit
	int free();
	//单词表初始化
	void init_lex();
	//direct insert key
	_TkWord* direct_insert(_TkWord *&tw);
	//find
	_TkWord* find(char *key);
	//direct
	_TkWord* insert(char* key);
	//test
	void print();
	int array_size();
	char* array_str(int token);
private:
	int elf_hash(char *key);
};


//LSC编译器类
class LSCCompilerImp :public LSCCompilerBase {
private:
	void getch();
	bool is_digit(char c);
	bool is_identifier(char c);
	int is_type_specifier(int v);//判断是否为类型区分符
	void preprocess();
	void skip_white_space();
	void parse_comment();
	void parse_identifier();
	void parse_num();
	void parse_string(char str);
	void get_token();//取单词
	void token_colored(int lex_state);
	char* get_tkstr(int token);
	void skip(int c);
	void print_tab(int num);//缩进n个Tab键
	void release();

public:
	LSCCompilerImp();
	~LSCCompilerImp();
	int init();
	int open(char* file_name);
	int close();
	void LexicalAnalysis();
	void SyntaxAnalysis();

	void syntaxAnalysis_unit();
	void syntax_indent();							//语法缩进
public:
	SmartString<char> *m_TkString;					//单词字符串
	SmartString<char> *m_SourceString;				//单词源码字符串
	TkTable *m_TkHashTable;							//单词哈希表
	SmartArray<_TkWord*>   *m_TkArray;				// 单词表中放置标识符，包括变量名,函数名,结构定义名
	FILE *m_file;									//源码文件
	char ch;										//当前从文件中get的到字符
	int token;										//单词编码
	int line_num;									//行号
	int tkvalue;									//单词值
	int syntax_state;								//语法状态
	int syntax_level;								//缩进Tab计数
	std::stack<Symbol> m_global_sym_stack;			//全局符号栈
	std::stack<Symbol> m_local_sym_stack;			//局部符号栈
	Type char_pointer_type;							// 字符串指针				
	Type	int_type;								// int类型
	Type	default_func_type;						// 缺省函数类型

//词法分析函数
private:
	void external_declaration(int sType);//解析外部声明
	int type_specifier(Type *);//类型区分符
	void struct_specifier(Type *);//结构区分符
	void struct_declaration_list(Type *);//
	void struct_declaration(int *maxalign, int *offset, Symbol ***ps);//struct
	void function_calling_convebtion(int &func);//函数调用约定
	void struct_member_aligment();//结构成员对齐
	void declarator();//声明符
	void direct_declarator();//直接声明符
	void direct_declarator_postfix();//直接声明符后缀
	void parameter_type_list(int func_call);//解析形参类型表
	void funcbody();//函数体
	void initializer();//初值符
	void statement();//语句
	void compound_statement();//复合语句
	void expression_statement();//表达式语句
	void if_statement();//if
	void for_statement();//for
	void continue_statement();//continue
	void break_statement();//break
	void return_statement();//return
	void expression();//表达式
	void assignment_expression();//赋值表达式
	void equality_expression();//相等类表达式
	void relational_expression();//关系表达式
	void additive_expression();//加减类
	void multiplicative_expression();//乘除类
	void unary_expression();//一元表达式
	void sizeof_expression();//sizeof()
	void postfix_expression();//后缀表达式
	void primary_expression();//初值表达式
	void argument_expression_list();//实参表达式表
	//	
	Symbol* sym_push_direct(std::stack<Symbol> *,int token,Type *type, int value);
	Symbol* sym_push(int token, Type *type, int reg, int value);
	Symbol* func_sym_push(int token, Type *type);
	Symbol* var_sym_push(Type *type, int reg, int value, int addr);
	Symbol* sec_sym_push(char* sec, int value);
	void sym_pop(std::stack<Symbol> *ptop, Symbol *b);
	Symbol *struct_search(int token);
	Symbol *sym_search(int token);
};
#endif

