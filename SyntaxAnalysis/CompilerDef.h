#ifndef  COMPILERDEF_H_
#define  COMPILERDEF_H_



typedef struct _Smart_String
{
	int size;//length of string
	int capacity;//the capacity of buffer
	char *data;
}m_SmartString;


typedef struct _Smart_Array {
	int size;//length of array
	int capacity;//the capacity of buffer
	void **data;

}m_SmartArray;

typedef struct _Tk_Table{


}m_TkTable;
#endif // ! COMPILERDEF_H_

