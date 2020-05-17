#ifndef HEADER_H
#define HEADER_H

#define true 1
#define false 0

//error definitions
#define NOT_IMPLEMENTED -1
#define OUT_OF_MEMORY -2
#define ERROR -3
#define NOT_ALLOWED -3
#define NAME_ALREADY_IN_USE -4
#define NO_SUCH_VARIABLE -5
#define NO_SUCH_PROCEDURE -6
#define FORM_LIST_EXCEEDED -7
#define ARGS_LIST_EXCEEDED -8

typedef enum {const_type, identifier_type, expression_type, routine_type, identifier_declaration } nodeType; /* used in the struct nodeType to define the type of node*/
typedef enum {undef, basic_int_value, basic_float_value, basic_boolean_value, basic_string_value } basicType;
typedef enum {complex_array, complex_structure} complexType;
typedef enum {basic_dataType, complex_dataType, procedure_type, no_op} dataType;
typedef enum {function, procedure} routineType;
typedef enum {routine_list} listContent;

typedef int bool;

typedef struct basic{
  basicType type;
  union{
     int i;
     float f;
     char* s;
     bool b;
  };
}basic;

typedef struct array{
  basicType type;
  union{
     int i;
     bool b;
  };
}array;

typedef struct complex{
  complexType type;
  union{
     int i;
     bool b;
  };
}complex;

typedef struct type{
  dataType dt;
  int size;
  union{
    basicType bt;
    complexType ct;
  }typeValue;
  struct type * t; //is the next type
}type;

/*
a symbol in the symbol table
 - name is the name of the symbol
 - type indicates the data type stored by the symbol
*/
typedef struct symbol
{
	char * name;
  dataType type;
  basicType bType;
  complexType cType;
  bool isCostant;
  union{
    int i;
    float f;
    bool b;
    char* s;
    complex c;
  }*value;
	struct symbol *next;
} symbol;

/* constants are stored in constant node type union */
typedef struct constant{
  basicType type;
  union{
    int int_value;
    bool bool_value;
    double float_value;
    char* string_value;
  } value;
} constant;

/* identifier for node union*/
typedef struct identifier{
  char * name;
}identifier;

typedef struct {
    int operator;                   /* operator */
    int noperands;                  /* number of operands */
    struct treeNode **op;	          /* operands */
} node;

typedef struct declarationNode{
    char * name;
    bool isCostant;
    type * t;
    struct treeNode * expr;
}declarationNode;

typedef struct routineNode{
   char * name;
   struct actual * args;
}routineNode;

typedef struct treeNode{
  nodeType type;        /* type of node, tells us the value contained in the union */
  union {
   constant con;        /* constants */
   identifier id;       /* identifiers */
   node expr;           /* can be the same under the name of operator*/
   declarationNode dec;
   routineNode routine; /*routine or procedure*/
  } value;
} treeNode;

//initialize a node
treeNode * init();

//expressions
treeNode * constantNode(const basicType, ... );
treeNode * identifierNode(const char *);
treeNode * opr(int oper, int nops, ...);
treeNode * fpCall(const char *, struct actual *);
treeNode * varDec(char *, bool constant, type * dataType, ...);

// symbol table functions
symbol * getSym(char const *, symbol *);
symbol * createSym(char const *, symbol **);
symbol * putSym(char const *, symbol **);

// array declaration 
type * arrayDec(int, type *, basicType);
type * basicDec(basicType);

// form struct
typedef struct form{
    char * name;
    bool byref;
    dataType type;
    union{
      basicType bt;
      complexType ct;
    };
    struct form * next;
}form;


form * newParam(const char * , bool byref, dataType ,...);
form * formList(form * , form ** );
int formLength(form *);
void printFormList(form * );
form * getFormAtIndex(int , form *);
form * formList(form * , form ** );

typedef struct actual{
    treeNode * expr;
    struct actual * next;
}actual;
actual * newActual(treeNode *);
actual * addToActList(actual *, actual **);
int actLength(actual *);
actual * getActualAtIndex(int, actual *);

typedef struct data{
    dataType type;
    union{
      complex c;
      basic b;
    };
}data;

// functions
typedef struct routine{
    char * name;
    routineType type;
    dataType returnType;
    union{
      basicType bt;
      complexType ct;
    };
    struct data * returnValue; 
    form * parameters;

    treeNode * statementList;
    symbol * env;
}routine;

routine * newRoutine(const char *,form *,treeNode *,...);

typedef struct list {
  listContent type;
  union{
    routine * r;
  };
  struct list * next;
}list;

routine * getRoutine(const char *, list *);

list * addToList(routine *, list **);
typedef struct program{
    treeNode * statementList;
    list * routineList;
    struct symbol * symtable;
 }program;

 void executeProgram(program *, symbol **, list * );
 void freeNode(treeNode *);
 
 // evaluation functions
 data eval(treeNode *p, symbol **, list *);
 data eval_constants(constant, symbol **);
 data eval_expr(node, symbol ** , list *);
 data eval_routine(routineNode , symbol **, list *);
 //for already evaluated data types
 //assigns data to symbol passed, does not use yet the routine List nor the symbol table
 data spec_assignment(symbol *, data , symbol **,list * );
 //evaluates treeNode and assign it to symbol, using the given symbol table
 data assignment(symbol *, treeNode *, symbol **,list * );
 data r_assignment(routine *, treeNode *, symbol **,list * );

 data * dataToDataPointer(data);

 data eval_identifier(identifier, symbol **, list *);
 symbol * getSymbolFromIdentifier(identifier, symbol **);

 void printData(data);
 data operation(int oper, data e1, data e2);
 data negate(data);

 data eval_identifier_declaration(declarationNode, symbol **, list * routineList);

#endif
