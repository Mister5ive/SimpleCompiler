// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� COMPLILERLOG_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// COMPLILERLOG_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef COMPLILERLOG_EXPORTS
#define COMPLILERLOG_API __declspec(dllexport)
#else
#define COMPLILERLOG_API __declspec(dllimport)
#endif

// �����Ǵ� ComplilerLog.dll ������
class COMPLILERLOG_API CComplilerLog {
public:
	CComplilerLog(void);
	// TODO:  �ڴ�������ķ�����
};

extern COMPLILERLOG_API int nComplilerLog;

COMPLILERLOG_API int fnComplilerLog(void);
