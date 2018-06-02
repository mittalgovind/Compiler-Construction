// 2014B4A70530P
// GOVIND MITTAL
#define ID_LEN 			21
#define BUF_SIZE 		4096
#define ERROR           -1
#define SYMBOLMISMATCH	-2
#define PATTERNMISMATCH	-3
#define IDTOOLONG		-4
#define BRACKETMISSING	-5
#define QUOTEMISSING	-6

#define EPSILON 		100
#define TK_ASSIGNOP		101
#define TK_FUNID		102
#define TK_ID			103
#define TK_NUM			104
#define TK_RNUM			105
#define TK_STR			106
#define TK_END			107
#define TK_INT			108
#define TK_REAL			109
#define TK_STRING		110
#define TK_MATRIX		111
#define TK_MAIN			112
#define TK_SQO			113
#define TK_SQC			114
#define TK_OP			115
#define TK_CL			116
#define TK_SEMICOLON	117
#define TK_COMMA		118
#define TK_IF			119
#define TK_ELSE			120
#define TK_ENDIF		121
#define TK_READ			122
#define TK_PRINT		123
#define TK_FUNCTION		124
#define TK_PLUS			125
#define TK_MUL			126
#define TK_MINUS		127
#define TK_DIV			128
#define TK_SIZE			129
#define TK_AND			130
#define TK_OR			131
#define TK_NOT			132
#define TK_LT			133
#define TK_LE			134
#define TK_EQ			135
#define TK_GT			136
#define TK_GE			137
#define TK_NE			138

struct lexeme
{
	int symbol;
	char info[ID_LEN];
	int line_no;
};

typedef struct 
{
	int i;
	int currBuf;
	char buf1[BUF_SIZE + 1];
	char buf2[BUF_SIZE + 1];
	int line_count;
	FILE* fp;
	char ch;
}FileBuffer;
