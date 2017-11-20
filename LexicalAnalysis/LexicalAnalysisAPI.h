#ifdef LEXICALANALYSIS_EXPORTS
#define LEXICALANALYSIS_API __declspec(dllexport)
#else
#define LEXICALANALYSIS_API __declspec(dllimport)
#endif

class LexicalAnalysis {
private:
	void getch();
	void preprocess();
	void skip_white_space();
	void parse_comment();
	void parse_identifier();
	void parse_num();
	void parse_string(char str);
	void process();
	void token_colored(int lex_state);
	char* get_tkstr(int token);
	void skip(int c);
public:
	LexicalAnalysis();
	~LexicalAnalysis();
	int init();
	int open(char* file_name);
	int close();
	void run();
};