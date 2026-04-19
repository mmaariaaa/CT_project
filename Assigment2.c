#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdarg.h>

enum {
    ID, BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE,
    CT_INT, CT_REAL, CT_CHAR, CT_STRING,
    COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC,
    ADD, SUB, MUL, DIV, DOT, AND, OR, NOT,
    ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ,
    END
};

typedef struct _Token{
    int code;
    union{
        char *text;
        long int i;
        double r;
    };
    int line;
    struct _Token *next;
}Token;

Token *tokens=NULL;
Token *lastToken=NULL;
int line=1;
char *pCrtCh;

void err(const char *fmt,...){
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error : ");
    vfprintf(stderr,fmt,va);
    fprintf(stderr,"\n");
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk,const char *fmt,...){
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d ",tk->line);
    vfprintf(stderr,fmt,va);
    fprintf(stderr,"\n");
    va_end(va);
    exit(-1);
}

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");


Token *addTk(int code){
    Token *tk;
    SAFEALLOC(tk,Token);
    tk->code=code;
    tk->line=line;
    tk->next=NULL;

    if(lastToken){
        lastToken->next=tk;
    }
    else{
        tokens=tk;
    }
    lastToken=tk;
    return tk;
}

char *create_String(const char *pstart,const char *pend){
    int n=(int)(pend-pstart);
    char *s=(char *)malloc(n+1);
    if(!s){
        err("not enough memory\n");
    }
    memcpy(s,pstart,n);
    s[n]='\0';
    return s;
}

char *load_file(const char *filename){
   FILE *f;
   char *buf;
   long n;
   f=fopen(filename,"rb");
   if(f==NULL){
    err("The file cannot be opened\n");
   }
   fseek(f,0,SEEK_END);
   n=ftell(f);
   fseek(f,0,SEEK_SET);
   buf=(char *)malloc(n+1);
   if(!buf){
    err("not enough memory");
   }
   if(fread(buf,1,n,f)!=(size_t)n){
    fclose(f);
    free(buf);
    err("cannot read input file");

   }
   buf[n]='\0';
   fclose(f);
   return buf;

}

int checkKeyword(const char *pstart, int n) {
    if (n==5 && memcmp(pstart, "break", 5)==0) return BREAK;
    if (n==4 && memcmp(pstart, "char", 4)==0) return CHAR;
    if (n==6 && memcmp(pstart, "double", 6)==0) return DOUBLE;
    if (n==4 && memcmp(pstart, "else", 4)==0) return ELSE;
    if (n==3 && memcmp(pstart, "for", 3)==0) return FOR;
    if (n==2 && memcmp(pstart, "if", 2)==0) return IF;
    if (n==3 && memcmp(pstart, "int", 3)==0) return INT;
    if (n==6 && memcmp(pstart, "return", 6)==0) return RETURN;
    if (n==6 && memcmp(pstart, "struct", 6)==0) return STRUCT;
    if (n==4 && memcmp(pstart, "void", 4)==0) return VOID;
    if (n==5 && memcmp(pstart, "while", 5)==0) return WHILE;
    return ID;
}

const char *getTokenName(int code) {
    switch (code) {
        case ID: return "ID";
        case BREAK: return "BREAK";
        case CHAR: return "CHAR";
        case DOUBLE: return "DOUBLE";
        case ELSE: return "ELSE";
        case FOR: return "FOR";
        case IF: return "IF";
        case INT: return "INT";
        case RETURN: return "RETURN";
        case STRUCT: return "STRUCT";
        case VOID: return "VOID";
        case WHILE: return "WHILE";
        case CT_INT: return "CT_INT";
        case CT_REAL: return "CT_REAL";
        case CT_CHAR: return "CT_CHAR";
        case CT_STRING: return "CT_STRING";
        case COMMA: return "COMMA";
        case SEMICOLON: return "SEMICOLON";
        case LPAR: return "LPAR";
        case RPAR: return "RPAR";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        case LACC: return "LACC";
        case RACC: return "RACC";
        case ADD: return "ADD";
        case SUB: return "SUB";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case DOT: return "DOT";
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case ASSIGN: return "ASSIGN";
        case EQUAL: return "EQUAL";
        case NOTEQ: return "NOTEQ";
        case LESS: return "LESS";
        case LESSEQ: return "LESSEQ";
        case GREATER: return "GREATER";
        case GREATEREQ: return "GREATEREQ";
        case END: return "END";
        default: return "UNKNOWN";
    }
}

int getNextToken(){
    int state=0,n;
    char c;
    const char *pstart;
    Token *tk;

    while(1){
        c=*pCrtCh;

        switch(state){
            case 0:
                if(c==' '||c=='\r'||c=='\t'){
                    pCrtCh++;
                }
                else if(c=='\n'){
                    line++;
                    pCrtCh++;
                }
                else if(c=='/'&&*(pCrtCh+1)=='/'){
                    pCrtCh+=2;
                    while(*pCrtCh!='\n'&&*pCrtCh!='\r'&&*pCrtCh!='\0'){
                        pCrtCh++;
                    }
                }
                else if(isalpha((unsigned char)c)||c=='_'){
                    pstart=pCrtCh;
                    pCrtCh++;
                    state=1;
                }
                else if(c>='1'&&c<='9'){
                    pstart=pCrtCh;
                    pCrtCh++;
                    state=3;
                }
                else if(c=='0'){
                    pstart=pCrtCh;
                    pCrtCh++;
                    state=36;
                }
                else if(c=='"'){
                    pstart=pCrtCh+1;
                    pCrtCh++;
                    state=11;
                }
                else if(c=='\''){
                    pstart=pCrtCh;
                    pCrtCh++;
                    state=12;
                }
                else if(c=='+'){
                    pCrtCh++;
                    state=16;
                }
                else if(c=='-'){
                    pCrtCh++;
                    state=17;
                }
                else if(c=='*'){
                    pCrtCh++;
                    state=18;
                }
                else if(c==','){
                    pCrtCh++;
                    state=19;
                }
                else if(c==';'){
                    pCrtCh++;
                    state=20;
                }
                else if(c=='='){
                    pCrtCh++;
                    state=21;
                }
                else if(c=='!'){
                    pCrtCh++;
                    state=23;
                }
                else if(c=='<'){
                    pCrtCh++;
                    state=27;
                }
                else if(c=='>'){
                    pCrtCh++;
                    state=29;
                }
                else if(c=='&'){
                    pCrtCh++;
                    state=32;
                }
                else if(c=='|'){
                    pCrtCh++;
                    state=34;
                }
                else if(c=='('){
                    pCrtCh++;
                    addTk(LPAR);
                    return LPAR;
                }
                else if(c==')'){
                    pCrtCh++;
                    addTk(RPAR);
                    return RPAR;
                }
                else if(c=='['){
                    pCrtCh++;
                    addTk(LBRACKET);
                    return LBRACKET;
                }
                else if(c==']'){
                    pCrtCh++;
                    addTk(RBRACKET);
                    return RBRACKET;
                }
                else if(c=='{'){
                    pCrtCh++;
                    addTk(LACC);
                    return LACC;
                }
                else if(c=='}'){
                    pCrtCh++;
                    addTk(RACC);
                    return RACC;
                }
                else if(c=='/'){
                    pCrtCh++;
                    addTk(DIV);
                    return DIV;
                }
                else if(c=='.'){
                    pCrtCh++;
                    addTk(DOT);
                    return DOT;
                }
                else if(c=='\0'){
                    addTk(END);
                    return END;
                }
                else{
                    tkerr(addTk(END),"invalid character");
                }
                break;

            case 1:
                if(isalnum((unsigned char)c)||c=='_'){
                    pCrtCh++;
                }
                else{
                    state=2;
                }
                break;

            case 2:
                n=(int)(pCrtCh-pstart);
                {
                    int code=checkKeyword(pstart,n);
                    tk=addTk(code);
                    if(code==ID){
                        tk->text=create_String(pstart,pCrtCh);
                    }
                    return tk->code;
                }

            case 3:
                if(isdigit((unsigned char)c)){
                    pCrtCh++;
                }
                else if(c=='.'){
                    pCrtCh++;
                    state=5;
                }
                else if(c=='e'||c=='E'){
                    pCrtCh++;
                    state=6;
                }
                else{
                    state=4;
                }
                break;

            case 4:
                tk=addTk(CT_INT);
                {
                    char *txt=create_String(pstart,pCrtCh);
                    tk->i=strtol(txt,NULL,0);
                    free(txt);
                }
                return CT_INT;

            case 5:
                if(isdigit((unsigned char)c)){
                    pCrtCh++;
                }
                else if(c=='e'||c=='E'){
                    pCrtCh++;
                    state=6;
                }
                else{
                    state=10;
                }
                break;

            case 6:
                if(c=='+'||c=='-'){
                    pCrtCh++;
                    state=7;
                }
                else if(isdigit((unsigned char)c)){
                    pCrtCh++;
                    state=8;
                }
                else{
                    tkerr(addTk(END),"invalid real exponent");
                }
                break;

            case 7:
                if(isdigit((unsigned char)c)){
                    pCrtCh++;
                    state=8;
                }
                else{
                    tkerr(addTk(END),"invalid real exponent");
                }
                break;

            case 8:
                if(isdigit((unsigned char)c)){
                    pCrtCh++;
                }
                else{
                    state=10;
                }
                break;

            case 10:
                tk=addTk(CT_REAL);
                {
                    char *txt=create_String(pstart,pCrtCh);
                    tk->r=atof(txt);
                    free(txt);
                }
                return CT_REAL;

            case 11:
                if(c=='"'){
                    tk=addTk(CT_STRING);
                    tk->text=create_String(pstart,pCrtCh);
                    pCrtCh++;
                    return CT_STRING;
                }
                else if(c=='\0'||c=='\n'||c=='\r'){
                    tkerr(addTk(END),"unterminated string");
                }
                else{
                    pCrtCh++;
                }
                break;

            case 12:
                if(c=='\0'||c=='\n'||c=='\r'||c=='\''){
                    tkerr(addTk(END),"invalid char constant");
                }
                else{
                    pCrtCh++;
                    state=13;
                }
                break;

            case 13:
                if(c=='\''){
                    pCrtCh++;
                    tk=addTk(CT_CHAR);
                    tk->i=*(pstart+1);
                    return CT_CHAR;
                }
                else{
                    tkerr(addTk(END),"invalid char constant");
                }
                break;

            case 16:
                addTk(ADD);
                return ADD;

            case 17:
                addTk(SUB);
                return SUB;

            case 18:
                addTk(MUL);
                return MUL;

            case 19:
                addTk(COMMA);
                return COMMA;

            case 20:
                addTk(SEMICOLON);
                return SEMICOLON;

            case 21:
                if(c=='='){
                    pCrtCh++;
                    state=22;
                }
                else{
                    addTk(ASSIGN);
                    return ASSIGN;
                }
                break;

            case 22:
                addTk(EQUAL);
                return EQUAL;

            case 23:
                if(c=='='){
                    pCrtCh++;
                    state=25;
                }
                else{
                    state=24;
                }
                break;

            case 24:
                addTk(NOT);
                return NOT;

            case 25:
                addTk(NOTEQ);
                return NOTEQ;

            case 27:
                if(c=='='){
                    pCrtCh++;
                    state=28;
                }
                else{
                    addTk(LESS);
                    return LESS;
                }
                break;

            case 28:
                addTk(LESSEQ);
                return LESSEQ;

            case 29:
                if(c=='='){
                    pCrtCh++;
                    state=31;
                }
                else{
                    state=30;
                }
                break;

            case 30:
                addTk(GREATER);
                return GREATER;

            case 31:
                addTk(GREATEREQ);
                return GREATEREQ;

            case 32:
                if(c=='&'){
                    pCrtCh++;
                    state=33;
                }
                else{
                    tkerr(addTk(END),"invalid operator &, expected &&");
                }
                break;

            case 33:
                addTk(AND);
                return AND;

            case 34:
                if(c=='|'){
                    pCrtCh++;
                    state=35;
                }
                else{
                    tkerr(addTk(END),"invalid operator |, expected ||");
                }
                break;

            case 35:
                addTk(OR);
                return OR;

            case 36:
                if(c=='x'||c=='X'){
                    pCrtCh++;
                    state=37;
                }
                else if(c>='0'&&c<='7'){
                    pCrtCh++;
                    state=38;
                }
                else if(c=='.'){
                    pCrtCh++;
                    state=5;
                }
                else if(c=='e'||c=='E'){
                    pCrtCh++;
                    state=6;
                }
                else{
                    state=4;
                }
                break;

            case 37:
                if(isxdigit((unsigned char)c)){
                    pCrtCh++;
                    state=39;
                }
                else{
                    tkerr(addTk(END),"invalid hex constant");
                }
                break;

            case 38:
                if(c>='0'&&c<='7'){
                    pCrtCh++;
                }
                else if(c=='.'){
                    pCrtCh++;
                    state=5;
                }
                else if(c=='e'||c=='E'){
                    pCrtCh++;
                    state=6;
                }
                 else if(c>='8' && c<='9'){
                    tkerr(addTk(END),"invalid octal constant");
                }
                
                else{
                    state=4;
                }
                break;

            case 39:
                if(isxdigit((unsigned char)c)){
                    pCrtCh++;
                }
                else if(isalnum((unsigned char)c)){
                    tkerr(addTk(END),"invalid hex constant");
                }
                else{
                    state=4;
                }
                break;
        }
    }
}

void showTokens() {
    Token *tk;
    for (tk=tokens;tk!=NULL;tk=tk->next) {
        printf("%s", getTokenName(tk->code));

        if (tk->code==ID || tk->code==CT_STRING) {
            printf(":%s",tk->text);
        }
        else if (tk->code==CT_INT || tk->code==CT_CHAR) {
            printf(":%ld",tk->i);
        }
        else if (tk->code==CT_REAL) {
            printf(":%lf",tk->r);
        }

        printf(" (line %d)\n",tk->line);
    }
}

void free_memory() {
    Token *tk=tokens;
    while (tk) {
        Token *aux=tk;
        tk=tk->next;

        if (aux->code==ID || aux->code==CT_STRING) {
            free(aux->text);
        }
        free(aux);
    }
}

int main(int argc, char **argv) {
    char *buf;

    if(argc!= 2) {
        printf("Usage: %s input.c\n", argv[0]);
        return 1;
    }

    buf=load_file(argv[1]);
    pCrtCh=buf;

    while(getNextToken()!=END) {
    }
    showTokens();
    free_memory();
    free(buf);
     return 0;
}


