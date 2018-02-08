#ifndef  COMPILERDEF_H_
#define  COMPILERDEF_H_


#define TABLEMAX 1024
#define MSG_LEN 256

#define CODEC_CHECK_RETURN(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__);return;}
#define CODEC_CHECK_WHILE(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__); Sleep(10);continue;}
#define CODEC_CHECK_RETURN_ERROR(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__); return ERROR_INVALID_PARAM;}
#define ZERO_MEMORY(VAR) {memset(&VAR, NULL, sizeof(VAR));}

#define ALIGN_SET 0x100  // ǿ�ƶ����־
#define MAXKEY	1024				// ��ϣ������
typedef enum {
	LEX_NORMAL = 0,
	LEX_LEP = 1
}scpLexState;

typedef enum {
	SCP_ERROR_NONE = 0,

	SCP_INVALID_PARAM = -1,
	SCP_POINTER_NULL = -2,
	SCP_MALLOC_FAILED = -3,
	SCP_OPEN_FAILED = -4,
	SCP_CLOSE_FAILED = -5,
	SCP_INIT_FAILED = -6



}scpStatus;

typedef enum {
	/* ��������ָ��� */
	TK_PLUS,		// + �Ӻ�
	TK_MINUS,		// - ����
	TK_STAR,		// * �Ǻ�
	TK_DIVIDE,		// / ����
	TK_MOD,			// % ���������
	TK_EQ,			// == ���ں�
	TK_NEQ,			// != �����ں�
	TK_LT,			// < С�ں�
	TK_LEQ,			// <= С�ڵ��ں�
	TK_GT,			// > ���ں�
	TK_GEQ,			// >= ���ڵ��ں�
	TK_ASSIGN,		// = ��ֵ����� 
	TK_POINTSTO,	// -> ָ��ṹ���Ա�����
	TK_DOT,			// . �ṹ���Ա�����
	TK_AND,         // & ��ַ�������
	TK_OPENPA,		// ( ��Բ����
	TK_CLOSEPA,		// ) ��Բ����
	TK_OPENBR,		// [ ��������
	TK_CLOSEBR,		// ] ��Բ����
	TK_BEGIN,		// { �������
	TK_END,			// } �Ҵ�����
	TK_SEMICOLON,	// ; �ֺ�    
	TK_COMMA,		// , ����
	TK_ELLIPSIS,	// ... ʡ�Ժ�
	TK_EOF,			// �ļ�������

	/* ���� */
	TK_CINT,		// ���ͳ���
	TK_CCHAR,		// �ַ�����
	TK_CSTR,		// �ַ�������

	/* �ؼ��� */
	KW_CHAR,		// char�ؼ���
	KW_SHORT,		// short�ؼ���
	KW_INT,			// int�ؼ���
	KW_BOOL,		// bool�ؼ���
	KW_VOID,		// void�ؼ���  
	KW_STRUCT,		// struct�ؼ���   
	KW_IF,			// if�ؼ���
	KW_ELSE,		// else�ؼ���
	KW_FOR,			// for�ؼ���
	KW_CONTINUE,	// continue�ؼ���
	KW_BREAK,		// break�ؼ���   
	KW_RETURN,		// return�ؼ���
	KW_SIZEOF,		// sizeof�ؼ���

	KW_ALIGN,		// __align�ؼ���	
	KW_CDECL,		// __cdecl�ؼ��� standard c call
	KW_STDCALL,     // __stdcall�ؼ��� pascal c call

	/* ��ʶ�� */
	TK_IDENT
}eTokenCode;

/* �洢���� */
typedef enum {
	SC_GLOBAL = 0x00f0,		// �������������ͳ������ַ��������ַ�������,ȫ�ֱ���,��������          
	SC_LOCAL = 0x00f1,		// ջ�б���
	SC_LLOCAL = 0x00f2,       // �Ĵ���������ջ��
	SC_CMP = 0x00f3,       // ʹ�ñ�־�Ĵ���
	SC_VALMASK = 0x00ff,       // �洢��������             
	SC_LVAL = 0x0100,       // ��ֵ       
	SC_SYM = 0x0200,       // ����	

	SC_ANOM = 0x10000000,     // ��������
	SC_STRUCT = 0x20000000,     // �ṹ�����
	SC_MEMBER = 0x40000000,     // �ṹ��Ա����
	SC_PARAMS = 0x80000000,     // ��������
}e_VarType;

/* ���ͱ��� */
typedef enum
{
	T_INT = 0,			// ����                     
	T_CHAR = 1,			// �ַ���                 
	T_SHORT = 2,			// ������                       
	T_VOID = 3,			// ������                        
	T_PTR = 4,			// ָ��                          
	T_FUNC = 5,			// ����                    
	T_STRUCT = 6,			// �ṹ�� 

	T_BTYPE = 0x000f,		// ������������          
	T_ARRAY = 0x0010,		// ����
}e_TypeCode;

/*�﷨״̬*/
typedef enum {
	SC_STATE_NULL = 0,		//��״̬��û���﷨����
	SC_STATE_SP = 1,		//�ո�	
	SC_STATE_LF_HT = 2,     //���в�������ÿһ���������������塢��������Ҫ��Ϊ��״̬
	SC_STATE_DELAY = 3		//�ӳٵ�ȡ����һ���ʺ�ȷ�������ʽ

}e_SynState;



template<typename T>
inline void safe_release(T **p, bool b_block)
{
	if (*p)
	{
		if (b_block)
			delete[](*p);
		else
			delete (*p);
		*p = NULL;
	}
}

template<typename T>
inline  T *rd_malloc(unsigned int size)
{
	T *p = NULL;
	try
	{
		if (size)
			p = new T[size];
		else
			p = new T;
	}
	catch (const std::bad_alloc& e)
	{
		//LogError("%s : %s malloc error:%s, line:%d!", __FILE__, __FUNCTION__, e.what(), __LINE__);
	}
	return p;
}

template<typename T>
inline  T *rd_malloc_class(void *reserverd)
{
	T *p = NULL;
	try
	{
		p = new T(reserverd);
	}
	catch (const std::bad_alloc& e)
	{
		//LogError("%s : %s malloc error:%s, line:%d!", __FILE__, __FUNCTION__, e.what(), __LINE__);
	}
	return p;
}



template<typename T>
struct _SmartString
{
	int size;//length of string
	int capacity;//the capacity of buffer
	T *data;
};


template<typename T>
struct _SmartArray {
	int size;//length of array
	int capacity;//the capacity of buffer
	T **data;

};

/* ���ʹ洢�ṹ���� */
typedef struct Type
{
	int t;						//��������
	struct Symbol *ref;			//���÷���
	
} Type;

/* ���Ŵ洢�ṹ���� */
typedef struct Symbol
{
	int token;					// ���ŵĵ��ʱ���
	int reg;					// ���Ź����ļĴ���
	int value;						// ���Ź���ֵ  ������COFF���ű�����ţ�
	Type type;					// ��������
	struct Symbol *next;		// �������������ţ��ṹ�嶨�������Ա�������ţ��������������������
	struct Symbol *prev_tok;	// ָ��ǰһ�����ͬ������
} Symbol;


typedef struct _Tk_Word {
	int tkcode;//word code						// ���ʱ��� 
	struct _Tk_Word *next;//conflict word		// ָ���ϣ��ͻ����������
	char *p_word;//word sring					// �����ַ��� 
	struct Symbol *sym_struct;					// ָ�򵥴�����ʾ�Ľṹ����
	struct Symbol *sym_identifier;				// ָ�򵥴�����ʾ�ı�ʶ��

}_TkWord;


#pragma pack(push, 1)
typedef struct Section
{
	int data_offset;			// ��ǰ����ƫ��λ��
	char *data;					// ������
	int data_allocated;			// �����ڴ�ռ�
	char  index;				// �����
	Section *link;				// ������������
	int *hashtab;				// ��ϣ��ֻ���ڴ洢���ű�ƫ��λ�ã�
	IMAGE_SECTION_HEADER sh;    // ��ͷ
}Section;

/* COFF���Žṹ���� */

typedef struct CoffSym
{

	DWORD   Name;					// �������� //ƫ����
	DWORD   Next;					// ���ڱ����ͻ����*/
	DWORD   Value;				// �������ص�ֵ
	short   Section;			// �ڱ������(��1��ʼ),���Ա�ʶ����˷��ŵĽ�*/
	WORD    Type;				// һ����ʾ���͵�����
	BYTE    StorageClass;		// ����һ����ʾ�洢����ö������ֵ
	BYTE    NumberOfAuxSymbols;	// ���ڱ���¼����ĸ������ű���ĸ���
} CoffSym;

/* �ض�λ�ṹ���� */
typedef struct CoffReloc
{
	DWORD offset;				// ��Ҫ�����ض�λ�Ĵ�������ݵĵ�ַ
	DWORD cfsym;				// ���ű������(��0��ʼ)
	BYTE  section;				// ��Ҫ�ض�λ���ݵĽڱ��
	BYTE  type;					// �ض�λ����
} CoffReloc;

#define CST_FUNC    0x20  //Coff�������ͣ�����
#define CST_NOTFUNC 0     //Coff�������ͣ��Ǻ���
#define OPSTACK_SIZE 256

#pragma pack(pop)

/*�����ݽṹ��*/
typedef struct Section_Data {

	Section *sec_text;			// �����
	Section	*sec_data;			// �ɶ���д���ݽ�
	Section	*sec_bss;			// δ��ʼ�����ݽ�
	Section	*sec_idata;			// ������
	Section	*sec_rdata;			// ֻ�����ݽ�
	Section	*sec_rel;			// �ض�λ��Ϣ��
	Section	*sec_symtab;		// ���ű��	
	Section	*sec_dynsymtab;		// ���ӿ���Ž�
}SectionData;


/* �������洢�ṹ�����������ջ�� */
typedef struct Operand
{
	Type type;			    // ��������
	unsigned short r;       // �Ĵ�����洢����
	int value;              // ����ֵ��������SC_GLOBAL
	struct Symbol *sym;     // ���ţ�������(SC_SYM | SC_GLOBAL)
} Operand;

/* Ѱַ��ʽ */
enum e_AddrForm
{
	ADDR_OTHER,				// �Ĵ������Ѱַ [EAX],[EBX]
	ADDR_REG = 3			// �Ĵ���ֱ��Ѱַ��EAX,EBX���൱��mod=11000000(C0)
};

/* �Ĵ������� */
enum e_Register
{
	REG_EAX = 0,
	REG_ECX,
	REG_EDX,
	REG_EBX,
	REG_ESP,
	REG_EBP,
	REG_ESI,
	REG_EDI,
	REG_ANY
};
#define REG_IRET  REG_EAX	// ��ź�������ֵ�ļĴ���

static _TkWord keywords[] = {
	{ TK_PLUS,		NULL	,"+",					NULL,	NULL },
	{ TK_MINUS,		NULL	,"-",					NULL,	NULL },
	{ TK_STAR,		NULL	,"*",					NULL,	NULL },
	{ TK_DIVIDE,	NULL	,"/",					NULL,	NULL },
	{ TK_MOD,		NULL	,"%",					NULL,	NULL },
	{ TK_EQ,		NULL	,"==",					NULL,	NULL },
	{ TK_NEQ,		NULL	,"!=",					NULL,	NULL },
	{ TK_LT,		NULL	,"<",					NULL,	NULL },
	{ TK_LEQ,		NULL	,"<=",					NULL,	NULL },
	{ TK_GT,		NULL	,">",					NULL,	NULL },
	{ TK_GEQ,		NULL	,">=",					NULL,	NULL },
	{ TK_ASSIGN,	NULL	,"=",					NULL,	NULL },
	{ TK_POINTSTO,	NULL	,"->",					NULL,	NULL },
	{ TK_DOT,		NULL	,".",					NULL,	NULL },
	{ TK_AND,		NULL	,"&",					NULL,	NULL },
	{ TK_OPENPA,	NULL	,"(",					NULL,	NULL },
	{ TK_CLOSEPA,	NULL	,")",					NULL,	NULL },
	{ TK_OPENBR,	NULL	,"[",					NULL,	NULL },
	{ TK_CLOSEBR,	NULL	,"]",					NULL,	NULL },
	{ TK_BEGIN,		NULL	,"{",					NULL,	NULL },
	{ TK_END,		NULL	,"}",					NULL,	NULL },
	{ TK_SEMICOLON,	NULL	,";",					NULL,	NULL },
	{ TK_COMMA,		NULL	,",",					NULL,	NULL },
	{ TK_ELLIPSIS,	NULL	,"...",					NULL,	NULL },
	{ TK_EOF,		NULL	,"End_Of_File",			NULL,	NULL },
	{ TK_CINT,		NULL	,"Integer Constant",	NULL,	NULL },
	{ TK_CCHAR,		NULL	,"Character Constant",	NULL,	NULL },
	{ TK_CSTR,		NULL	,"String Constants",	NULL,	NULL },
	{ KW_CHAR,		NULL	,"char",				NULL,	NULL },
	{ KW_SHORT,		NULL	,"short",				NULL,	NULL },
	{ KW_INT,		NULL	,"int",					NULL,	NULL },
	{ KW_BOOL,		NULL	,"bool",				NULL,	NULL },
	{ KW_VOID,		NULL	,"void",				NULL,	NULL },
	{ KW_STRUCT,	NULL	,"struct",				NULL,	NULL },
	{ KW_IF,		NULL	,"if",					NULL,	NULL },
	{ KW_ELSE,		NULL	,"else",				NULL,	NULL },
	{ KW_FOR,		NULL	,"for",					NULL,	NULL },
	{ KW_CONTINUE,	NULL	,"continue",			NULL,	NULL },
	{ KW_BREAK,		NULL	,"break",				NULL,	NULL },
	{ KW_RETURN,	NULL	,"return",				NULL,	NULL },
	{ KW_SIZEOF,	NULL	,"sizeof",				NULL,	NULL },
	{ KW_ALIGN,		NULL	,"__align",				NULL,	NULL },
	{ KW_CDECL,		NULL	,"__cdecl",				NULL,	NULL },
	{ KW_STDCALL,	NULL	,"__stdcall",			NULL,	NULL },
	{ 0,			NULL	,NULL,					NULL,	NULL },
};


#endif // ! COMPILERDEF_H_

