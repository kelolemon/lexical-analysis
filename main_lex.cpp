# include <cstdio>
# include <iostream>
# include <cstring>

using namespace std;

string keywds [51] = {"abstract", "boolean", "break", "byte","case", "catch", "char", "class", "continue", "default", "do","double", "else", "extends", "final", "finally", "float", "for","if", "implements", "import", "instanceof", "int", "interface", "long", "native", "new", "package", "private", "protected", "public", "return", "short", "static", "super", "switch","synchronized", "this", "throw","throws", "transient", "try","void","volatile","while","strictfp","enum","goto","const","assert","import"};
string sentence;
string blwds  [4] = {"true", "false", "TRUE", "FALSE"};

int lines = 0;
int cols = 0;
inline void throw_error(const string & info, const string & sentence){
    cout << "Error:" << info;
    printf(". at (%d,%d):",lines,cols - (int)sentence.length());
    cout << sentence << endl;
}

inline void out_put(const string &id_info, const string &contain_info){
    cout << "<" << id_info << ", " << contain_info;
    printf(", (%d, %d)>\n", lines, cols - (int)contain_info.length());
}

inline int check_delimiter(char ch){
    return ch == '{' || ch =='}' || ch =='(' || ch ==')' || ch == ','
        || ch == ';' || ch == '[' || ch == ']' || ch == '.' || ch == ':';
}

inline int check_space(char ch){
    if (ch == '\n') lines ++, cols = 0;
    return ch ==' ' || ch =='\n' || ch == '\t';
}

inline int check_number(char ch){
    return ch >= '0' && ch <= '9';
}

inline int check_char(char ch){
    return (ch <= 'Z' && ch >= 'A') || (ch <= 'z' && ch >= 'a') ;
}

inline int check_opt(char ch){
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '!'
        || ch == '&' || ch == '|' || ch == '?' || ch == '<' || ch == '>';
}

inline void append(char input){
    sentence = sentence + input ;
}

inline int check_keywords(const string & cmp){
    for (const auto & key : keywds){
        if (cmp == key){
            return 1;
        }
    }
    return 0;
}

inline void advance(char &input_char, char &input_next_char, int init, FILE* input){
    if (init == 0) {
        input_char = (char)getc(input);
        input_next_char = (char)getc(input);
        cols ++;
        return;
    }
    input_char = input_next_char;
    input_next_char = (char)getc(input);
    cols ++;
}

inline int check_bool(string cmp){
    for (const auto & key : blwds){
        if (cmp == key){
            return 1;
        }
    }
    return 0;
}

inline int analysis(char &input, char &input_next, FILE* file){
    sentence.clear();
    if (input == EOF){
        puts("<EOF, EOF>");
        return 0;
    }
    for (; check_space(input); advance(input, input_next, 1, file));
    if (check_delimiter(input)){
        append(input);
        advance(input, input_next, 1, file);
        out_put("delimiter", sentence);
        return 1;
    }
    if (check_char(input)){
        append(input);
        advance(input, input_next, 1, file);
        for (;check_char(input) || check_number(input);){
            append(input);
            advance(input, input_next, 1, file);
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
    if (check_number(input)){
        auto flag = 0;
        append(input);
        advance(input, input_next, 1, file);
        for (;check_number(input) || input == '.';){
            if (input == '.') flag ++;
            append(input);
            advance(input, input_next, 1, file);
        }
        if (!check_space(input) && (input == 'l' || input =='L' )){
            append(input);
            advance(input, input_next, 1, file);
        } else {
            if (check_char(input)){
                for (;!check_space(input);){
                    append(input);
                    advance(input, input_next, 1, file);
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
    if (input == '@'){
        append(input);
        advance(input, input_next, 1, file);
        for (;input != '\n';){
            append(input);
            advance(input, input_next, 1, file);
        }
        lines ++, cols = 0;
        advance(input, input_next, 1, file);
        out_put("note", sentence);
        return 1;
    }
    if (input == '/' && input_next == '/'){
        for (;input != '\n';) advance(input, input_next, 1, file);
        lines ++, cols = 0;
        advance(input, input_next, 1, file);
        return 1;
    }
    if (input == '/' && input_next == '*'){
        for (;!(input == '*' && input_next == '/');) {
            if (input == '\n') lines ++, cols = 0;
            advance(input, input_next, 1, file);
        }
        advance(input, input_next, 1, file);
        advance(input, input_next, 1, file);
        return 1;
    }
    if (check_opt(input)){
        append(input);
        advance(input, input_next, 1, file);
        if (check_opt(input)){
            append(input);
            advance(input, input_next, 1, file);
            out_put("binary operator", sentence);
            return 1;
        }
        out_put("operator", sentence);
        return 1;
    }
    if (input == '\"'){
        append(input);
        advance(input, input_next, 1, file);
        for (;input != '\"';){
            append(input);
            advance(input, input_next, 1, file);
        }
        append(input);
        advance(input, input_next, 1, file);
        out_put("string", sentence);
    }
    if (input == '\''){
        append(input);
        advance(input, input_next, 1, file);
        if (input_next != '\'') {
            for (;input != '\'';) {
                append(input);
                advance(input, input_next, 1, file);
            }
            append(input);
            throw_error("expect a char but get two chars", sentence);
            return 0;
        }
        for (;input != '\'';) {
            append(input);
            advance(input, input_next, 1, file);
        }
        append(input);
        advance(input, input_next, 1, file);
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
    if (file == nullptr) {
        printf("can't open %s\n",argv[1]);
        return 0;
    }
    char input, input_next;
    lines = 1;
    advance(input, input_next, 0, file);
    for (; analysis(input, input_next, file););
    return 0;
}
