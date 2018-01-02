#pragma once
#include"stdlib.h"
#include"../include/CompilerDef.h"

#define ALIGN_SET 0x100  // ǿ�ƶ����־

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