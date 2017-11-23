#include "stdafx.h"
#include "LexicalAnalysis.h"


static LogFunction g_LogMsg = NULL;
static LogFunction g_LogError = NULL;
static LogFunction	g_LogWarning = NULL;
static LogFunction g_LogDebug = NULL;

void LogMessage(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogMsg == NULL)
	{
#ifdef _WIN32||_WIN64
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);
#elif __ANDROID__
		__android_log_vprint(ANDROID_LOG_INFO, LOGTAG, fmt, arg);
#endif // _WIN32||_WIN64
	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogMsg(msg);
	}
	va_end(arg);
}

void LogError(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogError == NULL)
	{
#ifdef _WIN32||_WIN64
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);
#elif __ANDROID__
		__android_log_vprint(ANDROID_LOG_ERROR, LOGTAG, fmt, arg);
#endif // _WIN32||_WIN64
	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogError(msg);
	}
	va_end(arg);
}

void LogWarning(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogWarning == NULL)
	{
#ifdef _WIN32||_WIN64
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);
#elif __ANDROID__
		__android_log_vprint(ANDROID_LOG_WARN, LOGTAG, fmt, arg);
#endif // _WIN32||_WIN64
	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogWarning(msg);
	}
	va_end(arg);
}

void LogDebug(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	if (g_LogDebug == NULL)
	{
#ifdef _WIN32||_WIN64
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		strcat(msg, "\n");
		printf(msg, arg);
#elif __ANDROID__
		__android_log_vprint(ANDROID_LOG_DEBUG, LOGTAG, fmt, arg);
#endif // _WIN32||_WIN64
	}
	else
	{
		char msg[MSG_LEN];
		vsprintf(msg, fmt, arg);
		g_LogDebug(msg);
	}
	va_end(arg);
}

void SetLogFunction(LogFunction Logmsg, LogFunction Logerr, LogFunction LogWarnig, LogFunction Logdebug)
{
	g_LogMsg = Logmsg;
	g_LogError = Logerr;
	g_LogWarning = LogWarnig;
	g_LogDebug = Logdebug;
}
