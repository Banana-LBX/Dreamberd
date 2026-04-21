import sys

class Token:
    def __init__(self, type_, value, line, column):
        self.type = type_
        self.value = value
        self.line = line
        self.column = column

    def __repr__(self):
        return f"Token({self.type}, {repr(self.value)}, {self.line}:{self.column})"

class Lexer:
    def __init__(self, text):
        self.text = text
        self.pos = 0
        self.line = 1
        self.column = 1
        self.current_char = text[0] if text else None

    # move forward one character
    def advance(self):
        if self.current_char == "\n":
            self.line += 1
            self.column = 0

        self.pos += 1
        self.column += 1

        if self.pos >= len(self.text):
            self.current_char = None
        else:
            self.current_char = self.text[self.pos]

    # look ahead without consuming
    def peek(self):
        peek_pos = self.pos + 1
        if peek_pos >= len(self.text):
            return None
        return self.text[peek_pos]

    def skip_whitespace(self):
        while self.current_char and self.current_char.isspace():
            self.advance()

    def number(self):
        start_col = self.column
        result = ""

        while self.current_char and self.current_char.isdigit():
            result += self.current_char
            self.advance()

        return Token("NUMBER", result, self.line, start_col)

    def identifier(self):
        start_col = self.column
        result = ""

        while self.current_char and (
            self.current_char.isalnum() or self.current_char == "_"
        ):
            result += self.current_char
            self.advance()

        keywords = {
            "var": "VAR",
            "const": "CONST",
            "if": "IF",
            "true": "TRUE",
            "false": "FALSE",
            "print": "PRINT",
            "push": "PUSH",
            "pop": "POP",
        }

        token_type = keywords.get(result, "IDENT")
        return Token(token_type, result, self.line, start_col)

    def string(self):
        start_col = self.column
        self.advance()  # skip opening quote
        result = ""

        while self.current_char:
            if self.current_char == '"' and self.text[self.pos - 1] != '\\':
                break
            result += self.current_char
            self.advance()

        self.advance()  # skip closing quote
        return Token("STRING", result, self.line, start_col)

    def get_next_token(self):
        while self.current_char:
            if self.current_char.isspace():
                self.skip_whitespace()
                continue

            if self.current_char == '"':
                return self.string()

            if self.current_char.isalpha() or self.current_char == "_":
                return self.identifier()

            if self.current_char.isdigit():
                return self.number()

            start_col = self.column
            char = self.current_char
            self.advance()

            if char == "=":
                return Token("EQUALS", char, self.line, start_col)
            if char == "!":
                return Token("BANG", char, self.line, start_col)
            if char == ";":
                return Token("SEMICOLON", char, self.line, start_col)
            if char == "(":
                return Token("LPAREN", char, self.line, start_col)
            if char == ")":
                return Token("RPAREN", char, self.line, start_col)
            if char == "{":
                return Token("LBRACE", char, self.line, start_col)
            if char == "}":
                return Token("RBRACE", char, self.line, start_col)
            if char == "[":
                return Token("LBRACKET", char, self.line, start_col)
            if char == "]":
                return Token("RBRACKET", char, self.line, start_col)

            return Token("UNKNOWN", char, self.line, start_col)

        return Token("EOF", None, self.line, self.column)

    def tokenize(self):
        tokens = []
        while True:
            token = self.get_next_token()
            tokens.append(token)
            if token.type == "EOF":
                break
        return tokens


# ---- main ----
with open(sys.argv[1]) as f:
    text = f.read()

lexer = Lexer(text)
tokens = lexer.tokenize()

for token in tokens:
    print(token)
