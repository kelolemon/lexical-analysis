# include <stdio.h>
# include <string.h>

char * keywds [51] = {"abstract", "boolean", "break", "byte","case", "catch", "char", "class", "continue", "default", "do","double", "else", "extends", "final", "finally", "float", "for","if", "implements", "import", "instanceof", "int", "interface", "long", "native", "new", "package", "private", "protected", "public", "return", "short", "static", "super", "switch","synchronized", "this", "throw","throws", "transient", "try","void","volatile","while","strictfp","enum","goto","const","assert","import"};
int len = 0;
char sentence[25500];
char * blwds  [4] = {"true", "false", "TRUE", "FALSE"};

int lines = 0;
int cols = 0;
void throw_error(char * info, char * sentence){
    printf("Error: %s", info);
    //cout << "Error:" << info;
    printf(". at (%d,%d):",lines,cols - (int)strlen(sentence));
    printf("%s", sentence);
}

void out_put(char * id_info, char * contain_info){
    printf( "<%s, %s", id_info, contain_info);
    printf(", (%d, %d)>\n", lines, cols - (int)strlen(contain_info));
}

int check_delimiter(char ch){
    return ch == '{' || ch =='}' || ch =='(' || ch ==')' || ch == ','
        || ch == ';' || ch == '[' || ch == ']' || ch == '.' || ch == ':';
}

int check_space(char ch){
    if (ch == '\n') lines ++, cols = 0;
    return ch ==' ' || ch =='\n' || ch == '\t';
}

int check_number(char ch){
    return ch >= '0' && ch <= '9';
}

int check_char(char ch){
    return (ch <= 'Z' && ch >= 'A') || (ch <= 'z' && ch >= 'a') ;
}

int check_opt(char ch){
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '!'
        || ch == '&' || ch == '|' || ch == '?' || ch == '<' || ch == '>';
}

void append(char input){
    sentence[len ++] = input;
    sentence[len] = '\0';
}

int check_keywords(char * cmp){
    for (int i = 0; i < 51; i++){
        if (strcmp(cmp, keywds[i]) == 0){
            return 1;
        }
    }
    return 0;
}

void advance(char *pinput_char, char *pinput_next_char, int init, FILE* input){
    if (init == 0) {
        *pinput_char = (char)getc(input);
        *pinput_next_char = (char)getc(input);
        cols ++;
        return;
    }
    *pinput_char = *pinput_next_char;
    *pinput_next_char = (char)getc(input);
    cols ++;
}

int check_bool(char * cmp){
    for (int i = 0; i < 4; i++){
        if (strcmp(cmp, blwds[i]) == 0){
            return 1;
        }
    }
    return 0;
}

int analysis(char *pinput, char *pinput_next, FILE* file){
    memset(sentence, 0, sizeof(sentence));
    len = 0;
    if (*pinput == EOF){
        puts("<EOF, EOF>");
        return 0;
    }
    for (; check_space(*pinput); advance(pinput, pinput_next, 1, file));
    if (check_delimiter(*pinput)){
        append(*pinput);
        advance(pinput, pinput_next, 1, file);
        out_put("delimiter", sentence);
        return 1;
    }
    if (check_char(*pinput)){
        append(*pinput);
        advance(pinput, pinput_next, 1, file);
        for (;check_char(*pinput) || check_number(*pinput);){
            append(*pinput);
            advance(pinput, pinput_next, 1, file);
        }
        if (check_bool(sentence)){
            out_put("bool", sentence);
        } else if (check_keywords(sentence)){
            out_put(sentence, sentence);
        } else {
            out_put("identifier", sentence);
        }
        return 1;
    }
    if (check_number(*pinput)){
        int flag = 0;
        append(*pinput);
        advance(pinput, pinput_next, 1, file);
        for (;check_number(*pinput) || *pinput == '.';){
            if (*pinput == '.') flag ++;
            append(*pinput);
            advance(pinput, pinput_next, 1, file);
        }
        if (!check_space(*pinput) && (*pinput == 'l' || *pinput =='L' )){
            append(*pinput);
            advance(pinput, pinput_next, 1, file);
        } else {
            if (check_char(*pinput)){
                for (;!check_space(*pinput);){
                    append(*pinput);
                    advance(pinput, pinput_next, 1, file);
                }
                throw_error("expected unqualified-id", sentence);
                return 0;
            }
        }
        if (flag == 1) {
            out_put("float", sentence);
        } else {
            out_put("integer", sentence);
        }
        return 1;
    }
    if (*pinput == '@'){
        append(*pinput);
        advance(pinput, pinput_next, 1, file);
        for (;*pinput != '\n';){
            append(*pinput);
            advance(pinput, pinput_next, 1, file);
        }
        lines ++, cols = 0;
        advance(pinput, pinput_next, 1, file);
        out_put("note", sentence);
        return 1;
    }
    if (*pinput == '/' && *pinput_next == '/'){
        for (;*pinput != '\n';) advance(pinput, pinput_next, 1, file);
        lines ++, cols = 0;
        advance(pinput, pinput_next, 1, file);
        return 1;
    }
    if (*pinput == '/' && *pinput_next == '*'){
        for (;!(*pinput == '*' && *pinput_next == '/');) {
            if (*pinput == '\n') lines ++, cols = 0;
            advance(pinput, pinput_next, 1, file);
        }
        advance(pinput, pinput_next, 1, file);
        advance(pinput, pinput_next, 1, file);
        return 1;
    }
    if (check_opt( *pinput)){
        append( *pinput);
        advance(pinput, pinput_next, 1, file);
        if (check_opt(*pinput)){
            append( *pinput);
            advance(pinput, pinput_next, 1, file);
            out_put("binary operator", sentence);
            return 1;
        }
        out_put("operator", sentence);
        return 1;
    }
    if (*pinput == '\"'){
        append(*pinput);
        advance(pinput, pinput_next, 1, file);
        for (;*pinput != '\"';){
            append(*pinput);
            advance(pinput, pinput_next, 1, file);
        }
        append(*pinput);
        advance(pinput, pinput_next, 1, file);
        out_put("string", sentence);
    }
    if (*pinput == '\''){
        append(*pinput);
        advance(pinput, pinput_next, 1, file);
        if (*pinput_next != '\'') {
            for (;*pinput != '\'';) {
                append(*pinput);
                advance(pinput, pinput_next, 1, file);
            }
            append(*pinput);
            throw_error("expect a char but get two chars", sentence);
            return 0;
        }
        for (;*pinput != '\'';) {
            append(*pinput);
            advance(pinput, pinput_next, 1, file);
        }
        append(*pinput);
        advance(pinput, pinput_next, 1, file);
        out_put("char", sentence);
    }
    return 1;
}

int main(int argc, char* argv[]){
    if (argc < 2) {
        puts("error: please input file");
        return 0;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("can't open %s\n",argv[1]);
        return 0;
    }
    char input, input_next;
    lines = 1;
    advance(&input, &input_next, 0, file);
    for (; analysis(&input, &input_next, file););
    return 0;
}
