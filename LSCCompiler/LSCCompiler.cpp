// LSCCompiler.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "LSCCompiler.h"


// ���ǵ���������һ��ʾ��
LSCCOMPILER_API int nLSCCompiler=0;

// ���ǵ���������һ��ʾ����
LSCCOMPILER_API int fnLSCCompiler(void)
{
    return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� LSCCompiler.h
CLSCCompiler::CLSCCompiler()
{
    return;
}
