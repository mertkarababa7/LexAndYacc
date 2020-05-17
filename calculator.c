#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "y.tab.h"

void yyerror(char * );

void executeProgram(program * program, symbol ** symTable, list * routineList){
    eval(program->statementList, symTable,routineList);
}

data eval_constants(constant c, symbol ** symTable){
  data res;
  basic b;
  b.type=c.type;
  switch (c.type) {
    case basic_int_value:
      b.i = c.value.int_value;
      break;
    case basic_float_value:
      b.f = c.value.float_value;
      break;
    case basic_boolean_value:
      b.b = c.value.bool_value;
      break;
    case basic_string_value:
      b.s = c.value.string_value;
      break;
    default:
      yyerror("unknown type in the eval_constants function");
      exit(NOT_IMPLEMENTED);
      break;
  }
  res.type = basic_dataType;
  res.b = b;
  return res;
}

data eval_expr(node expr, symbol ** symTable,list * routineList){
  data res;
  switch (expr.operator) {
    case SEMICOLON:
      {
        eval(expr.op[0],symTable,routineList);
        if(expr.noperands>1){          
          return eval(expr.op[1], symTable, routineList);
        }
      }
      break;
    case PRINT:
        { //statement
          data e = eval(expr.op[0], symTable,routineList);
          printData(e);
          res.type = no_op;
          return res;
        }
      break;
    case EQUALS: 
        { //statement
          symbol * s = getSymbolFromIdentifier(expr.op[0]->value.id,symTable);
          //check if the variable is a function
          routine * r = getRoutine(expr.op[0]->value.id.name, routineList);
          if(s == NULL && r == NULL){
            yyerror("during assignment found unknown variable or function");
            exit(NO_SUCH_VARIABLE);
          }
          //else
          data res;
          if(s!=NULL){
            res = assignment(s,expr.op[1],symTable,routineList);
          }
          if(r!=NULL){
            //TODO check if this is a function or a procedure
            res = r_assignment(r,expr.op[1], symTable,routineList);
          }
          return res;
        }
          break;
    case WHILE: //statement
        { 
          data condition = eval(expr.op[0],symTable,routineList);          
          if(condition.b.b == true){
            eval(expr.op[1],symTable,routineList);            
            treeNode * newWhile = opr(WHILE,2,expr.op[0],expr.op[1]);
            eval(newWhile,symTable,routineList);
          }
          data res;
          res.type = no_op;
          return res;
        }
        break;
    case IF: //statement
      {
          data condition = eval(expr.op[0],symTable,routineList);
          if(condition.b.b == true){
            eval(expr.op[1],symTable,routineList);
          }else if(expr.noperands == 3){ 
            eval(expr.op[2],symTable,routineList);
          }
          data res;
          res.type = no_op;
          return res;
      }
    break;
    case FOR: 
      { //statement
        symbol * s = getSymbolFromIdentifier(expr.op[0]->value.id,symTable);
        if(s == NULL){
          yyerror("variable not found");
          exit(NO_SUCH_VARIABLE);
        }

        assignment(s,expr.op[1],symTable,routineList);
        identifier index; index.name = malloc(strlen(s->name)+1);
        strcpy(index.name,s->name);
        data id = eval_identifier(index, symTable,routineList);
        //consider only LT
        data condition = eval(expr.op[2],symTable,routineList);
        data comparison = operation(LT, id, condition);
        // typechecking
        if(comparison.b.b == true){
          eval(expr.op[3],symTable,routineList);
          // assume only integers
          treeNode * nextValue = constantNode(basic_int_value, id.b.i + 1);
          //assignment(s,nextValue,symTable);
          treeNode * newFor = opr(FOR,4,expr.op[0],nextValue,expr.op[2],expr.op[3]);

          eval(newFor,symTable,routineList);
        }
      }
      break;
    case UMINUS:
    { data e1, e2;
      //typechecking 
      e1= eval(expr.op[0],symTable,routineList);
      return negate(e1);
    }
    case MINUS:
    { data e1, e2;
      //typechecking 
      e1= eval(expr.op[0],symTable,routineList);
      e2= eval(expr.op[1],symTable,routineList);
      return operation(MINUS, e1,e2);
    }
    break;

    case PLUS :
    { data e1, e2;
      //typechecking 
      e1= eval(expr.op[0],symTable,routineList);
      e2= eval(expr.op[1],symTable,routineList);
      return operation(PLUS, e1,e2);
    }
    break;

    case MULTIPLY:
    { data e1, e2;
      //typechecking 
      e1= eval(expr.op[0],symTable,routineList);
      e2= eval(expr.op[1],symTable,routineList);
      return operation(MULTIPLY, e1,e2);
    }
    break;

    case DIVIDE :
    { data e1, e2;
      //typechecking 
      e1= eval(expr.op[0],symTable,routineList);
      e2= eval(expr.op[1],symTable,routineList);
      return operation(DIVIDE, e1,e2);
    }
    break;
    case LT :
      { data e1, e2;
        //typechecking
        e1= eval(expr.op[0],symTable,routineList);
        e2= eval(expr.op[1],symTable,routineList);
        return operation(LT, e1,e2);
      }
      break;
    case GT :
      { data e1, e2;
        //typechecking
        e1= eval(expr.op[0],symTable,routineList);
        e2= eval(expr.op[1],symTable,routineList);
        return operation(GT, e1,e2);
      }
      break;
    case GE :
      { data e1, e2;
        //typechecking
        e1= eval(expr.op[0],symTable,routineList);
        e2= eval(expr.op[1],symTable,routineList);
        return operation(GE, e1,e2);
      }
      break;
    case LE :
      { data e1, e2;
        //typechecking
        e1= eval(expr.op[0],symTable,routineList);
        e2= eval(expr.op[1],symTable,routineList);
        return operation(LE, e1,e2);
      }
      break;
    case NE :
      { data e1, e2;
        //typechecking
        e1= eval(expr.op[0],symTable,routineList);
        e2= eval(expr.op[1],symTable,routineList);
        return operation(NE, e1,e2);
      }
      break;
    case EQ :
      { data e1, e2;
        //typechecking
        e1= eval(expr.op[0],symTable,routineList);
        e2= eval(expr.op[1],symTable,routineList);
        return operation(EQ, e1,e2);
      }
      break;
    case AND:
    { 
      data e1, e2; 
      e1= eval(expr.op[0],symTable,routineList);
      e2= eval(expr.op[1],symTable,routineList);
      //TODO : only int is allowed as of now
      return operation(AND, e1,e2);
    }
    case OR:
    { 
      data e1, e2; 
      e1= eval(expr.op[0],symTable,routineList);
      e2= eval(expr.op[1],symTable,routineList);
      //TODO : only int is allowed as of now
      return operation(OR, e1,e2);
    }
    default:
    yyerror("eval_expr new type not implemented");
    exit(NOT_IMPLEMENTED);
    break;
  }
  return res;
}

data eval(treeNode *p, symbol ** symTable,list * routineList) {
  data d;
  if(p == NULL){
    d.type = no_op;
    return d;
  }
  switch (p->type) {
    case const_type:      
      return eval_constants(p->value.con, symTable);
      break;
    case identifier_type:
      return eval_identifier(p->value.id,symTable,routineList);
      break;
    case expression_type:
      return eval_expr(p->value.expr,symTable,routineList);
      break;
    case routine_type:
      return eval_routine(p->value.routine,symTable,routineList);
      break;
    case identifier_declaration:
        return eval_identifier_declaration(p->value.dec,symTable,routineList);
      break;
    default:
      yyerror("new type is not handled in the evaluation funciton");
      exit(NOT_IMPLEMENTED);
      break;
  }
}

void printData(data d){
  switch (d.type) {
    case basic_dataType:
        {
          switch (d.b.type) {
            case basic_int_value:
              printf("%d\n", d.b.i);
              break;
            case basic_float_value:
              printf("%f\n", d.b.f);
              break;
            case basic_boolean_value:
              printf("%s\n", (d.b.b == true ? "TRUE" : "FALSE"));
              break;
            case basic_string_value:
              printf("%s\n", d.b.s);
              break;
            case undef:
            default:
              yyerror("printData unmanaged type in costant node");
              exit(ERROR);
              break;
          }
        }
      break;
    case complex_dataType:
      yyerror("printing a complex type is not yet implemented");
      exit(NOT_IMPLEMENTED);
      break;
    case procedure_type:
      yyerror("printing a procedure.. unreachable state");
      exit(ERROR);
      break;
    case no_op:
      yyerror("evaluation returned no operation");
      exit(ERROR);
      break;
  }
 }
 
 int addInt(int n, int m) {
     return n+m;
 }
 int subInt(int n, int m) {
     return n-m;
 }
 int mulInt(int n, int m) {
     return n*m;
 }
 //unexpected ceiling
 int divInt(int n, int m) {
     return n/m;
 }
 
 int andInt(int n, int m) {
    return n && m;
 }

 int orInt(int n, int m) {
    return n || m;
 }

 float addFloat(float n, float m) {
     return n+m;
 }
 float subFloat(float n, float m) {
     return n-m;
 }
 float mulFloat(float n, float m) {
     return n*m;
 }
 float divFloat(float n, float m) {
     return n/m;
 }
 char* addString(char* n,char* m){
   char* result = NULL;
   if(n != NULL && m != NULL){
     result = (char*)malloc((strlen(n)+strlen(m)+1)*sizeof(char));
     strcpy(result, n);
     strcat(result, m);
   }
   return result;
 }
 //==========================

 bool ltFloat(float n, float m) {
     return (n<m)?true:false;
 }
 bool gtFloat(float n, float m) {
     return (n>m)?true:false;
 }
 bool geFloat(float n, float m) {
     return (n>=m)?true:false;
 }
 bool leFloat(float n, float m) {
     return (n<=m)?true:false;
 }
 bool neFloat(float n, float m) {
     return (n!=m)?true:false;
 }
 bool eqFloat(float n, float m) {
     return (n==m)?true:false;
 }
 //==========================

 bool ltInt(int n, int m) {
     return (n<m)?true:false;
 }
 bool gtInt(int n, int m) {
     return (n>m)?true:false;
 }
 bool geInt(int n, int m) {
     return (n>=m)?true:false;
 }
 bool leInt(int n, int m) {
     return (n<=m)?true:false;
 }
 bool neInt(int n, int m) {
     return (n!=m)?true:false;
 }
 bool eqInt(int n, int m) {
    return (n==m)?true:false;
 }
 //==========================
 bool eqBool(bool n, bool m) {

     return (n==m)?true:false;
 }
 //assumption is that typechecking has already happened
 data operation(int oper, data e1, data e2){
   data res;
   basic b;
   //pointer to a funcion that takes two int input and return an int
   int (*funIntPtr)(int,int);
   float (*funFloatPtr)(float,float);
   char* (*funStringPtr)(char*,char*);
   bool (*funBoolFloatPrt)(float,float);
   bool (*funBoolIntPrt)(int,int);
   bool (*funBoolPtr)(bool,bool);

   funIntPtr = NULL;
   funFloatPtr= NULL;
   funStringPtr = NULL;
   funBoolFloatPrt = NULL;
   funBoolIntPrt = NULL;
     switch (oper) {
       case MINUS:
         switch (e1.b.type) {
           case basic_int_value:
             funIntPtr = subInt;
             break;
           case basic_float_value:
             funFloatPtr = subFloat;
             break;
           case basic_boolean_value:
             yyerror("boolean arithmetic operation is not allowed");
             exit(NOT_ALLOWED);
             break;
           case undef:
           default:
             yyerror("operation - unknown type");
             exit(ERROR);
             break;
           }
         break;
       case PLUS :
         switch (e1.b.type) {
           case basic_int_value:funIntPtr = addInt;break;
           case basic_float_value:funFloatPtr = addFloat;break;
           case basic_string_value:funStringPtr = addString;break;
           case basic_boolean_value:
             yyerror("boolean arithmetic operation are NOT_ALLOWED");
             exit(NOT_ALLOWED);
             break;
           case undef:
           default:
             yyerror("operation unmanaged type...");
             exit(ERROR);
             break;
         }
         break;
       case MULTIPLY:
         switch (e1.b.type) {
           case basic_int_value:
            funIntPtr = mulInt;
            break;
           case basic_float_value:
            funFloatPtr = mulFloat;
            break;
           case basic_boolean_value:
             yyerror("boolean arithmetic operation are NOT_ALLOWED");
             exit(NOT_ALLOWED);
             break;
           case undef:
           default:
             yyerror("operation unmanaged type.");
             exit(ERROR);
             break;
         }
         break;
       case DIVIDE :
         switch (e1.b.type) {
           case basic_int_value:funIntPtr = divInt;break;
           case basic_float_value:funFloatPtr = divFloat;break;
           case basic_boolean_value:
             yyerror("boolean arithmetic operation are NOT_ALLOWED");
             exit(NOT_ALLOWED);
             break;
           case undef:
           default:
            yyerror("operation unmanaged type.");
            exit(ERROR);
            break;
         }
         break;
       case LT :
         { switch (e1.b.type) {
           case basic_int_value:funBoolIntPrt = ltInt;break;
           case basic_float_value:funBoolFloatPrt = ltFloat;break;
           case basic_boolean_value:
             yyerror("boolean arithmetic operation are NOT_ALLOWED");
             exit(NOT_ALLOWED);
             break;
           case undef:
           default:
             yyerror("operation unmanaged type.");
             exit(ERROR);
             break;
           }
         }
         break;
       case GT :
         {switch (e1.b.type) {
           case basic_int_value:funBoolIntPrt = gtInt;break;
           case basic_float_value:funBoolFloatPrt = gtFloat;break;
           case basic_boolean_value:
             yyerror("boolean arithmetic operation are NOT_ALLOWED");
             exit(NOT_ALLOWED);
             break;
           case undef:
           default:
             yyerror("operation unmanaged type.");
             exit(ERROR);
             break;
           }
         }
         break;
       case GE :
         {switch (e1.b.type) {
           case basic_int_value:funBoolIntPrt = geInt;break;
           case basic_float_value:funBoolFloatPrt = geFloat;break;
           case basic_boolean_value:
             yyerror("boolean arithmetic operation are NOT_ALLOWED");
             exit(NOT_ALLOWED);
             break;
           case undef:
           default:
             yyerror("operation unmanaged type.");
             exit(ERROR);
             break;
           }
         }
         break;
       case LE :
         {switch (e1.b.type) {
           case basic_int_value:funBoolIntPrt = leInt;break;
           case basic_float_value:funBoolFloatPrt = leFloat;break;
           case basic_boolean_value:
             yyerror("boolean arithmetic operation are NOT_ALLOWED");
             exit(NOT_ALLOWED);
             break;
           case undef:
           default:
             yyerror("operation unmanaged type.");
             exit(ERROR);
             break;
           }
         }
         break;
       case NE :
          {
           switch (e1.b.type) {
             case basic_int_value:funBoolIntPrt = neInt;break;
             case basic_float_value:funBoolFloatPrt = neFloat;break;
             case basic_boolean_value:
               yyerror("boolean arithmetic operation are NOT_ALLOWED");
               exit(NOT_ALLOWED);
               break;
             case undef:
             default:
               yyerror("operation unmanaged type.");
               exit(ERROR);
               break;
             }
           }
         break;
       case EQ :
        {
          switch (e1.b.type) {
           case basic_int_value:funBoolIntPrt = eqInt;break;
           case basic_float_value:funBoolFloatPrt = eqFloat;break;
           case basic_boolean_value: funBoolPtr = eqBool;break;
           case undef:
           default:
             yyerror("operation unmanaged type.");
             exit(ERROR);
             break;
           }
         }
         break;
       case AND :
         switch (e1.b.type) {
           case 
               basic_int_value:funBoolIntPrt = andInt;
               break;
           case undef:
           default:
            yyerror("operation unmanaged type.");
            exit(ERROR);
            break;
         }
         break;
       case OR :
         switch (e1.b.type) {
           case 
               basic_int_value:funBoolIntPrt = orInt;
               break;
           case undef:
           default:
            yyerror("operation unmanaged type.");
            exit(ERROR);
            break;
         }
         break;
       default:
         yyerror("Error");
         exit(ERROR);
         break;
      }
    //look for values to be used
    if( funIntPtr == NULL && funFloatPtr == NULL &&
        funBoolIntPrt == NULL && funBoolFloatPrt == NULL &&
        funBoolPtr == NULL
        ){
      yyerror("no function found for the specified operation.");
      exit(ERROR);
    }
    res.type = basic_dataType;
    switch (e1.b.type) {
      case basic_int_value:
        {
          if(funIntPtr!=NULL){
            if(oper == EQUALS){
              res.b.type = basic_boolean_value;
              res.b.b = funIntPtr(e1.b.i, e2.b.i);
            }else{
              res.b.type = e1.b.type;//inherithed
              res.b.i = funIntPtr(e1.b.i, e2.b.i);
            }
          }else if(funBoolIntPrt!=NULL){
            //evaluating something like int < int or int == int
            res.b.type = basic_boolean_value;
            res.b.b = funBoolIntPrt(e1.b.i, e2.b.i);
          }else{
            yyerror("reached unexpected state.");
            exit(ERROR);
          }

        }
        break;
      case basic_float_value:
        {
          if(funFloatPtr!=NULL){
            if(oper == EQUALS){
              res.b.type = basic_boolean_value;
              res.b.b = funFloatPtr(e1.b.f, e2.b.f);
            }else{
              res.b.type = e1.b.type;
              res.b.f = funFloatPtr(e1.b.f, e2.b.f);
            }
          }else if(funBoolFloatPrt!=NULL){
            res.b.type = basic_boolean_value;
            res.b.b = funBoolFloatPrt(e1.b.f, e2.b.f);
          }else{
            yyerror("reached unexpected state.");
            exit(ERROR);
          }
        }
        break;
      case basic_string_value:
        if(funStringPtr!=NULL){
          if(oper == PLUS){
            res.b.type = basic_string_value;
            res.b.s = funStringPtr(e1.b.s, e2.b.s);
          }else{
            yyerror(" basic_string_value reached unexpected state.");
            exit(ERROR);          
          }
        }
        break;
      case basic_boolean_value:
        {
          if(funFloatPtr!=NULL || funBoolFloatPrt!=NULL ||
            funBoolIntPrt!=NULL ||  funIntPtr!=NULL){
              yyerror("reached unexpected state..");
              exit(ERROR);
          }else{
            res.b.type =  e1.b.type;//inherithed
            res.b.b = funBoolPtr(e1.b.b, e2.b.b);
          }
        }
        break;
      
      default:
        yyerror("no data found for the specified operation.");
        exit(ERROR);
    }
   return res;
 }

 data negate(data input){
   data res;
   res.type = input.type;
   basic b;
   switch (input.type) {
     case basic_dataType:
          {
            switch (input.b.type) {
              case basic_int_value: b.type = input.b.type; b.i = - input.b.i; break;
              case basic_float_value: b.type = input.b.type; b.f = - input.b.f; break;
              case basic_boolean_value: b.type = input.b.type; b.b =  input.b.b == true ? false:true; break;
              case undef:
              default:
                yyerror("negate unmanaged type.");
                exit(ERROR);
                break;
            }
          }
      break;
     case complex_dataType:
     default:
      yyerror("negating != basic data type is NOT_ALLOWED");
      exit(NOT_ALLOWED);
     break;
   }
   res.b = b;
   return res;
 }

data eval_identifier_declaration(declarationNode decl, symbol ** symbolTable,list * routineList){
    data res;
    res.type = no_op;
    symbol * s;
    s=getSym(decl.name, *symbolTable);
    if(s==0){
      s = createSym(decl.name, symbolTable);
      //insert
      switch (decl.t->dt) {
        case basic_dataType:
          {
            s->type = decl.t->dt;
            s->bType = decl.t->typeValue.bt;
            s->isCostant = decl.isCostant;
            if(s->isCostant){
              //assign the given expr
              data e = eval(decl.expr,symbolTable,routineList);
              //TODO typechecking
              switch (decl.t->typeValue.bt) {
                case basic_int_value:
                  (*s->value).i = e.b.i;
                  break;
                case basic_float_value:
                  (*s->value).f = e.b.f;
                  break;
                case basic_boolean_value:
                  (*s->value).i = e.b.i;
                  break;
                case undef:
                default:
                  yyerror("unmanaged type.");
                  exit(ERROR);
                  break;
              }
            }
          }
          break;
        case complex_dataType:
          yyerror("not implemented yet");
          exit(NOT_IMPLEMENTED);
          break;
        default:
          yyerror("unmanaged data type in identifier_declaration");
          exit(NOT_IMPLEMENTED);
      }
    }else{
      yyerror("variable name already in use.");
      exit(NAME_ALREADY_IN_USE);
    }
    return res;
}

data spec_assignment(symbol * variable, data e, symbol ** symTable ,list * routineList){
  if(variable == NULL){
    yyerror("variable not found");
    exit(NO_SUCH_VARIABLE);
  }
  data res;
  res.type = no_op;
  //assign the variable the given value
  char temp[20];
  switch (variable->type) {
    case basic_dataType:
      {
        switch (variable->bType) {
          case basic_boolean_value:
            (*variable->value).b = e.b.b;
            break;
          case basic_int_value:
            (*variable->value).i = e.b.i;
            break;
          case basic_float_value:
            (*variable->value).f = e.b.f;
            break;
          case basic_string_value:
            (*variable->value).s = e.b.s;
            break;
          case undef:
          default:
            yyerror("assignment unmanaged type.");
            exit(ERROR);
            break;
        }
      }
      break;
    case complex_dataType:
      {
        yyerror("assigning complex data type is not yet implemented");
        exit(NOT_IMPLEMENTED);
      }
      break;
    default:
      {
      yyerror("assigning procedure or no_op to varibale");
      exit(ERROR);
      }
      break;
  }
return res;
}


data assignment(symbol * variable, treeNode * expr, symbol ** table, list * routineList){
    if(variable == NULL){
      yyerror("variable not found");
      exit(NO_SUCH_VARIABLE);
    }
    //typechecking 
    data e = eval(expr, table,routineList);
        
    if(variable->bType != e.b.type){
      yyerror("Incorrect datatype for assignment");
      exit(ERROR);
    }
  return spec_assignment(variable,e,table,routineList);
}
//changes the value pointed by r return value
data r_assignment(routine * r, treeNode * expr, symbol ** symTable,list * routineList){
  if(r == NULL){
    yyerror("routine not found");
    exit(NO_SUCH_VARIABLE);
  }

  data e = eval(expr, symTable,routineList);
  //typechecking
  r->returnValue = dataToDataPointer(e);
  data res;
  res.type = no_op; //is the last action that should be executed in the program
  return res;
}

data eval_identifier(identifier identifierNode, symbol ** symbolTable,list * routineList){
  //TODO: place a check for not initialized values
  data res;
  symbol * s = getSym(identifierNode.name, *symbolTable);
  if(s == 0){
    //TODO: place variable name in output error
    yyerror("variable not found in environment");
    exit(NO_SUCH_VARIABLE);
  }
  res.type = s->type;
  switch (s->type) {
    case basic_dataType:
      { basic b;
        b.type = s->bType;
        switch (s->bType) {
          case basic_boolean_value:
            b.b = (*s->value).b;
            break;
          case basic_int_value:
            b.i = (*s->value).i;
            break;
          case basic_float_value:
            b.f = (*s->value).f;
            break;
          case basic_string_value:
            b.s = (*s->value).s;
            break;
          case undef:
          default:
            yyerror("eval_identifier unknown type.");
            exit(ERROR);
            break;
        }
        res.b = b;        
      }
      break;
    case complex_dataType:
      yyerror("assigning complex data type is not yet implemented");
      exit(NOT_IMPLEMENTED);
      break;
    case no_op:
      break;
    case procedure_type:
      yyerror("assigning complex data type is not yet implemented");
      exit(NOT_IMPLEMENTED);
      break;
  }
  return res;
}

 data eval_routine(routineNode rout, symbol ** symTable, list * routineList){
   data res;
   routine * r = getRoutine(rout.name, routineList);
   if(r==NULL){
      yyerror("accessing unexisting procedure or function..");
      exit(NO_SUCH_PROCEDURE);
   }
   form * forms;
   forms = r->parameters;
   actual * aes;
   aes = rout.args;
   // typechecking
   int flen = formLength(forms);
   int alen = actLength(aes);
   if(flen!=alen){
     yyerror("args len is different from formal length");
   }
   symbol * rSymbol;
   rSymbol = NULL;

   for(int i = 0; i< flen; i++){
     form * f = getFormAtIndex(i,forms);
     if(f == NULL){
       //TODO place here the number of forms already used
       yyerror("execeed form list");
       exit(FORM_LIST_EXCEEDED);
     }
     actual * a = getActualAtIndex(i,aes);
     if (a == NULL){
       //TODO place here the number of args already used
       yyerror("execeed args list");
       exit(ARGS_LIST_EXCEEDED);
     }
    
     type * t = basicDec(f->bt); //TODO: considered only basic type
     treeNode * dec = varDec(f->name,false,t);
     eval_identifier_declaration(dec->value.dec,&rSymbol,routineList);
     identifier id; id.name = malloc(strlen(f->name)+1);
     strcpy(id.name,f->name);

     if (f->byref){
       if (a->expr->type != identifier_type){
         yyerror("incompatible arg for pass by ref");
         exit(ARGS_LIST_EXCEEDED);
       }
      symbol * tmp = getSymbolFromIdentifier(id,&rSymbol);
      symbol * tmp2 = getSymbolFromIdentifier(a->expr->value.id, symTable);
      tmp->value = tmp2->value;
     } else {
       //eval expr in current env, then assign it
       symbol * tmp = getSymbolFromIdentifier(id,&rSymbol);
       data e = eval(a->expr, symTable,routineList);
       spec_assignment(tmp,e, &rSymbol, routineList);
     }
   }
  
  eval(r->statementList, &rSymbol,routineList);

  if(r->type == procedure){
    res.type = no_op;
  }else{    
    res = *r->returnValue;
    r->returnValue = NULL;
  }
  return res;
}
