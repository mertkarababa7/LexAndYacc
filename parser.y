
%{
    #include <stdio.h>
    #include "header.h"

    //varibales to read input files
    int yylex(void);
    void yyerror(char *s);
    extern FILE* yyin;
    char ** fileList;
    unsigned currentFile = 0;
    unsigned nFiels;
    symbol * table;

    basicType basic_type_temp;

%}
%error-verbose

%token INT_T FLOAT_T BOOL_T STRING_T

%token <iValue>INTEGER_V
%token <bValue>BOOLEAN_V
%token <fValue>FLOAT_V
%token <sValue>STRING_V
%token VARIABLE
%token WHILE IF PRINT FOR TO
%nonassoc IFX ARGS
%nonassoc ELSE
%nonassoc CONST RECORD

%left GE LE EQ NE LT GT
%left PLUS MINUS
%left MULTIPLY DIVIDE
%left EQUALS
%left AND OR
%right UMINUS POINTER
%nonassoc RCURLY LCURLY COMMA SEMICOLON MAIN RBRACK LBRACK EVAL LP RP

%union {
    int iValue;
    float fValue;
    char* sValue;
    int bValue;
    treeNode * tNode;
    symbol * symbolRecord;
    basicType bType;
    type * t;
    program * prg;
    form * formal;
    actual * ae;
    routine * routine;
    list * list;
};
%type <tNode> expr stmt_list stmt dec opt_stmt_list
%type <symbolRecord> VARIABLE
%type <bType> B
%type <t> type C opt_type
%type <formal> param form_list form
%type <ae> opt_actual_expr actual_expr
%type <prg> program
%type <routine> proc
%type <list> proc_list opt_proc_list


%start program


%%

program:
  opt_proc_list
  MAIN LCURLY
  opt_stmt_list
  RCURLY                                                                        {    program * prg = malloc(sizeof(program));                                                                              // prg = (program*)malloc(sizeof(program));
                                                                                     prg->routineList = $1;                                                                                     
                                                                                     prg->statementList = $4;
                                                                                     prg->symtable = table; //TODO: remove table if no global var can be declared
                                                                                     executeProgram(prg, &(prg->symtable), prg->routineList);
                                                                                }
  ;

dec:
    type VARIABLE SEMICOLON                                                     {$$= varDec($2->name, false, $1);}
    | type CONST VARIABLE EQUALS expr                                           {$$= varDec($3->name, true, $1, $5);}
    //| RECORD LCURLY opt_dec_list RCURLY VARIABLE
    ;


type: B                                                                         {
                                                                                  basic_type_temp = $1;
                                                                                }
      C                                                                         {
                                                                                  if($3==NULL){
                                                                                    $$ = basicDec(basic_type_temp);
                                                                                  }else{
                                                                                    $$ = $3;
                                                                                  }
                                                                                }    
      ;

C :   /*empty*/                                                                 {$$ = NULL;}
      | LBRACK INTEGER_V RBRACK C                                               {$$ = arrayDec($2,$4,basic_type_temp);}
      ;
opt_stmt_list:
      /*empty*/                                                                 {$$ = NULL;}
      | stmt_list                                                               {$$ = $1;}
      ;
opt_proc_list:
      /*empty*/                                                                 {$$=NULL;}
      | proc_list                                                               {$$=$1;}
      ;

proc_list:
        proc                                                                    {$$=addToList($1, NULL);}
        | proc_list proc                                                        {$$=addToList($2,&$1);}
        ;
proc:
        opt_type VARIABLE LP form RP LCURLY
                opt_stmt_list
            RCURLY                                                              {$$ = newRoutine($2->name,$4,$7,$1);}
        ;
opt_type:
        /*empty*/                                                               {$$ = basicDec(undef);}
        | B                                                                     {$$ = basicDec(basic_int_value);}
        ;
B:      INT_T                                                                   {$$ = basic_int_value;}
        | FLOAT_T                                                               {$$ = basic_float_value;}
        | BOOL_T                                                                {$$ = basic_boolean_value;}
        | STRING_T                                                              {$$ = basic_string_value;}
        ;
form:
         /*empty*/                                                              {$$ = NULL;}
         |form_list                                                             {$$ = $1;}
         ;
form_list:
        param                                                                   {$$=$1;}
        | form COMMA param                                                      {$$=formList($3,&$1);}
        ;
param:  B VARIABLE                                                              {$$ = newParam($2->name, false, basic_dataType,$1);}
        | POINTER B VARIABLE                                                      {$$ = newParam($3->name, true, basic_dataType,$2);}
        ;
opt_actual_expr:
        /*empty*/                                                               {$$ = NULL;}
        |actual_expr                                                            {$$ = $1;}
        ;
actual_expr:
        expr                                                                    {$$ = newActual($1);}
        |actual_expr COMMA expr                                                 {$$ = addToActList(newActual($3),&$1);}
        ;
stmt:
        SEMICOLON                                                               {$$ = opr(SEMICOLON,1,NULL);}
        | dec                                                                   {$$ = $1;}
        | expr SEMICOLON                                                        {$$ = $1;}
        | PRINT expr SEMICOLON                                                  {$$ = opr(PRINT,1,$2);}
        | VARIABLE EQUALS expr SEMICOLON                                        {$$ = opr(EQUALS,2,identifierNode($1->name),$3);}
        | WHILE LP expr RP stmt                                                 {$$ = opr(WHILE,2,$3,$5);}
        | IF LP expr RP stmt %prec IFX                                          {$$ = opr(IF,2,$3,$5);}
        | IF LP expr RP stmt ELSE stmt                                          {$$ = opr(IF,3,$3,$5,$7);}        
        | FOR LP VARIABLE EQUALS expr TO expr RP stmt                           {$$ = opr(FOR,4,identifierNode($3->name),$5,$7,$9);}
        | LCURLY stmt_list RCURLY                                               {$$ = $2;}
        ;


stmt_list:
        stmt                                                                    {$$ = $1;}
        | stmt_list stmt                                                        {$$ = opr(SEMICOLON,2,$1,$2);}
        ;
expr:
        INTEGER_V                                                               {
                                                                                  $$ = constantNode(basic_int_value, $1);
                                                                                }
        | FLOAT_V                                                               {
                                                                                  $$ = constantNode(basic_float_value, $1);
                                                                                }
        | STRING_V                                                              {                                                                                  
                                                                                  $$ = constantNode(basic_string_value, $1);
                                                                                }                                                                                
        | BOOLEAN_V                                                             {$$ = constantNode(basic_boolean_value, $1);}
        | EVAL VARIABLE LP opt_actual_expr RP                                   {$$ = fpCall($2->name,$4);}
        | VARIABLE                                                              {$$ = identifierNode($1->name);}
        | MINUS expr %prec UMINUS                                               {$$ = opr(UMINUS, 1, $2);}
        | expr PLUS expr                                                        {$$ = opr(PLUS,2,$1,$3);}
        | expr MINUS expr                                                       {$$ = opr(MINUS,2,$1,$3);}
        | expr MULTIPLY expr                                                    {$$ = opr(MULTIPLY,2,$1,$3);}
        | expr DIVIDE expr                                                      {$$ = opr(DIVIDE,2,$1,$3);}
        | expr LT expr                                                          {$$ = opr(LT,2,$1,$3);}
        | expr GT expr                                                          {$$ = opr(GT,2,$1,$3);}
        | expr GE expr                                                          {$$ = opr(GE,2,$1,$3);}
        | expr LE expr                                                          {$$ = opr(LE,2,$1,$3);}
        | expr NE expr                                                          {$$ = opr(NE,2,$1,$3);}
        | expr EQ expr                                                          {$$ = opr(EQ,2,$1,$3);}
        | expr AND expr                                                         {$$ = opr(AND,2,$1,$3);}
        | expr OR expr                                                          {$$ = opr(OR,2,$1,$3);}
        | LP expr RP                                                            {$$ = $2;}
        ;

%%


void yyerror(char *s){
    fprintf(stdout,"%s\n",s);
}

int yywrap(){
	FILE *file;
	if(yyin!=NULL){
		//close the file
		fclose(yyin);
	}
	file = NULL;	
	if(fileList[currentFile] != (char*)0){
            file = fopen(fileList[currentFile], "r");
            if(file!= NULL){
                currentFile++;
                yyin = file;
            }
	}
	if(file == NULL && currentFile < nFiels){
		fprintf(stderr,"%s %s\n","could not open file",fileList[currentFile]);
	}
	return (file ? 0 : 1); //1 = no more input
}

int main(int argc, char ** argv){
    if(argc < 2){
        return -1;
    }	
    fileList = argv+1;
    nFiels = argc-1;
    //open the files and set it to yyin
yywrap();
//parse the file to produce output/tokens
yyparse();
return 0;
}
