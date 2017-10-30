#ifndef  COMPILERDEF_H_
#define  COMPILERDEF_H_

#define HASHMAX 1024 //哈希表容量

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

typedef struct _TkWord {
	int tkcode;//单词编码
	struct m_TkWord *next;//冲突的同义词
	char *spelling;//单词字符串
	struct Symbol *sym_struct;//指向单词所表示的数据结构
	struct Symbol *sym_identifier;//指向单词所表示的标识符
} m_TkWord;



#endif // ! COMPILERDEF_H_

