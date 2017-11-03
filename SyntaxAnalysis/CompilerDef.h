#ifndef  COMPILERDEF_H_
#define  COMPILERDEF_H_

#define HASHMAX 1024 //��ϣ������

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
	int tkcode;//���ʱ���
	struct m_TkWord *next;//��ͻ��ͬ���
	char *spelling;//�����ַ���
	struct Symbol *sym_struct;//ָ�򵥴�����ʾ�����ݽṹ
	struct Symbol *sym_identifier;//ָ�򵥴�����ʾ�ı�ʶ��
} m_TkWord;



#endif // ! COMPILERDEF_H_

