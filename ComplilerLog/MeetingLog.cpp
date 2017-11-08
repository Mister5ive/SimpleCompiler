#include "MeetingLog.h"
#include "RDMediaCommon.h"
#include "RDThreads.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef RD_WIN
#include <Windows.h>
#elif defined RD_AND
#include <jni.h>
#endif

#define MAX_LOGFILE_LEN (1024*1024)   //10MB
#define LOGSTR_MAXLEN 128

static int  g_logLevel = 0x00000;
static FILE *fLog = NULL;
char g_logPath[256];
RDMutex g_lock;

#ifdef RD_WIN

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
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),ecolor);
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
	if(strlen(g_logPath) <= 1)
	{
		fLog = stdout;
		return FALSE;
	}

	char logPath[256];
	wchar_t temp[256];
	strcpy(logPath, g_logPath);
	int tlen = MultiByteToWideChar(CP_ACP, 0, logPath, strlen(logPath) ,NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, logPath, strlen(logPath) ,temp, tlen);
	temp[tlen]='\0';
	if(FindFirstFileExists(temp, FILE_ATTRIBUTE_DIRECTORY) == FALSE)
	{
		if(CreateDirectoryA(logPath, NULL) ==FALSE)
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
	sprintf(logtime,"\\%u-%02u-%02u-%02u-%02u-%02u-%03u_%lu.log",lpsystime.wYear,lpsystime.wMonth,lpsystime.wDay,lpsystime.wHour,lpsystime.wMinute,lpsystime.wSecond,lpsystime.wMilliseconds, pid);  
	logtime[strlen(logtime)] = '\0';

	strcat(logPath, logtime);

	fLog = fopen(logPath, "ab+");

	if(fLog == NULL)
	{
		fLog = stdout;
		return FALSE;
	}
	return TRUE;
}

#endif

#ifdef RD_AND

static JavaVM *g_JavaVM = 0;
static jclass g_CustomLog_class = 0;
static jmethodID g_CustomLogInfo_id = 0;
static jmethodID g_CustomLogError_id = 0;
static jmethodID g_CustomLogWarn_id = 0;
static jmethodID g_CustomLogDebug_id = 0;
static jstring g_Custom_LogTag = 0;

static jstring strtoJstring(JNIEnv* env, const char* pat)
{
	jstring jstr;
	jclass strClass = env->FindClass("java/lang/String");
	jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
	jmethodID getBytesID = env->GetMethodID(strClass, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray bytes = env->NewByteArray(strlen(pat));
	env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
	jstring encoding = env->NewStringUTF("gbk");
	jstr = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
	encoding = env->NewStringUTF("utf-8");
	env->DeleteLocalRef(bytes);
	bytes = (jbyteArray)env->CallObjectMethod(jstr, getBytesID, encoding);
	return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
}

JNIEnv *Get_Jni_Env(int *bAttach)
{
	JNIEnv *env = NULL;
	if(g_JavaVM == NULL)
	{
		LogError("g_JavaVM is NULL");
		return NULL;
	}
	*bAttach = 0;
	jint ret = g_JavaVM->GetEnv((void**)(&env), JNI_VERSION_1_4);
	if(ret != JNI_OK)
	{
		ret = g_JavaVM->AttachCurrentThread(&env,NULL);
		if(ret != JNI_OK)
		{
			return NULL;
		}
		*bAttach = 1;
	}
	return env;
}

void Del_Jni_Env(int bAttach)
{
	if(bAttach)
	{
		g_JavaVM->DetachCurrentThread();
	}
}

void SetJavaVM(void *vm)
{
	g_JavaVM = (JavaVM *)vm;
}
#endif //RD_AND

void LogOpen(char *logFilePath, int logLevel)
{
	g_logLevel = logLevel;

#ifdef RD_WIN
	g_logPath[0] = '\0';
	if(logFilePath != NULL)
	{
		strcpy(g_logPath, logFilePath);
	}
	else
	{
		fLog = stdout;
		return;
	}
	OpenFile();
#elif defined RD_IOS
	fLog = stdout;
#elif defined RD_AND
	JNIEnv *env = NULL;
	int bAttach = 0;
	env = Get_Jni_Env(&bAttach);
	if(env == NULL)
	{
		return;
	}
	jclass customlog_localref = env->FindClass("cn/redcdn/log/CustomLog");
	if(customlog_localref == NULL)
	{
		//LOG_JNIE("Could not find class cn.redcdn.log.CustomLog");
		return;
	}
	g_CustomLog_class = (jclass)env->NewGlobalRef(customlog_localref);
	env->DeleteLocalRef(customlog_localref);
	g_CustomLogInfo_id = env->GetStaticMethodID(g_CustomLog_class, "i", "(Ljava/lang/String;Ljava/lang/String;)V");
	if(g_CustomLogInfo_id == NULL)
	{
		//LOG_JNIE("Could not find static method CustomLog.i");
		return;
	}
	g_CustomLogWarn_id = env->GetStaticMethodID(g_CustomLog_class, "w", "(Ljava/lang/String;Ljava/lang/String;)V");
	if(g_CustomLogWarn_id == NULL)
	{
		//LOG_JNIE("Could not find static method CustomLog.w");
		return;
	}
	g_CustomLogError_id = env->GetStaticMethodID(g_CustomLog_class, "e", "(Ljava/lang/String;Ljava/lang/String;)V");
	if(g_CustomLogError_id == NULL)
	{
		//LOG_JNIE("Could not find static method CustomLog.e");
		return;
	}
	g_CustomLogDebug_id = env->GetStaticMethodID(g_CustomLog_class, "d", "(Ljava/lang/String;Ljava/lang/String;)V");
	if(g_CustomLogDebug_id == NULL)
	{
		//LOG_JNIE("Could not find static method CustomLog.d");
		return;
	}

	g_Custom_LogTag = (jstring)env->NewGlobalRef(strtoJstring(env, "qnsipphone"));
	if(g_Custom_LogTag == NULL)
	{
		//LOG_JNIE("Custom log tag is NULL");
		return;
	}
	Del_Jni_Env(bAttach);
#else
#endif
}

void LogClose()
{
	if(fLog != NULL)
	{
		if(fLog != stdout)
		{
			fclose(fLog);
		}
		fLog = NULL;
	}
	g_logLevel = 0;
}

static void WriteLogEx(char level, const char *fmt, va_list args)
{
	RDAutoLock lock(g_lock);
	if (fLog == stdout)
	{
		vprintf(fmt, args);
		char endflag[] = "\r\n";
		printf(endflag);
		return;
	}
	else
	{
		if(ftell(fLog) >= MAX_LOGFILE_LEN)
		{
			fclose(fLog);
			fLog = NULL;
			OpenFile();
		}
	}
#ifdef RD_WIN
	char logtime[30];
	SYSTEMTIME lpsystime;  
	GetLocalTime(&lpsystime);  
	sprintf(logtime,"%u:%02u:%02u %02u:%02u:%02u:%03u",lpsystime.wYear,lpsystime.wMonth,lpsystime.wDay,lpsystime.wHour,lpsystime.wMinute,lpsystime.wSecond,lpsystime.wMilliseconds);  
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
#endif

}

static void WriteLog(const char *fmt, va_list args)
{
	RDAutoLock lock(g_lock);
	if (fLog == stdout)
	{
		vprintf(fmt, args);
		char endflag[] = "\r\n";
		printf(endflag);
		return;
	}
	else
	{
		if(ftell(fLog) >= MAX_LOGFILE_LEN)
		{
			fclose(fLog);
			fLog = NULL;
			OpenFile();
		}
	}
#ifdef RD_WIN
	char logtime[30];
	SYSTEMTIME lpsystime;  
	GetLocalTime(&lpsystime);  
	sprintf(logtime,"%u:%02u:%02u:%02u:%02u:%02u:%03u\n",lpsystime.wYear,lpsystime.wMonth,lpsystime.wDay,lpsystime.wHour,lpsystime.wMinute,lpsystime.wSecond,lpsystime.wMilliseconds);  
	logtime[strlen(logtime)-1] = '\0';
	fprintf(fLog, logtime);
	fprintf(fLog, " ");
	//char log[1024];
	//vsprintf(log, fmt, args);
	//fprintf(fLog, "%s\n", log);
	vfprintf(fLog, fmt, args);
	char endflag[] = "\r\n";
	fprintf(fLog, endflag);
	fflush(fLog);
#endif

}

void LogMessage(const char *fmt, ...)
{
	if((g_logLevel & PRINT_MESSAGE) == 0)
	{
		return;
	}
	if (fLog == NULL)
	{
		fLog = stdout;
	}
	va_list args;
	va_start (args, fmt);
#ifdef RD_WIN
	if(fLog == stdout)
	{
		SetColor(GREEN);
	}
	//WriteLog(fmt, args);
	WriteLogEx('I', fmt, args);
	if(fLog == stdout)
	{
		SetColor(WHITE);
	}
#elif RD_IOS
	WriteLog(fmt, args);
#endif
	va_end (args);
}

void LogError(const char *fmt, ...)
{
	if((g_logLevel & PRINT_ERROR) == 0)
	{
		return;
	}
	if (fLog == NULL)
	{
		fLog = stdout;
	}
	va_list args;
	va_start (args, fmt);
#ifdef RD_WIN
	if(fLog == stdout)
	{
		SetColor(RED);
	}
	//WriteLog(fmt, args);
	WriteLogEx('E', fmt, args);
	if(fLog == stdout)
	{
		SetColor(WHITE);
	}
#elif RD_IOS
	WriteLog(fmt, args);
#endif
	va_end (args);
}

void LogWarrning(const char *fmt, ...)
{
	if((g_logLevel & PRINT_WARRNING) == 0)
	{
		return;
	}
	if (fLog == NULL)
	{
		fLog = stdout;
	}
	va_list args;
	va_start (args, fmt);
#ifdef RD_WIN
	if(fLog == stdout)
	{
		SetColor(PINK);
	}
	//WriteLog(fmt, args);
	WriteLogEx('W', fmt, args);
	if(fLog == stdout)
	{
		SetColor(WHITE);
	}
#elif RD_IOS
	WriteLog(fmt, args);
#endif
	va_end (args);
}

void LogDebug(const char *fmt, ...)
{
	if((g_logLevel & PRINT_DEBUG) == 0)
	{
		return;
	}
	if (fLog == NULL)
	{
		fLog = stdout;
	}
	va_list args;
	va_start (args, fmt);
#ifdef RD_WIN
	//WriteLog(fmt, args);
	WriteLogEx('D', fmt, args);
#elif RD_IOS
	WriteLog(fmt, args);
#endif
	va_end (args);
}