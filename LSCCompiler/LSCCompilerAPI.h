#ifdef LSCCOMPILER_EXPORTS
#define LSCCOMPILER_API __declspec(dllexport)
#else
#define LSCCOMPILER_API __declspec(dllimport)
#endif

//LexicalAnalysisLog
typedef void(*LogFunction)(const char* fmt, ...);

class LSCCompilerBase {
public:
	virtual void getch() = 0;
	virtual bool is_digit(char c) = 0;
	virtual bool is_identifier(char c) = 0;
	virtual void preprocess() = 0;
	virtual void skip_white_space() = 0;
	virtual void parse_comment() = 0;
	virtual void parse_identifier() = 0;
	virtual void parse_num() = 0;
	virtual void parse_string(char str) = 0;
	virtual void get_token() = 0;
	virtual void token_colored(int lex_state) = 0;
	virtual char* get_tkstr(int token) = 0;
	virtual void skip(int c) = 0;
public:
	LSCCompilerBase() {};
	virtual ~LSCCompilerBase() {};
	virtual int init() = 0;
	virtual int open(char* file_name) = 0;
	virtual int close() = 0;
	virtual void LexicalAnalysis() = 0;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


	LSCCOMPILER_API LSCCompilerBase* LexicalAnalysisCreate(void);
	LSCCOMPILER_API void LexicalAnalysisDestroy(LSCCompilerBase*);
	LSCCOMPILER_API void SetLogFunction(LogFunction Logmsg, LogFunction Logerr, LogFunction LogWarnig, LogFunction LogLink, LogFunction LogCompile);

#ifdef __cplusplus
}
#endif // __cplusplus
