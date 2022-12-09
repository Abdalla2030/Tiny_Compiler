# Tiny_Compiler
Assignments of Compilers Course ( CS423 )

# Assignments
***************************************************************************************************
### Assignment-1  ( Scanning )

Write a program in C++ that implements the scanning(Tokenization) step of a custom compiler built to compile the TINY programming language 

Program input: The scanner will take a source code file written in the TINY programming language as an input; You can find an example of an input file in the file called `input.txt` 

Program output: The scanner should output a text file that contains the tokens found in the input file with each token represented on a single line in the following format 
[number of line the token was found in starting from 1 in square brackets] [the actual string that represents the token in the input file] [a string that represents the token type in round brackets] 

TINY language rules:
- sequence of statements separated by ;
- Comments {}
- I/O read write
- math expressions integers only, + - * / ^
- boolean only in if and repeat conditions < = and two mathematical expressions
- repeat-statement: repeat until (boolean)
- if-statement: if (boolean) then [else] end
- variable names can include only alphabetic characters(a:z or A:Z) and underscores
- variables are declared simply by assigning values to them :=
- all variables are integers
- no procedures - no declarations

###### Output

![Scanning Example#1](https://user-images.githubusercontent.com/90295968/206789503-d179dbea-5b00-4109-af29-69971c83102d.jpg)
![Scanning Example#2](https://user-images.githubusercontent.com/90295968/206789484-f5c3ca2a-e6dd-48a9-91c9-269a868aac18.jpg)
***************************************************************************************************
### Assignment-2  ( Parsing )

Write a program in C++ that implements the analyzer step of our custom compiler of the TINY programming language based on the grammar rules of the language.

Program input: The scanner will take the path of a source code file written in the TINY programming language as an input; You can find an example of an input file in the file called `input.txt`.


- You will also find a suggested data structure to the tree node that you may use, however, this's not mandatory. You may create a different format to the tree nodes as long as your code is clean, readable, performant and follows the SOLID principles.

Program output: The program should output the terminal(leaf) nodes of the parse tree of the input file to the terminal or throw an exception once an error was found. You may use the `printTree` method found in the attached file called `CompilersTask_2_Parser.cpp` or if you decide to represent the tree nodes with a different data structure make sure your output follows the same format as in this method.

- The number of spaces to output before each node represents the level on which the node relies on(The order of the rule the node represented in related to the program node which is level 0).

- Eech level in the tree is 3 spaces apart than its prior.

## TINY language grammar rules:

- program -> stmtseq
- stmtseq -> stmt { ; stmt }
- stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
- ifstmt -> if expr then stmtseq [ else stmtseq ] end
- repeatstmt -> repeat stmtseq until expr
- assignstmt -> identifier := expr
- readstmt -> read identifier
- writestmt -> write expr
- expr -> mathexpr [ (<|=) mathexpr ]
- mathexpr -> term { (+|-) term }    left associative
- term -> factor { (*|/) factor }    left associative
- factor -> newexpr { ^ newexpr }    right associative
- newexpr -> ( mathexpr ) | number | identifier

###### Output
![Parsing Example](https://user-images.githubusercontent.com/90295968/206790840-433aeaa6-56bf-4c74-89ae-dc41fce3b6ff.jpg)
***************************************************************************************************
### Assignment-3  ( Analyzing )

Write a program in C++ that implements the code generation step of our custom compiler of the TINY programming language.

Program input: for this assignment you have the freedom to either take the path of the source code file written in the TINY programming language as an input or hardcode it within your code; You can find an example of an input file in the file called `input.txt`.

Program output: The program should output 

- First: The symbol table of the code basically containing all the named variables in the following format for each variable

[Var=variable name][Mem=initial value of the variable(default is 0, unless the variable was initialized with a different value in an assignment statment][Line=the first line number the variable appears in][Line=the second line number the variable appears in]  and so on for all the other lines the variable is referenced in 

example of a symbol table:
[Var=x][Mem=1][Line=6][Line=7][Line=20]
[var=y][Mem=0][Line=1]

- Second: The syntax tree; which is basically the same output of the previous assignment but with each non void(Integer or Boolean) node followed by its data type in square brackets.

 All operators are considered of Integer types except for the LessThan and Equal operators; they're considered of Boolean type.
 Numbers and IDs are also considered of Integer Types.
 The rest of the node types can be considered of Void types, which doesn't need to get printed(the data type not the entire node).

example of an Integer node: [ID][x][Integer]
example of a Boolean node: [Oper][LessThan][Boolean]
example of a void node: [Read][x]

note: In this step you're also expected to preform type checking, so for example if an 'if' is followed by something other than a Boolean node, print to the terminal that there is a problem with that explaining the issue. So something along the lines of "Error, an "if" should be followed by a Boolean".



***************************************************************************************************
### Assignment-4  ( Code Generator )


