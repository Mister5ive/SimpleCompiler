#ifndef LSCCOMPILER_H
#define LSCCOMPILER_H
#include "stdafx.h"
#include "LSCCompilerAPI.h"
#include "../include/CompilerDef.h"
#include<stack>
#include<vector>
#include<list>
#include <algorithm> 

/*
*Log
*/

void LogCompile(const char *fmt, ...);
void LogLink(const char *fmt, ...);
void LogWarning(const char *fmt, ...);
void LogError(const char *fmt, ...);
void LogMessage(const char* fmt, ...);

//stack 
//template<typename T, class C = std::list<T>>
//class Stack {
//public:
//	typedef typename C::iterator iterator;
//	iterator begin() { return cc.begin(); }
//	iterator end() { return cc.end(); }
//	void push(const T& vaule) { cc.push_front(vaule); }
//	void pop() { cc.pop_front(); }
//	T top() { return cc.front(); }
//	bool empty() { return cc.empty(); }
//	int size() { return cc.size(); }
//	T& operator[](int i) {
//		if (i < size()) {
//			return cc[i];
//		}
//		else
//		{
//			printf("超过下标最大值\n");
//		}
//	}
//	void top_swap() {
//		if(size()>=2)
//			std::swap(cc[0],cc[1]);
//	}
//	
//private:
//	C cc;
//};

//操作数栈类
//template<typename T>
class opStack {
private:
	Operand		m_stack[256];
	Operand*	m_top;

public:
				opStack();
				~opStack();
	void		push(Type *type, int r, int value);
	void		pop();
	void		swap();
	void		cancel_lvalue();
	bool		is_lvalue();
	Operand*	point2StackTop(int index);
	Operand*	point2Stack();
};


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
	_TkWord **m_Word;// 单词哈希表
	SmartArray<_TkWord> m_tkTable;// 单词表中放置标识符，包括变量名,函数名,结构定义名
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
	int  type_size(Type *t, int *a);
	void mk_pointer(Type *t);//生成指针类型
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
	SmartString<char>*								m_TkString;					//单词字符串
	SmartString<char>*								m_SourceString;				//单词源码字符串
	TkTable*										m_TkHashTable;				//单词表
	std::vector<Section*>							m_section;
	FILE*											m_file;						//源码文件
	char											ch;							//当前从文件中get的到字符
	int												token;						//单词编码
	int												line_num;					//行号
	int												tkvalue;					//单词值
	int												syntax_state;				//语法状态
	int												syntax_level;				//缩进Tab计数
	std::stack<Symbol>								m_global_sym_stack;			//全局符号栈
	std::stack<Symbol>								m_local_sym_stack;			//局部符号栈
	Type											char_pointer_type;			// 字符串指针				
	Type											int_type;					// int类型
	Type											default_func_type;			// 缺省函数类型
	int												nsec_image;					// 映像文件节个数
	SectionData										m_section_data;				// 节数据
	//Operand  										m_optop;					// 操作数栈栈顶
	//std::list<Operand>::iterator					m_optop;
	//Stack<Operand>								m_opstack;					// 操作数栈
	opStack											m_opstack;
	int												rsym;						// 记录return指令位置
	int												ind ;						// 指令在代码节位置
	int												loc;						// 局部变量在栈中位置
	int												func_begin_ind;				// 函数开始指令
	int												func_ret_sub;				// 函数返回释放栈空间大小
	Symbol*											sym_sec_rdata;				// 只读节符号
//词法分析函数
private:
	void external_declaration(int sType);//解析外部声明
	int type_specifier(Type *);//类型区分符
	void struct_specifier(Type *);//结构区分符
	void struct_declaration_list(Type *);//
	void struct_declaration(int *maxalign, int *offset, Symbol ***ps);//struct
	void function_calling_convebtion(int &func);//函数调用约定
	void struct_member_aligment(int*);//结构成员对齐
	void declarator(Type *type, int *token, int *force_align);//声明符
	void direct_declarator(Type *type, int *token, int func_call);//直接声明符
	void direct_declarator_postfix(Type *type, int func_call);//直接声明符后缀
	void parameter_type_list(Type *type, int func_call);//解析形参类型表
	void funcbody(Symbol *sym);//函数体
	void initializer(Type *type, int c, Section *sec);//初值符
	void statement(int *bsym, int *csym);//语句
	void compound_statement(int *bsym, int *csym);//复合语句
	void expression_statement();//表达式语句
	void if_statement(int *bsym, int *csym);//if
	void for_statement(int *bsym, int *csym);//for
	void continue_statement(int* csym);//continue
	void break_statement(int* bsym);//break
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
	//sec
	Section* section_create(char *name, int Characteristics);
	void *section_malloc(Section *sec, int increment);
	int section_realloc(Section *sec, int new_size);
	Section *coffsym_section_create(char *symtab_name, int Characteristics, char *strtab_name);
	int coffsym_add(Section *symtab, char* name, int val, int sec_index,short type, char StorageClass);
	int coffsym_search(Section *symtab, char *name);
	char *coffstr_add(Section *strtab, char* name);
	void coffsym_add_update(Symbol *s, int val, int sec_index, short type, char StorageClass);
	void coffreloc_add(Section *sec, Symbol *sym, int offset, char type);
	void coffreloc_direct_add(int offset, int cfsym, char section, char type);
	void coff_init();
	void fpad(FILE *fp, int new_pos, int pad);
	void write_obj(char *name);

	//oper
	void operand_assign(Operand *opd, int t, int r, int v);
	void indirection();

	//gencode
	void gen_byte(char c);
	void gen_prefix(char opcode);
	void gen_opcode1(char opcode);
	void gen_opcode2(char first, char second);
	void gen_modrm(int mod, int reg_opcode, int r_m, Symbol *sym, int c);
	void gen_dword(unsigned int c);
	void gen_addr32(int r, Symbol *sym, int c);
	void operand_load(int r, Operand *opd);
	int stack_top_load1(int rc, Operand *opd);
	void stack_top_load2(int rc1, int rc2);
	void operand_store(int rc, Operand *opd);
	void stack_store();
	int	 allocate_reg(int rc);
	void spill_reg(int r);
	void spill_regs();

	void gen_op(int op);


};
#endif

