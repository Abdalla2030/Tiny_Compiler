# Tiny_Compiler
Assignments of Compilers Course ( CS423 )

Assignments
***************************************************************************************************
Assignment-1  ( Scanning )

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
Assignment-2  ( Parsing )

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
Assignment-3  ( Code Generator )


