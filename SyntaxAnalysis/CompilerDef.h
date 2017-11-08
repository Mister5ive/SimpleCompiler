#ifndef  COMPILERDEF_H_
#define  COMPILERDEF_H_

#define TABLEMAX 1024

#define CODEC_CHECK_RETURN(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__);return;}
#define CODEC_CHECK_WHILE(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__); Sleep(10);continue;}
#define CODEC_CHECK_RETURN_ERROR(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__); return ERROR_INVALID_PARAM;}
#define ZERO_MEMORY(VAR) {memset(&VAR, 0, sizeof(VAR));}

enum  eTokenCode {
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
	TK_EPOINTSTO,
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
};

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


typedef struct _Smart_String
{
	int size;//length of string
	int capacity;//the capacity of buffer
	char *data;
}_SmartString;


typedef struct _Smart_Array {
	int size;//length of array
	int capacity;//the capacity of buffer
	void **data;

}_SmartArray;

typedef struct _Tk_Word{
	int tkcode;//word code
	struct _Tk_Word *next;//conflict word
	char *p_word;//word sring
	struct Symbol *sym_struct;
	struct Symbol *sym_identifier;

}_TkWord;



#endif // ! COMPILERDEF_H_

