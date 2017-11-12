// ComplilerLog.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ComplilerLog.h"

#define MAX_LOGFILE_LEN (1024*1024)   //10MB
#define LOGSTR_MAXLEN 128

static int  g_logLevel = 0x00000;
static FILE *fLog = NULL;
char g_logPath[256];

typedef enum _Color
{
	DARKBLUE = 1,
	DARKGREEN,
	DARKTEAL,
	DARKRED,
	DARKPINK,
	DARKYELLOW,
	GRAY,
	DARKGRAY,
	BLUE,
	GREEN,
	TEAL,
	RED,
	PINK,
	YELLOW,
	WHITE
}Color;

void SetColor(Color ecolor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ecolor);
}

static BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}

static BOOL OpenFile()
{
	if (strlen(g_logPath) <= 1)
	{
		fLog = stdout;
		return FALSE;
	}

	char logPath[256];
	wchar_t temp[256];
	strcpy(logPath, g_logPath);
	int tlen = MultiByteToWideChar(CP_ACP, 0, logPath, strlen(logPath), NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, logPath, strlen(logPath), temp, tlen);
	temp[tlen] = '\0';
	if (FindFirstFileExists(temp, FILE_ATTRIBUTE_DIRECTORY) == FALSE)
	{
		if (CreateDirectoryA(logPath, NULL) == FALSE)
		{
			printf("Create directory failed.\n");
			fLog = stdout;
			return FALSE;
		}
	}

	char logtime[64];
	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	DWORD pid = GetCurrentProcessId();
	sprintf(logtime, "\\%u-%02u-%02u-%02u-%02u-%02u-%03u_%lu.log", lpsystime.wYear, lpsystime.wMonth, lpsystime.wDay, lpsystime.wHour, lpsystime.wMinute, lpsystime.wSecond, lpsystime.wMilliseconds, pid);
	logtime[strlen(logtime)] = '\0';

	strcat(logPath, logtime);

	fLog = fopen(logPath, "ab+");

	if (fLog == NULL)
	{
		fLog = stdout;
		return FALSE;
	}
	return TRUE;
}


void LogOpen(char *logFilePath, int logLevel)
{
	g_logLevel = logLevel;

	g_logPath[0] = '\0';
	if (logFilePath != NULL)
	{
		strcpy(g_logPath, logFilePath);
	}
	else
	{
		fLog = stdout;
		return;
	}
	OpenFile();
}

void LogClose()
{
	if (fLog != NULL)
	{
		if (fLog != stdout)
		{
			fclose(fLog);
		}
		fLog = NULL;
	}
	g_logLevel = 0;
}

static void WriteLogEx(char level, const char *fmt, va_list args)
{
	
	if (fLog == stdout)
	{
		vprintf(fmt, args);
		char endflag[] = "\r\n";
		printf(endflag);
		return;
	}
	else
	{
		if (ftell(fLog) >= MAX_LOGFILE_LEN)
		{
			fclose(fLog);
			fLog = NULL;
			OpenFile();
		}
	}

	char logtime[30];
	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	sprintf(logtime, "%u:%02u:%02u %02u:%02u:%02u:%03u", lpsystime.wYear, lpsystime.wMonth, lpsystime.wDay, lpsystime.wHour, lpsystime.wMinute, lpsystime.wSecond, lpsystime.wMilliseconds);
	logtime[strlen(logtime)] = '\0';
	DWORD tid = GetCurrentThreadId();
	fprintf(fLog, "[%s] [%c] tid[% 5lu]", logtime, level, tid);
	fprintf(fLog, " ");
	//char log[1024];
	//vsprintf(log, fmt, args);
	//fprintf(fLog, "%s\n", log);
	vfprintf(fLog, fmt, args);
	char endflag[] = "\r\n";
	fprintf(fLog, endflag);
	fflush(fLog);

}

static void WriteLog(const char *fmt, va_list args)
{
	
	if (fLog == stdout)
	{
		vprintf(fmt, args);
		char endflag[] = "\r\n";
		printf(endflag);
		return;
	}
	else
	{
		if (ftell(fLog) >= MAX_LOGFILE_LEN)
		{
			fclose(fLog);
			fLog = NULL;
			OpenFile();
		}
	}

	char logtime[30];
	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	sprintf(logtime, "%u:%02u:%02u:%02u:%02u:%02u:%03u\n", lpsystime.wYear, lpsystime.wMonth, lpsystime.wDay, lpsystime.wHour, lpsystime.wMinute, lpsystime.wSecond, lpsystime.wMilliseconds);
	logtime[strlen(logtime) - 1] = '\0';
	fprintf(fLog, logtime);
	fprintf(fLog, " ");
	//char log[1024];
	//vsprintf(log, fmt, args);
	//fprintf(fLog, "%s\n", log);
	vfprintf(fLog, fmt, args);
	char endflag[] = "\r\n";
	fprintf(fLog, endflag);
	fflush(fLog);


}

void LogMessage(const char *fmt, ...)
{
	if ((g_logLevel & PRINT_MESSAGE) == 0)
	{
		return;
	}
	if (fLog == NULL)
	{
		fLog = stdout;
	}
	va_list args;
	va_start(args, fmt);

	if (fLog == stdout)
	{
		SetColor(GREEN);
	}
	//WriteLog(fmt, args);
	WriteLogEx('I', fmt, args);
	if (fLog == stdout)
	{
		SetColor(WHITE);
	}
	va_end(args);
}

void LogError(const char *fmt, ...)
{
	if ((g_logLevel & PRINT_ERROR) == 0)
	{
		return;
	}
	if (fLog == NULL)
	{
		fLog = stdout;
	}
	va_list args;
	va_start(args, fmt);

	if (fLog == stdout)
	{
		SetColor(RED);
	}
	//WriteLog(fmt, args);
	WriteLogEx('E', fmt, args);
	if (fLog == stdout)
	{
		SetColor(WHITE);
	}

	va_end(args);
}

void LogWarrning(const char *fmt, ...)
{
	if ((g_logLevel & PRINT_WARRNING) == 0)
	{
		return;
	}
	if (fLog == NULL)
	{
		fLog = stdout;
	}
	va_list args;
	va_start(args, fmt);

	if (fLog == stdout)
	{
		SetColor(PINK);
	}
	//WriteLog(fmt, args);
	WriteLogEx('W', fmt, args);
	if (fLog == stdout)
	{
		SetColor(WHITE);
	}

	va_end(args);
}

void LogDebug(const char *fmt, ...)
{
	if ((g_logLevel & PRINT_DEBUG) == 0)
	{
		return;
	}
	if (fLog == NULL)
	{
		fLog = stdout;
	}
	va_list args;
	va_start(args, fmt);

	//WriteLog(fmt, args);
	WriteLogEx('D', fmt, args);

	va_end(args);
}

