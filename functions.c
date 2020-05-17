#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "header.h"

extern symbol * table;
void yyerror(char * );

treeNode * init(){
  treeNode * res;
  if((res = malloc(sizeof(treeNode))) == NULL){
    yyerror("Can not allocate more memory");
    exit(OUT_OF_MEMORY);
  }
  return res;
}

treeNode * constantNode(const basicType bt, ... ){
  treeNode * res = init();
  va_list ap;
  va_start(ap, bt);
  //init const
  constant c;
  c.type = bt;
  char* arg = NULL;
  switch (c.type) {
    case basic_int_value:
      c.value.int_value = va_arg(ap,int);
      break;
    case basic_float_value:
      c.value.float_value = va_arg(ap, double);
      break;
    case basic_boolean_value:
      c.value.bool_value = (va_arg(ap,int) >= 1 ? true : false);
      break;
    case basic_string_value:
      arg = va_arg(ap, char*);
      c.value.string_value = malloc(strlen(arg));
      strcpy(c.value.string_value, arg);
      break;
    case undef:
    default:
      yyerror("unmanaged type in costant node");
      exit(ERROR);
      break;
  }
  va_end(ap);
  res->type = const_type;
  res->value.con = c;
  return res;
}

treeNode * identifierNode(const char * varName){
  treeNode * res = init();
  identifier id;
  id.name = malloc(strlen(varName)+1);
  strcpy(id.name, varName);
  res->value.id =id;
  res->type = identifier_type;
  return res;
}

treeNode * opr(int oper, int nops, ...){
  va_list ap;
  treeNode *node = init();
  int i;
  if((node->value.expr.op = malloc(nops*sizeof(treeNode)))== NULL){
      yyerror("opr - can not allocate memory");
      exit(OUT_OF_MEMORY);
  }
  node->type = expression_type;
  node->value.expr.operator = oper;
  node->value.expr.noperands = nops;
  va_start(ap, nops);
  for(i = 0; i<nops;i++){
      node->value.expr.op[i]=va_arg(ap,treeNode*);
  }
  va_end(ap);
  return node;
}

treeNode * fpCall(const char * name , actual * args){
  treeNode *node = init();
  routineNode rtn;
  rtn.name = malloc(strlen(name)+1);
  strcpy(rtn.name, name);
  rtn.args = args;
  node->type = routine_type;
  node->value.routine = rtn;
  return node;
}

symbol * createSym(char const * varName, symbol ** symbolTable){
     symbol * s;
     char * variableName = malloc(strlen(varName)+1);
     strcpy(variableName,varName);
     s = putSym(variableName, symbolTable);
     return s;
 }

 symbol * putSym(char const * identifier, symbol ** symbolTable){
   symbol *ptr = malloc (sizeof (symbol));
   (ptr->value) = malloc(sizeof(int));
   ptr->name = (char *) malloc (strlen (identifier) + 1);
   strcpy (ptr->name,identifier);

   ptr->next = *symbolTable;
   *symbolTable = ptr;

   return ptr;
}

symbol * getSym(char const *identifier, symbol * symTable){
  symbol *ptr;
  for (ptr = symTable; ptr != (symbol *) 0; ptr = ptr->next){
    if (strcmp (ptr->name, identifier) == 0)
        return ptr;
    }
  return NULL;
}

treeNode * varDec(char * name, bool constant, type * dataType, ...){
  va_list ap;
  treeNode *node = init();
  node->type = identifier_declaration;
  node->value.dec.name = malloc(strlen(name)+1);
  strcpy(node->value.dec.name, name);
  node->value.dec.isCostant = constant;
  node->value.dec.t = dataType;
  if(dataType == NULL){
    yyerror("data type reached is null");
  }
  va_start(ap, dataType);
  node->value.dec.expr = va_arg(ap,treeNode*);
  va_end(ap);
  return node;
}

// array functions
type * arrayDec(int size, type * t, basicType bt){
  type * res = malloc(sizeof(type));
  res->size = size;
  if(t==NULL){
    res->dt = basic_dataType;
    res->typeValue.bt = bt;
    res->t = NULL;
  }else{
    res->dt = complex_dataType;
    res->typeValue.ct = complex_array;
    res->t = t;
  }
  return res;
}

type * basicDec(basicType bt){
  type * res = malloc(sizeof(type));
  res->size = 0;
  res->dt = basic_dataType;
  res->typeValue.bt = bt;
  res->t = NULL;
  return res;
}

// parameters

//TODO: optimize this code
form * formList(form * new, form ** list){
    form ** tmp = &((*list)->next);
    while(*tmp!=NULL){
      tmp = &((*tmp)->next);
    }
    *tmp = new;
    return (*list);
}

//implemented only basic types
form * newParam(const char * paramName, bool byref, dataType dt, ...){
  form * res = malloc(sizeof(form));
  res->name = malloc(strlen(paramName)+1);
  res->byref = byref;
  strcpy(res->name, paramName);
  res->type = dt;
  va_list ap;
  va_start(ap, dt);

  switch (dt) {
    case basic_dataType:
      (*res).bt =  va_arg(ap,basicType);
      break;
    case complex_dataType:
      (*res).ct = va_arg(ap,complexType);;
      break;
    default:
      yyerror("unmanaged type in parameter list.");
      exit(ERROR);
      break;
  }
  va_end(ap);
  res->next = NULL;
  return res;
}

//next param is the return type, just basic 
routine * newRoutine(const char * name,form * formals, treeNode * statements, ...){
  routine * res = malloc(sizeof(routine));
  res->name = malloc(strlen(name)+1);
  strcpy(res->name,name);
  res->parameters = formals;
  res->statementList = statements;
  va_list ap;
  va_start(ap, statements);
  type * t = va_arg(ap,type*);

  if(t == NULL || t->typeValue.bt == undef){
    res->bt = undef;
    res->type = procedure;
  }else{
    res->type = function;
    res->bt = t->typeValue.bt;

  }
  va_end(ap);
  res->returnType = basic_dataType;
  return res;
}

list * addToList(routine * newRoutine, list ** rList){
  list * l = malloc(sizeof(list));
  l->r = newRoutine;
  l->type = routine_list;
  if(rList == NULL){
    return l;
  }
  l->next = *rList;
  *rList = l;
  return l;
}

actual * newActual(treeNode * expr){
  actual * act = malloc(sizeof(actual));
  act->expr = expr;
  act->next = NULL;
  return act;
}

actual * addToActList(actual * new, actual ** list){
  actual ** tmp = &((*list)->next);
  while(*tmp!=NULL){
    tmp = &((*tmp)->next);
  }
  *tmp = new;
  return (*list);
}

int actLength(actual * args){
  int count = 0;
  if(args==NULL){
    return count;
  }
  actual * temp;
  for(temp = args; temp!=NULL; temp = temp->next){
    count++;
  }
  return count;
}
int formLength(form * forms){
  int count = 0;
  if(forms==NULL){
    return count;
  }
  form * temp;
  for(temp = forms; temp!=NULL; temp = temp->next){
    count++;
  }
  return count;
}

routine * getRoutine(const char * name, list * routineList){
  if(routineList == NULL){
    return NULL;
  }
  //TODO typecheking 
  routine *ptr;
  list * tmp;  
  for (tmp = routineList; tmp!=NULL; tmp=tmp->next){
    ptr = tmp->r;    
    if(strcmp(ptr->name, name)==0){
      return ptr;
    }
  }
  return NULL;
}

form * getFormAtIndex(int index, form * list){
  form * tmp;
  int i = 0;
  for(tmp = list; tmp!=NULL; tmp=tmp->next){
    if(i == index){
      return tmp;
    }
    i++;
  }
  return NULL;
}

actual * getActualAtIndex(int index, actual * list){
  actual * tmp;
  int i = 0;
  for(tmp = list; tmp!=NULL; tmp=tmp->next){
    if(i == index){
      return tmp;
    }
    i++;
  }
  return NULL;
}

symbol * getSymbolFromIdentifier(identifier identifierNode, symbol ** symbolTable){
 symbol * s = getSym(identifierNode.name, *symbolTable);
 if(s == 0){
   yyerror("variable not found");
 }
 return s;
}

data * dataToDataPointer(data d){
  data * res = malloc(sizeof(data));
  //TODO: validation
  res->type = d.type;
  switch (d.type) {
    case basic_dataType:
    {
      basic bRes;
      switch (d.b.type) {
        case basic_int_value:
          bRes.type = basic_int_value;
          bRes.i = d.b.i;
        break;
        case basic_float_value:
          bRes.type = basic_float_value;
          bRes.f = d.b.f;
        break;
        case basic_boolean_value:
          bRes.type = basic_boolean_value;
          bRes.b = d.b.b;
        break;
        default:
          yyerror("unknown data type");
          exit(NOT_IMPLEMENTED);
      }
      res->b = bRes;
    }
    break;
    case complex_dataType:
      yyerror("complex_dataType not implemented yet");
    default:
      yyerror("invalid data");
      exit(ERROR);
  }
  return res;
}
