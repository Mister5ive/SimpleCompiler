#pragma once
#include"stdlib.h"
#include"../include/CompilerDef.h"

/*��������*/
typedef enum {
	SC_GLOBAL = 0,          //ȫ�ֱ���
	SC_LOCAL = 1			//�ֲ�����	
}e_VarType;

/*�﷨״̬*/
typedef enum {
	SC_STATE_NULL = 0,		//��״̬��û���﷨����
	SC_STATE_SP = 1,		//�ո�	
	SC_STATE_LF_HT = 2,     //���в�������ÿһ���������������塢��������Ҫ��Ϊ��״̬
	SC_STATE_DELAY = 3		//�ӳٵ�ȡ����һ���ʺ�ȷ�������ʽ

}e_SynState;