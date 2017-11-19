#ifndef COMPLILERLOG_H_
#define COMPLILERLOG_H_

#define PRINT_MESSAGE  (0x00001)
#define PRINT_ERROR    (0x00010)
#define PRINT_WARRNING (0x00100)
#define PRINT_LINK     (0x01000)
#define PRINT_COMPILE  (0x10000)

#define PRINT_ALL (PRINT_ERROR | PRINT_MESSAGE | PRINT_WARRNING | PRINT_DEBUG | PRINT_COMPILE)

#ifdef COMPLILERLOG_EXPORTS
#define COMPLILERLOG_API __declspec(dllexport)
#else
#define COMPLILERLOG_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

	COMPLILERLOG_API void LogOpen(char *logFilePath, int logLevel);

	COMPLILERLOG_API void LogClose();

	COMPLILERLOG_API void LogError(const char *fmt, ...);

	COMPLILERLOG_API void LogWarrning(const char *fmt, ...);

	COMPLILERLOG_API void LogMessage(const char *fmt, ...);

	COMPLILERLOG_API void LogLink(const char *fmt, ...);

	COMPLILERLOG_API void LogCompile(const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif
