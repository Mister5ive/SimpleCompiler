// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LSCCOMPILER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LSCCOMPILER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef LSCCOMPILER_EXPORTS
#define LSCCOMPILER_API __declspec(dllexport)
#else
#define LSCCOMPILER_API __declspec(dllimport)
#endif

// �����Ǵ� LSCCompiler.dll ������
class LSCCOMPILER_API CLSCCompiler {
public:
	CLSCCompiler(void);
	// TODO:  �ڴ�������ķ�����
};

extern LSCCOMPILER_API int nLSCCompiler;

LSCCOMPILER_API int fnLSCCompiler(void);
