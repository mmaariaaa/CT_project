#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdarg.h>

enum {
    ID,BREAK,CHAR,DOUBLE,ELSE,FOR,IF,INT,RETURN,STRUCT,VOID,WHILE,
    CT_INT,CT_REAL,CT_CHAR,CT_STRING,COOMA,SEMICOLON,
    LPAR,RPAR,LBRACKET,RBRACKET,LACC,RACC,ADD,SUB,MUL,DIV,DOT,AND,OR,NOT,ASSIGN,
    EQUAL,NOTEQ,LESSEQ,GREATER,GREATERQ,END
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
    fprintf(stderr,"error :");
    vfprintf(stderr,fmt,va);
    fprintf(stderr,"\n");
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk,const char *fmt,...){
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d",tk->line);
    vfprintf(stderr,fmt,va);
    fprintf(stderr,"\n");
    va_end(va);
    exit(-1);
}

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");

///Add Token

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

