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

*** Output

![Scanning Example#1](https://user-images.githubusercontent.com/90295968/206789503-d179dbea-5b00-4109-af29-69971c83102d.jpg)
![Scanning Example#2](https://user-images.githubusercontent.com/90295968/206789484-f5c3ca2a-e6dd-48a9-91c9-269a868aac18.jpg)
***************************************************************************************************
Assignment-2  ( Parsing )



***************************************************************************************************
Assignment-3  ( Code Generator )


