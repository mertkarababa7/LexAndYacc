
%{
    #include "header.h"
    #include "y.tab.h"
    #undef yywrap
    void yyerror(char *);

%}

%%
"pointer"                                                                 return POINTER;
">="                                                                      return GE;
"<="                                                                      return LE;
"=="                                                                      return EQ;
"!="                                                                      return NE;
"<"                                                                       return LT;
">"                                                                       return GT;
"while"                                                                   return WHILE;
"if"                                                                      return IF;
"else"                                                                    return ELSE;
"print"                                                                   return PRINT;
"for"                                                                     return FOR;
"to"                                                                      return TO;
eval                                                                      return EVAL;
"TRUE"                                                                  {
                                                                          yylval.bValue=true;
                                                                          return BOOLEAN_V;
                                                                        }
"FALSE"                                                                 {
                                                                          yylval.bValue=false;
                                                                          return BOOLEAN_V;
                                                                        }
CONST                                                                     return CONST;
int                                                                       return INT_T;
float                                                                     return FLOAT_T;
bool                                                                      return BOOL_T;
string                                                                    return STRING_T;
main                                                                      return MAIN;
\"(([^\"]|\\\")*[^\\])?\"                                                 {
                                                                            //yyerror(yytext);
                                                                            char *dst = yytext;
                                                                            char *src = yytext;
                                                                            char c;
                                                                            while ((c = *src++) != '\0')
                                                                            {
                                                                                if (c == '\\')
                                                                                {
                                                                                    *dst++ = c;
                                                                                    if ((c = *src++) == '\0')
                                                                                        break;
                                                                                    *dst++ = c;
                                                                                }
                                                                                else if (c != '"')
                                                                                    *dst++ = c;
                                                                            }
                                                                            *dst = '\0';
                                                                            yylval.sValue = malloc(strlen(dst));
                                                                            strcpy (yylval.sValue, yytext);
                                                                            return STRING_V;
                                                                          }
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]       { /* Start for multiline comment */ }
[/][*]                                    { yyerror("Unterminated comment"); }
"and"                                                                   return AND;
"or"                                                                    return OR;
[a-zA-Z]([a-zA-Z_]*[0-9]*)*                                             {
                                                                          symbol * s;
                                                                          s = malloc (sizeof(symbol));
                                                                          s->name = malloc(strlen(yytext));//malloc(strlen(variableName)+1);
                                                                          strcpy (s->name,yytext);
                                                                          *((symbol**)&yylval) = s;
                                                                          return VARIABLE;
                                                                       }
([0-9]+\.)[0-9]+                                                        {
                                                                          int i;
                                                                          for (i = 0; i < strlen(yytext); i += 1)
                                                                          {
                                                                            if (yytext[i] == '.')
                                                                              yytext[i] = '.';
                                                                          }                                                                            
                                                                          yylval.fValue = atof(yytext);                                                                          
                                                                          return FLOAT_V;
                                                                        }

0                                                                       {
                                                                          //bool or int?
                                                                          yylval.iValue = atoi(yytext);
                                                                          return INTEGER_V;
                                                                        }

[1-9][0-9]*                                                             {
                                                                          yylval.iValue = atoi(yytext);
                                                                          return INTEGER_V;
                                                                        }
\+                                                                      {yyerror("plus");return PLUS;}
\-                                                                      return MINUS;
\*                                                                      return MULTIPLY;
\/                                                                      return DIVIDE;
=                                                                       {
                                                                          //yyerror("EQUALS");
                                                                          return EQUALS;                                                                          
                                                                        }

[ \t\n]                                                                 ;/* skip whitespace */
\;                                                                      return SEMICOLON;
\,                                                                      return COMMA;
\(                                                                      return LP;
\)                                                                      return RP;
\{                                                                      return LCURLY;
\}                                                                      return RCURLY;
\[                                                                      return LBRACK;
\]                                                                      return RBRACK;
.                                                                       { ECHO;
                                                                          yyerror("Previous char is unknown to lexer");
                                                                        }

%%
