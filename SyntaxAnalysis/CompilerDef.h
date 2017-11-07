#ifndef  COMPILERDEF_H_
#define  COMPILERDEF_H_

#define TABLEMAX 1024

#define CODEC_CHECK_RETURN(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__);return;}
#define CODEC_CHECK_WHILE(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__); Sleep(10);continue;}
#define CODEC_CHECK_RETURN_ERROR(P)  if(!P){LogError("%s : %s invalid param, line:%d!",__FILE__, __FUNCTION__, __LINE__); return ERROR_INVALID_PARAM;}
#define ZERO_MEMORY(VAR) {memset(&VAR, 0, sizeof(VAR));}



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
	char *p_word;//word sring
	struct _Tk_Word *next;//conflict word
	//struct Symbol *
}_TkWord;



#endif // ! COMPILERDEF_H_

