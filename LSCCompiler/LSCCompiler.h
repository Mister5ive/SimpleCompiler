#ifndef LSCCOMPILER_H
#define LSCCOMPILER_H
#include "stdafx.h"
#include "LSCCompilerAPI.h"
#include "../include/CompilerDef.h"

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
	_TkWord **m_Word;//hash map
	SmartArray<_TkWord> m_tkTable;//dynamic array
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
	void release();
public:
	LSCCompilerImp();
	~LSCCompilerImp();
	int init();
	int open(char* file_name);
	int close();
	void LexicalAnalysis();
	void SyntaxAnalysis();

	void translation_unit();//
	void syntax_indent();////�﷨����
public:
	SmartString<char> *m_TkString;//�����ַ���
	SmartString<char> *m_SourceString;//����Դ���ַ���
	TkTable *m_TkHashTable;//���ʹ�ϣ��
	FILE *m_file;//Դ���ļ�
	char ch;//��ǰ���ļ���get�ĵ��ַ�
	int token;//���ʱ���
	int line_num;//�к�
	int tkvalue;//����ֵ
	int syntax_state;
	int syntax_level;
//�ʷ���������
private:
	void external_declaration(int sType);//�����ⲿ����
	int type_specifier();//�������ַ�
	void struct_specifier();//�ṹ���ַ�
	void struct_declaration_list();//
	void struct_declaration();//struct
	void function_calling_convebtion(int &func);//��������Լ��
	void struct_member_aligment();//�ṹ��Ա����
	void declarator();//������
	void direct_declarator();//ֱ��������
	void direct_declarator_postfix();//ֱ����������׺
	void parameter_type_list(int func_call);//�����β����ͱ�
	void funcbody();//������
	void initializer();//��ֵ��
	void statement();//���
	void compound_statement();//�������
	void expression_statement();//���ʽ���
	void if_statement();//if
	void for_statement();//for
	void continue_statement();//continue
	void break_statement();//break
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
};
#endif

