#ifndef LSCCOMPILER_H
#define LSCCOMPILER_H
#include "stdafx.h"
#include "LSCCompilerAPI.h"
#include "../include/CompilerDef.h"
#include<stack>
#include<vector>

/*
*Log
*/

void LogCompile(const char *fmt, ...);
void LogLink(const char *fmt, ...);
void LogWarning(const char *fmt, ...);
void LogError(const char *fmt, ...);
void LogMessage(const char* fmt, ...);
///*
//*��̬�ַ���
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
*��̬����
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
*���ʱ�
*/

class TkTable {
public:
	_TkWord **m_Word;// ���ʹ�ϣ��
	SmartArray<_TkWord> m_tkTable;// ���ʱ��з��ñ�ʶ��������������,������,�ṹ������
public:
	TkTable();
	~TkTable();
	//init
	int init();
	//uninit
	int free();
	//���ʱ��ʼ��
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


//LSC��������
class LSCCompilerImp :public LSCCompilerBase {
private:
	void getch();
	bool is_digit(char c);
	bool is_identifier(char c);
	int is_type_specifier(int v);//�ж��Ƿ�Ϊ�������ַ�
	void preprocess();
	void skip_white_space();
	void parse_comment();
	void parse_identifier();
	void parse_num();
	void parse_string(char str);
	void get_token();//ȡ����
	void token_colored(int lex_state);
	char* get_tkstr(int token);
	void skip(int c);
	void print_tab(int num);//����n��Tab��
	int  type_size(Type *t, int *a);
	void mk_pointer(Type *t);//����ָ������
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
	void syntax_indent();							//�﷨����
public:
	SmartString<char>*								m_TkString;					//�����ַ���
	SmartString<char>*								m_SourceString;				//����Դ���ַ���
	TkTable*										m_TkHashTable;				//���ʱ�
	std::vector<Section*>							m_section;
	FILE*											m_file;						//Դ���ļ�
	char											ch;							//��ǰ���ļ���get�ĵ��ַ�
	int												token;						//���ʱ���
	int												line_num;					//�к�
	int												tkvalue;					//����ֵ
	int												syntax_state;				//�﷨״̬
	int												syntax_level;				//����Tab����
	std::stack<Symbol>								m_global_sym_stack;			//ȫ�ַ���ջ
	std::stack<Symbol>								m_local_sym_stack;			//�ֲ�����ջ
	Type											char_pointer_type;			// �ַ���ָ��				
	Type											int_type;					// int����
	Type											default_func_type;			// ȱʡ��������
	int												nsec_image;					// ӳ���ļ��ڸ���
	SectionData										m_section_data;				//������
	Operand  										m_optop;					    // ������ջջ��
	std::stack<Operand>								m_opstack;					// ������ջ
	int												rsym;						// ��¼returnָ��λ��
	int												ind ;						// ָ���ڴ����λ��
	int												loc;						// �ֲ�������ջ��λ��
	int												func_begin_ind;				// ������ʼָ��
	int												func_ret_sub;				// ���������ͷ�ջ�ռ��С
	Symbol*											sym_sec_rdata;				// ֻ���ڷ���
//�ʷ���������
private:
	void external_declaration(int sType);//�����ⲿ����
	int type_specifier(Type *);//�������ַ�
	void struct_specifier(Type *);//�ṹ���ַ�
	void struct_declaration_list(Type *);//
	void struct_declaration(int *maxalign, int *offset, Symbol ***ps);//struct
	void function_calling_convebtion(int &func);//��������Լ��
	void struct_member_aligment(int*);//�ṹ��Ա����
	void declarator(Type *type, int *token, int *force_align);//������
	void direct_declarator(Type *type, int *token, int func_call);//ֱ��������
	void direct_declarator_postfix(Type *type, int func_call);//ֱ����������׺
	void parameter_type_list(Type *type, int func_call);//�����β����ͱ�
	void funcbody(Symbol *sym);//������
	void initializer(Type *type, int c, Section *sec);//��ֵ��
	void statement(int *bsym, int *csym);//���
	void compound_statement(int *bsym, int *csym);//�������
	void expression_statement();//���ʽ���
	void if_statement(int *bsym, int *csym);//if
	void for_statement(int *bsym, int *csym);//for
	void continue_statement(int* csym);//continue
	void break_statement(int* bsym);//break
	void return_statement();//return
	void expression();//���ʽ
	void assignment_expression();//��ֵ���ʽ
	void equality_expression();//�������ʽ
	void relational_expression();//��ϵ���ʽ
	void additive_expression();//�Ӽ���
	void multiplicative_expression();//�˳���
	void unary_expression();//һԪ���ʽ
	void sizeof_expression();//sizeof()
	void postfix_expression();//��׺���ʽ
	void primary_expression();//��ֵ���ʽ
	void argument_expression_list();//ʵ�α��ʽ��
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
	void operand_push(Type *type, int r, int value);
	void operand_pop();
	void operand_swap();
	void operand_assign(Operand *opd, int t, int r, int v);
	void cancel_lvalue();
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
};
#endif

