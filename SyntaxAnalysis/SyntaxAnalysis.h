// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SYNTAXANALYSIS_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SYNTAXANALYSIS_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef SYNTAXANALYSIS_EXPORTS
#define SYNTAXANALYSIS_API __declspec(dllexport)
#else
#define SYNTAXANALYSIS_API __declspec(dllimport)
#endif

// �����Ǵ� SyntaxAnalysis.dll ������
class SYNTAXANALYSIS_API CSyntaxAnalysis {
public:
	CSyntaxAnalysis(void);
	// TODO:  �ڴ�������ķ�����
};

extern SYNTAXANALYSIS_API int nSyntaxAnalysis;

SYNTAXANALYSIS_API int fnSyntaxAnalysis(void);
