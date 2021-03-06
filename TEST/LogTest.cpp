#include<iostream>
#include"windows.h"
using namespace std;

#define PRINT_MESSAGE  (0x00001)
#define PRINT_ERROR    (0x00010)
#define PRINT_WARRNING (0x00100)
#define PRINT_LINK     (0x01000)
#define PRINT_COMPILE  (0x10000)

#define PRINT_ALL (PRINT_ERROR | PRINT_MESSAGE | PRINT_WARRNING | PRINT_LINK | PRINT_COMPILE)

typedef void(*LogError)(const char* fmt, ...);
typedef void(*LogWarning)(const char* fmt, ...);
typedef void(*LogLink)(const char* fmt, ...);
typedef void(*LogCompile)(const char* fmt, ...);
typedef void(*LogMessage)(const char* fmt, ...);
typedef void(*LogOpen)(char *logFilePath, int logLevel);
typedef void(*LogClose)();

#define MSG_LEN 256
char g_logpath[256] = "F:\\";									///<日志文件路径

int main() {
	LogOpen pLogOpen = NULL;
	LogWarning pLogWarning = NULL;
	LogLink pLogLink = NULL;
	LogCompile pLogCompile = NULL;
	LogMessage pLogMessage = NULL;
	LogClose pLogClose = NULL;
	LogError pLogError = NULL;

	wchar_t _dllpath[MSG_LEN];
	memset(_dllpath, 0, sizeof(wchar_t)*MSG_LEN);
	GetModuleFileName(NULL, _dllpath, MSG_LEN);
	LPCWSTR __LibName = L"Plugin\\ComplilerLog.dll";

	int len = wcslen(_dllpath);
	for (int i = len - 1; i > 0; i--)
	{
		if (_dllpath[i] == '\\')
		{
			_dllpath[i + 1] = '\0';
			break;
		}
	}
	
	__LibName = wcscat(_dllpath, __LibName);
	wcscpy(_dllpath, __LibName);
	void *m_libinst = LoadLibrary(_dllpath);

	pLogOpen = (LogOpen)GetProcAddress((HMODULE)m_libinst, "LogOpen");
	pLogClose = (LogClose)GetProcAddress((HMODULE)m_libinst, "LogClose");
	pLogWarning = (LogWarning)GetProcAddress((HMODULE)m_libinst, "LogWarning");
	pLogLink = (LogLink)GetProcAddress((HMODULE)m_libinst, "LogLink");
	pLogCompile = (LogCompile)GetProcAddress((HMODULE)m_libinst, "LogCompile");
	pLogError = (LogError)GetProcAddress((HMODULE)m_libinst, "LogError");
	pLogMessage = (LogMessage)GetProcAddress((HMODULE)m_libinst, "LogMessage");

	pLogOpen(g_logpath, PRINT_ALL);
	pLogMessage("[MediaFrameWork] : MeidaFrameWorkInit MeetingConfigPath:, MeetingLogPath:");
	pLogWarning("[MediaFrameWork] : MeidaFrameWorkInit MeetingConfigPath:, MeetingLogPath:");
	pLogLink("[MediaFrameWork] : MeidaFrameWorkInit MeetingConfigPath:, MeetingLogPath:");
	pLogCompile("[MediaFrameWork] : MeidaFrameWorkInit MeetingConfigPath:, MeetingLogPath:");
	pLogError("[MediaFrameWork] : MeidaFrameWorkInit MeetingConfigPath:, MeetingLogPath:");
	pLogClose();
	system("pause");

}

