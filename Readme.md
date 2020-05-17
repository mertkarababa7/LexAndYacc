# Group Members:

Mert KARABABA
Enes Salim ERDEM
Recai Kaan KANATLI

# Referance
https://cse.iitkgp.ac.in/~bivasm/notes/LexAndYaccTutorial.pdf
# Run codes
 after clone
 1. make clean
 2. make
 3. run the test files with ./calculator
 
 example usage =>>
 
 make clean
 
 clean
 
 ./calculator ./testForWhile  && ./calculator ./testDivFunction etc.
 
# Sample examples


### Comment

/* commented */

### Assign some values

float a; a = 22.00;

int b; b = 6;

string s1; s1 = "AKDENIZ CSE ";


### if else operators
 
    if(i1 == 0){
        do something;
    } else {
        do something;
    }
    
  ### for & while  operators 
  
 
    int k; k = y;   
    while(k >= 0){
       do something;
    }
    
    int j; j = x;
    for (j = x to y){
        do something;
    }
    
   ### Functions
   
    (all values can be int or float)
    multiply(int x, int y, pointer int result){
    result = x*y; }
    add(int x, int y, pointer int result){
    result = x+y; }
    divide(int x, int y, pointer int result){
    result = x/y; }
    /* print function (with integer) */
    printFn(int x){
    print x;
    
   ### Exceptions
     while we are assigning int value we should give an int vlue.
     while we are assinging float value we should give a float value.
     we should give exact data type to not get error.
     
     
    
  
    
 
     

# Language Definition

### Alphabet

* (`a-z`) of the English alphabet
* numeric digits (`0-9`)

### Lexic

* special symbols, including:
  * operators `+ - * / 
  *  boolean operators `AND: '&&' ; OR: '||' ; ASSIGNMENT: '=='  
  * separators `( ) { } ;
  
* identifiers
  * a sequence of letters and digits, such that the first charater is a letter; the rule is:
    ```
    identifier = letter | letter{letter}{digit}
    letter = lowercase_letter
    lowercase_letter = "a" | "b" | ... | "z"
    digit = "0" | "1" | ... | "9"
    ```

  * character
    ```
    character = 'letter' | 'digit'
    ```
  * bool
    ```
    bool = false | true
    ```




