#ifdef LEXICALANALYSIS_EXPORTS
#define LEXICALANALYSIS_API __declspec(dllexport)
#else
#define LEXICALANALYSIS_API __declspec(dllimport)
#endif
//LexicalAnalysisLog
typedef void(*LogFunction)(const char* fmt, ...);

class LexicalAnalysis {
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
	virtual void process() = 0;
	virtual void token_colored(int lex_state) = 0;
	virtual char* get_tkstr(int token) = 0;
	virtual void skip(int c) = 0;
public:
	LexicalAnalysis() {};
	virtual ~LexicalAnalysis() {};
	virtual int init() = 0;
	virtual int open(char* file_name) = 0;
	virtual int close() = 0;
	virtual void run() = 0;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


	LEXICALANALYSIS_API LexicalAnalysis* LexicalAnalysisCreate(void);
	LEXICALANALYSIS_API void LexicalAnalysisDestroy(LexicalAnalysis*);
	LEXICALANALYSIS_API void SetLogFunction(LogFunction Logmsg, LogFunction Logerr, LogFunction LogWarnig, LogFunction LogLink, LogFunction LogCompile);

#ifdef __cplusplus
}
#endif // __cplusplus