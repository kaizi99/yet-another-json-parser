#include <stdio.h>
#include <stdbool.h>

typedef enum token {
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_COMMA,
	TOK_COLON,
	TOK_LBRACK,
	TOK_RBRACK,
	TOK_STRING,
	TOK_NUMBER,
	TOK_TRUE,
	TOK_FALSE,
	TOK_NULL,
	TOK_EOF,
	TOK_ERROR,
	TOK_NONE
} token;

typedef enum lexer_state {
	LS_NONE,
	LS_TRUE_T,
	LS_TRUE_TR,
	LS_TRUE_TRU,
	LS_FALSE_F,
	LS_FALSE_FA,
	LS_FALSE_FAL,
	LS_FALSE_FALS,
	LS_NULL_N,
	LS_NULL_NU,
	LS_NULL_NUL,
	LS_STRING,
	LS_NUMBER,
	LS_ERROR
} lexer_state;

typedef struct lexer_return {
	char* token_start;
	size_t token_length;
	token token_type;
} lexer_return;

typedef struct lexer_info {
	char* string;
	lexer_state state;
	char* current_loc;
	char* token_start;
} lexer_info;

lexer_info lexer_init(char* string) {
	lexer_info lexer;

	lexer.string = string;
	lexer.state = LS_NONE;
	lexer.current_loc = string;
	lexer.token_start = string;

	return lexer;
}

#define LEXER_RETURN(tok, start, length) return (lexer_return){.token_start = start, .token_length = length, .token_type = tok};
#define LEXER_RETURN(tok) return (lexer_return){.token_start = NULL, .token_length = 0, .token_type = tok};

lexer_return lexer_step(lexer_info* lexer) {
	if (*(lexer->current_loc) == 0) {
		LEXER_RETURN(TOK_EOF)
	}

	switch (lexer->state) {
	case LS_ERROR:
		LEXER_RETURN(TOK_ERROR)
	case LS_NONE:
		switch (*(lexer->current_loc)) {
		case '{':
			lexer->current_loc++;
			LEXER_RETURN(TOK_LBRACE)
		case '}':
			lexer->current_loc++;
			LEXER_RETURN(TOK_RBRACE)
		case '[':
			lexer->current_loc++;
			LEXER_RETURN(TOK_LBRACK)
		case ']':
			lexer->current_loc++;
			LEXER_RETURN(TOK_RBRACK)
		case ':':
			lexer->current_loc++;
			LEXER_RETURN(TOK_COLON)
		case ',':
			lexer->current_loc++;
			LEXER_RETURN(TOK_COMMA)
		case ' ':
		case '\n':
		case '\t':
		case '\r':
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE)
		case 't':
			lexer->state = LS_TRUE_T;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE)
		case 'f':
			lexer->state = LS_FALSE_F;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE)
		case 'n':
			lexer->state = LS_NULL_N;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE)
		case '"':
			lexer->state = LS_STRING;
			lexer->current_loc++;
			lexer->token_start = lexer->current_loc;
			LEXER_RETURN(TOK_NONE)
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			lexer->state = LS_NUMBER;
			lexer->current_loc++;
			lexer->token_start = lexer->current_loc;
			LEXER_RETURN(TOK_NONE)
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR)
		}
	case LS_TRUE_T:
		switch (*(lexer->current_loc)) {
		case 'r':
			lexer->state = LS_TRUE_TR;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_TRUE_TR:
		switch (*(lexer->current_loc)) {
		case 'u':
			lexer->state = LS_TRUE_TRU;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_TRUE_TRU:
		switch (*(lexer->current_loc)) {
		case 'e':
			lexer->state = LS_NONE;
			lexer->current_loc++;
			LEXER_RETURN(TOK_TRUE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_NULL_N:
		switch (*(lexer->current_loc)) {
		case 'u':
			lexer->state = LS_NULL_NU;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_NULL_NU:
		switch (*(lexer->current_loc)) {
		case 'l':
			lexer->state = LS_NULL_NUL;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_NULL_NUL:
		switch (*(lexer->current_loc)) {
		case 'l':
			lexer->state = LS_NONE;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NULL);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_FALSE_F:
		switch (*(lexer->current_loc)) {
		case 'a':
			lexer->state = LS_FALSE_FA;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_FALSE_FA:
		switch (*(lexer->current_loc)) {
		case 'l':
			lexer->state = LS_FALSE_FAL;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_FALSE_FAL:
		switch (*(lexer->current_loc)) {
		case 's':
			lexer->state = LS_FALSE_FALS;
			lexer->current_loc++;
			LEXER_RETURN(TOK_NONE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	case LS_FALSE_FALS:
		switch (*(lexer->current_loc)) {
		case 'e':
			lexer->state = LS_NONE;
			lexer->current_loc++;
			LEXER_RETURN(TOK_FALSE);
		default:
			lexer->state = LS_ERROR;
			LEXER_RETURN(TOK_ERROR);
		}
	}
}

int main()
{
	printf("Hallo Welt\n");
}
