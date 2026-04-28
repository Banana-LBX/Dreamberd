#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 10000

typedef struct {
    char type[20];
    char value[100];
    int line;
    int column;
} Token;

char *text;
int pos = 0;
int line = 1;
int column = 1;
char current_char;

void advance() {
    if (current_char == '\n') {
        line++;
        column = 0;
    }

    pos++;
    column++;

    current_char = text[pos];
}

char peek() {
    if (text[pos + 1] == '\0') return '\0';
    return text[pos + 1];
}

void skip_whitespace() {
    while (current_char && isspace(current_char)) {
        advance();
    }
}

void skip_comment() {
    while (current_char && current_char != '\n') {
        advance();
    }
}

Token make_token(const char *type, const char *value, int line_, int col_) {
    Token t;
    strcpy(t.type, type);
    strcpy(t.value, value);
    t.line = line_;
    t.column = col_;
    return t;
}

Token number() {
    int start_col = column;
    char buffer[100] = {0};
    int i = 0;

    while (current_char && isdigit(current_char)) {
        buffer[i++] = current_char;
        advance();
    }

    return make_token("NUMBER", buffer, line, start_col);
}

Token identifier() {
    int start_col = column;
    char buffer[100] = {0};
    int i = 0;

    while (current_char && (isalnum(current_char) || current_char == '_')) {
        buffer[i++] = current_char;
        advance();
    }

    // keywords
    if (strcmp(buffer, "var") == 0) return make_token("VAR", buffer, line, start_col);
    if (strcmp(buffer, "const") == 0) return make_token("CONST", buffer, line, start_col);
    if (strcmp(buffer, "if") == 0) return make_token("IF", buffer, line, start_col);
    if (strcmp(buffer, "true") == 0) return make_token("TRUE", buffer, line, start_col);
    if (strcmp(buffer, "false") == 0) return make_token("FALSE", buffer, line, start_col);
    if (strcmp(buffer, "print") == 0) return make_token("PRINT", buffer, line, start_col);
    if (strcmp(buffer, "push") == 0) return make_token("PUSH", buffer, line, start_col);
    if (strcmp(buffer, "pop") == 0) return make_token("POP", buffer, line, start_col);

    return make_token("IDENT", buffer, line, start_col);
}

Token string() {
    int start_col = column;
    char buffer[256] = {0};
    int i = 0;

    advance(); // skip opening quote

    while (current_char && !(current_char == '"' && text[pos - 1] != '\\')) {
        buffer[i++] = current_char;
        advance();
    }

    advance(); // skip closing quote

    return make_token("STRING", buffer, line, start_col);
}

Token get_next_token() {
    while (current_char) {
        // skip whitespaces
        if (isspace(current_char)) {
            skip_whitespace();
            continue;
        }

        // skip comments
        if (current_char == '/' && peek() == '/') {
            advance();
            advance();
            skip_comment();
            continue;
        }

        if (current_char == '"')
            return string();

        if (isalpha(current_char) || current_char == '_')
            return identifier();

        if (isdigit(current_char))
            return number();

        int start_col = column;

        // Two character operators
        if (current_char == '=' && peek() == '=') {
            advance(); advance();
            return make_token("EQEQ", "==", line, start_col);
        }

        if (current_char == '!' && peek() == '=') {
            advance(); advance();
            return make_token("NOTEQ", "!=", line, start_col);
        }

        if (current_char == '<' && peek() == '=') {
            advance(); advance();
            return make_token("LTE", "<=", line, start_col);
        }

        if (current_char == '>' && peek() == '=') {
            advance(); advance();
            return make_token("GTE", ">=", line, start_col);
        }

        if (current_char == '&' && peek() == '&') {
            advance(); advance();
            return make_token("AND", "&&", line, start_col);
        }

        if (current_char == '|' && peek() == '|') {
            advance(); advance();
            return make_token("OR", "||", line, start_col);
        }

        // Single character tokens
        char ch = current_char;
        advance();

        switch (ch) {
            case '=': return make_token("EQUALS", "=", line, start_col);
            case '!': return make_token("BANG", "!", line, start_col);
            case ';': return make_token("SEMICOLON", ";", line, start_col);
            case '(': return make_token("LPAREN", "(", line, start_col);
            case ')': return make_token("RPAREN", ")", line, start_col);
            case '{': return make_token("LBRACE", "{", line, start_col);
            case '}': return make_token("RBRACE", "}", line, start_col);
            case '[': return make_token("LBRACKET", "[", line, start_col);
            case ']': return make_token("RBRACKET", "]", line, start_col);
        }

        return make_token("UNKNOWN", (char[]){ch, '\0'}, line, start_col);
    }

    return make_token("EOF", "", line, column);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("File error");
        return 1;
    }

    // read whole file
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    text = malloc(size + 1);
    fread(text, 1, size, f);
    text[size] = '\0';

    fclose(f);

    current_char = text[0];

    // tokenize
    while (1) {
        Token t = get_next_token();
        printf("Token(%s, '%s', %d:%d)\n", t.type, t.value, t.line, t.column);
        if (strcmp(t.type, "EOF") == 0)
            break;
    }

    free(text);
    return 0;
}
