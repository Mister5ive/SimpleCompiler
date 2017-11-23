#ifndef  COMPILERDEF_H_
#define  COMPILERDEF_H_

#define TABLEMAX 1024
#define MSG_LEN 256

#define CODEC_CHECK_RETURN(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__);return;}
#define CODEC_CHECK_WHILE(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__); Sleep(10);continue;}
#define CODEC_CHECK_RETURN_ERROR(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__); return ERROR_INVALID_PARAM;}
#define ZERO_MEMORY(VAR) {memset(&VAR, NULL, sizeof(VAR));}

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
	TK_PLUS,
	TK_MINUS,
	TK_STAR,
	TK_DIVIDE,
	TK_MOD,
	TK_EQ,
	TK_NEQ,
	TK_LT,
	TK_LEQ,
	TK_GT,
	TK_GEQ,
	TK_ASSIGN,
	TK_POINTSTO,
	TK_DOT,
	TK_AND,
	TK_OPENPA,
	TK_CLOSEPA,
	TK_OPENBR,
	TK_CLOSEBR,
	TK_BEGIN,
	TK_END,
	TK_SEMICOLON,
	TK_COMMA,
	TK_ELLIPSIS,
	TK_EOF,
	TK_CINT,
	TK_CCHAR,
	TK_CSTR,
	KW_CHAR,
	KW_SHORT,
	KW_INT,
	KW_BOOL,
	KW_VOID,
	KW_STRUCT,
	KW_IF,
	KW_ELSE,
	KW_FOR,
	KW_CONTINUE,
	KW_BREAK,
	KW_RETURN,
	KW_SIZEOF,
	KW_ALIGN,
	KW_CDECL,
	KW_STDCALL,
	TK_IDENT
}eTokenCode;



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



typedef struct _Tk_Word {
	int tkcode;//word code
	struct _Tk_Word *next;//conflict word
	char *p_word;//word sring
	struct Symbol *sym_struct;
	struct Symbol *sym_identifier;

}_TkWord;


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

