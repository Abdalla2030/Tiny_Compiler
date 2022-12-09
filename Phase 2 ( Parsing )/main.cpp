#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
using namespace std;

/*
{ Sample program
  in TINY language
  compute factorial
}

read x; {input an integer}
if 0<x then {compute only if x>=1}
  fact:=1;
  repeat
    fact := fact * x;
    x:=x-1
  until x=0;
  write fact {output factorial}
end
*/

/// sequence of statements separated by ;
/// no procedures - no declarations
/// all variables are integers
/// variables are declared simply by assigning values to them :=
/// if-statement: if (boolean) then [else] end
/// repeat-statement: repeat until (boolean)
/// boolean only in if and repeat conditions < = and two mathematical expressions
/// math expressions integers only, + - * / ^
/// I/O read write
/// Comments {}

////////////////////////////////////////////////////////////////////////////////////
/// Strings /////////////////////////////////////////////////////////////////////////

bool Equals(const char* a, const char* b)
{
    return strcmp(a, b)==0;
}

bool StartsWith(const char* a, const char* b)
{
    int nb=strlen(b);
    return strncmp(a, b, nb)==0;
}

void Copy(char* a, const char* b, int n=0)
{
    if(n>0) {strncpy(a, b, n); a[n]=0;}
    else strcpy(a, b);
}

void AllocateAndCopy(char** a, const char* b)
{
    if(b==0) {*a=0; return;}
    int n=strlen(b);
    *a=new char[n+1];
    strcpy(*a, b);
}

////////////////////////////////////////////////////////////////////////////////////
/// Input and Output ////////////////////////////////////////////////////////////////

#define MAX_LINE_LENGTH 10000

struct InFile
{
    FILE* file;
    int cur_line_num;

    char line_buf[MAX_LINE_LENGTH];
    int cur_ind, cur_line_size;

    InFile(const char* str) {file=0; if(str) file=fopen(str, "r"); cur_line_size=0; cur_ind=0; cur_line_num=0;}
    ~InFile(){if(file) fclose(file);}

    void SkipSpaces()
    {
        while(cur_ind<cur_line_size)
        {
            char ch=line_buf[cur_ind];
            if(ch!=' ' && ch!='\t' && ch!='\r' && ch!='\n') break;
            cur_ind++;
        }
    }

    bool SkipUpto(const char* str)
    {
        while(true)
        {
            SkipSpaces();
            while(cur_ind>=cur_line_size) {if(!GetNewLine()) return false; SkipSpaces();}

            if(StartsWith(&line_buf[cur_ind], str))
            {
                cur_ind+=strlen(str);
                return true;
            }
            cur_ind++;
        }
        return false;
    }

    bool GetNewLine()
    {
        cur_ind=0; line_buf[0]=0;
        if(!fgets(line_buf, MAX_LINE_LENGTH, file)) return false;
        cur_line_size=strlen(line_buf);
        if(cur_line_size==0) return false; // End of file
        cur_line_num++;
        return true;
    }

    char* GetNextTokenStr()
    {
        SkipSpaces();
        while(cur_ind>=cur_line_size) {if(!GetNewLine()) return 0; SkipSpaces();}
        return &line_buf[cur_ind];
    }

    void Advance(int num)
    {
        cur_ind+=num;
    }
};

struct OutFile
{
    FILE* file;
    OutFile(const char* str) {file=0; if(str) file=fopen(str, "w");}
    ~OutFile(){if(file) fclose(file);}

    void Out(const char* s)
    {
        fprintf(file, "%s\n", s); fflush(file);
    }
};

////////////////////////////////////////////////////////////////////////////////////
/// Compiler Parameters /////////////////////////////////////////////////////////////

struct CompilerInfo
{
    InFile in_file;
    OutFile out_file;
    OutFile debug_file;

    CompilerInfo(const char* in_str, const char* out_str, const char* debug_str)
                : in_file(in_str), out_file(out_str), debug_file(debug_str)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////
/// Scanner /////////////////////////////////////////////////////////////////////////

#define MAX_TOKEN_LEN 40

enum TokenType{
                IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,TYPES,
                ASSIGN, EQUAL, LESS_THAN,
                PLUS, MINUS, TIMES, DIVIDE, POWER,
                SEMI_COLON,
                LEFT_PAREN, RIGHT_PAREN,
                LEFT_BRACE, RIGHT_BRACE,
                ID, NUM,
                ENDFILE, ERROR,
                INT,BOOL,REAL
              };

/// Used for debugging only /////////////////////////////////////////////////////////
const char* TokenTypeStr[]=
            {
                "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write", "Types",
                "Assign", "Equal", "LessThan",
                "Plus", "Minus", "Times", "Divide", "Power",
                "SemiColon",
                "LeftParen", "RightParen",
                "LeftBrace", "RightBrace",
                "ID", "Num",
                "EndFile", "Error",
                "Int","Bool","Real"
            };

struct Token
{
    TokenType type;
    char str[MAX_TOKEN_LEN+1];

    Token(){str[0]=0; type=ERROR;}
    Token(TokenType _type, const char* _str) {type=_type; Copy(str, _str);}
};

const Token reserved_words[]=
{
    Token(IF, "if"),
    Token(THEN, "then"),
    Token(ELSE, "else"),
    Token(END, "end"),
    Token(REPEAT, "repeat"),
    Token(UNTIL, "until"),
    Token(READ, "read"),
    Token(WRITE, "write"),
    Token(TYPES, "types"),
    Token(INT, "int"),
    Token(BOOL, "bool"),
    Token(REAL, "real")
};
const int num_reserved_words=sizeof(reserved_words)/sizeof(reserved_words[0]);

// if there is tokens like < <=, sort them such that sub-tokens come last: <= <
// the closing comment should come immediately after opening comment
const Token symbolic_tokens[]=
{
    Token(ASSIGN, ":="),
    Token(EQUAL, "="),
    Token(LESS_THAN, "<"),
    Token(PLUS, "+"),
    Token(MINUS, "-"),
    Token(TIMES, "*"),
    Token(DIVIDE, "/"),
    Token(POWER, "^"),
    Token(SEMI_COLON, ";"),
    Token(LEFT_PAREN, "("),
    Token(RIGHT_PAREN, ")"),
    Token(LEFT_BRACE, "{"),
    Token(RIGHT_BRACE, "}")
};
const int num_symbolic_tokens=sizeof(symbolic_tokens)/sizeof(symbolic_tokens[0]);

inline bool IsDigit(char ch){return (ch>='0' && ch<='9');}
inline bool IsLetter(char ch){return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'));}
inline bool IsLetterOrUnderscore(char ch){return (IsLetter(ch) || ch=='_');}
/////////////////////////////////////////////////////////////////////////////
void GetNextToken(CompilerInfo* pci, Token* ptoken)
{
    ptoken->type=ERROR;
    ptoken->str[0]=0;

    int i;
    char* s=pci->in_file.GetNextTokenStr();
    if(!s)
    {
        ptoken->type=ENDFILE;
        ptoken->str[0]=0;
        return;
    }

    for(i=0;i<num_symbolic_tokens;i++)
    {
        if(StartsWith(s, symbolic_tokens[i].str))
            break;
    }

    if(i<num_symbolic_tokens)
    {
        if(symbolic_tokens[i].type==LEFT_BRACE)
        {
            pci->in_file.Advance(strlen(symbolic_tokens[i].str));
            if(!pci->in_file.SkipUpto(symbolic_tokens[i+1].str)) return;
            return GetNextToken(pci, ptoken);
        }
        ptoken->type=symbolic_tokens[i].type;
        Copy(ptoken->str, symbolic_tokens[i].str);
    }
    else if(IsDigit(s[0]))
    {
        int j=1;
        while(IsDigit(s[j])) j++;

        ptoken->type=NUM;
        Copy(ptoken->str, s, j);
    }
    else if(IsLetterOrUnderscore(s[0]))
    {
        int j=1;
        while(IsLetterOrUnderscore(s[j])) j++;

        ptoken->type=ID;
        Copy(ptoken->str, s, j);

        for(i=0;i<num_reserved_words;i++)
        {
            if(Equals(ptoken->str, reserved_words[i].str))
            {
                ptoken->type=reserved_words[i].type;
                break;
            }
        }
    }
    /*
    else if(ptoken->type=ERROR){
            Copy(ptoken->str, s,1);
    }
    */
    int len=strlen(ptoken->str);
    if(len>0) pci->in_file.Advance(len);
}
////////////////////////////////////////////////////////////////////////////////////
// Parser //////////////////////////////////////////////////////////////////////////

// program -> stmtseq
// stmtseq -> stmt { ; stmt }
// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
// ifstmt -> if exp then stmtseq [ else stmtseq ] end
// repeatstmt -> repeat stmtseq until expr
// assignstmt -> identifier := expr
// readstmt -> read identifier
// writestmt -> write expr
// expr -> mathexpr [ (<|=) mathexpr ]
// mathexpr -> term { (+|-) term }    left associative
// term -> factor { (*|/) factor }    left associative
// factor -> newexpr { ^ newexpr }    right associative
// newexpr -> ( mathexpr ) | number | identifier
enum NodeKind{
                IF_NODE, REPEAT_NODE, ASSIGN_NODE, READ_NODE, WRITE_NODE,
                OPER_NODE, NUM_NODE, ID_NODE
             };

// Used for debugging only /////////////////////////////////////////////////////////
const char* NodeKindStr[]=
            {
                "If", "Repeat", "Assign", "Read", "Write",
                "Oper", "Num", "ID"
            };

enum ExprDataType {VOID, INTEGER, BOOLEAN};

// Used for debugging only /////////////////////////////////////////////////////////
const char* ExprDataTypeStr[]=
            {
                "Void", "Integer", "Boolean"
            };

#define MAX_CHILDREN 3

struct TreeNode
{
    TreeNode* child[MAX_CHILDREN];
    TreeNode* sibling; // used for sibling statements only

    NodeKind node_kind;

    union{TokenType oper; int num; char* id;}; // defined for expression/int/identifier only
    ExprDataType expr_data_type; // defined for expression/int/identifier only

    int line_num;

    TreeNode() {int i; for(i=0;i<MAX_CHILDREN;i++) child[i]=0; sibling=0; expr_data_type=VOID;}
};

struct ParseInfo
{
    Token next_token;
};
/////////////////////////////////////////////////////////////////////////////
void MatchNextToken(CompilerInfo* , ParseInfo* ,TokenType);
TreeNode* Parsing(CompilerInfo* );
TreeNode* StmtSeq(CompilerInfo* ,ParseInfo* );
TreeNode* Stmt(CompilerInfo* ,ParseInfo* );
TreeNode* IfStmt(CompilerInfo* ,ParseInfo* );
TreeNode* RepeatStmt(CompilerInfo* ,ParseInfo* );
TreeNode* AssignStmt(CompilerInfo* ,ParseInfo* );
TreeNode* ReadStmt(CompilerInfo* ,ParseInfo* );
TreeNode* WriteStmt(CompilerInfo* ,ParseInfo* );
TreeNode* Expr(CompilerInfo* ,ParseInfo* );
TreeNode* MathExpr(CompilerInfo* ,ParseInfo* );
TreeNode* Term(CompilerInfo* ,ParseInfo* );
TreeNode* Factor(CompilerInfo* ,ParseInfo* );
TreeNode* NewExpr(CompilerInfo* ,ParseInfo* );
TreeNode* Expr(CompilerInfo*, ParseInfo*);
/////////////////////////////////////////////////////////////////////////////
void MatchNextToken(CompilerInfo* Tiny, ParseInfo* parserToken,TokenType expectedNextTokenType){
    if(parserToken->next_token.type != expectedNextTokenType){
        throw 0;
    }
    GetNextToken(Tiny, &parserToken->next_token);
}
/////////////////////////////////////////////////////////////////////////////
/// program -> stmtseq
TreeNode* Parsing(CompilerInfo* Tiny){
    ParseInfo parserToken;
    GetNextToken(Tiny, &parserToken.next_token);
    TreeNode* parse_tree = new TreeNode;
    parse_tree = StmtSeq(Tiny,&parserToken);
    if(parserToken.next_token.type != ENDFILE){
        cout<<"There are error in code"<<endl;////////////////
    }
    return parse_tree;
}
/////////////////////////////////////////////////////////////////////////////
/// stmtseq -> stmt { ; stmt }
TreeNode* StmtSeq(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* first_tree = new TreeNode;
    first_tree = Stmt(Tiny, parserToken);
    TreeNode* second_tree = new TreeNode;
    second_tree = first_tree;
    while(parserToken->next_token.type != ENDFILE && parserToken->next_token.type != END &&
          parserToken->next_token.type != ELSE && parserToken->next_token.type!= UNTIL){
        MatchNextToken(Tiny, parserToken, SEMI_COLON);
        TreeNode* third_tree = new TreeNode;
        third_tree = Stmt(Tiny, parserToken);
        second_tree->sibling = third_tree;
        second_tree = third_tree;
    }
    return first_tree;
}
/////////////////////////////////////////////////////////////////////////////
/// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
TreeNode* Stmt(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree=new TreeNode;
    tree = 0;
    if(parserToken->next_token.type==IF) tree = IfStmt(Tiny, parserToken);
    else if(parserToken->next_token.type==REPEAT) tree = RepeatStmt(Tiny, parserToken);
    else if(parserToken->next_token.type==ID) tree = AssignStmt(Tiny, parserToken);
    else if(parserToken->next_token.type==READ) tree = ReadStmt(Tiny, parserToken);
    else if(parserToken->next_token.type==WRITE) tree = WriteStmt(Tiny, parserToken);
    else throw 0;

    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// ifstmt -> if exp then stmtseq [ else stmtseq ] end
TreeNode* IfStmt(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree = new TreeNode;
    tree->node_kind=IF_NODE;
    tree->line_num=Tiny->in_file.cur_line_num;

    MatchNextToken(Tiny, parserToken, IF);
    tree->child[0] = Expr(Tiny, parserToken);
    MatchNextToken(Tiny, parserToken, THEN);
    tree->child[1]= StmtSeq(Tiny, parserToken);
    if(parserToken->next_token.type==ELSE){
        MatchNextToken(Tiny, parserToken, ELSE);
        tree->child[2] = StmtSeq(Tiny, parserToken);
        //tree->child[0]->sibling =  StmtSeq(Tiny, parserToken);
    }
    MatchNextToken(Tiny, parserToken, END);
    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// repeatstmt -> repeat stmtseq until expr
TreeNode* RepeatStmt(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree = new TreeNode;
    tree->node_kind=REPEAT_NODE;
    tree->line_num=Tiny->in_file.cur_line_num;

    MatchNextToken(Tiny, parserToken, REPEAT);
    tree->child[0] = StmtSeq(Tiny, parserToken);
    MatchNextToken(Tiny, parserToken, UNTIL);
    tree->child[1] = Expr(Tiny, parserToken);

    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// assignstmt -> identifier := expr
TreeNode* AssignStmt(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree=new TreeNode;
    tree->node_kind = ASSIGN_NODE;
    tree->line_num=Tiny->in_file.cur_line_num;

    if(parserToken->next_token.type==ID){ // read variable
       AllocateAndCopy(&tree->id, parserToken->next_token.str);
    }
    MatchNextToken(Tiny, parserToken, ID);
    MatchNextToken(Tiny, parserToken, ASSIGN);
    tree->child[0]= Expr(Tiny, parserToken);

    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// readstmt -> read identifier
TreeNode* ReadStmt(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree = new TreeNode;
    tree->node_kind = READ_NODE;
    tree->line_num = Tiny->in_file.cur_line_num;

    MatchNextToken(Tiny, parserToken, READ);
    if(parserToken->next_token.type==ID){ // read variable
        AllocateAndCopy(&tree->id, parserToken->next_token.str);
    }
    MatchNextToken(Tiny, parserToken, ID);
    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// writestmt -> write expr
TreeNode* WriteStmt(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree=new TreeNode;
    tree->node_kind = WRITE_NODE;
    tree->line_num=Tiny->in_file.cur_line_num;
    MatchNextToken(Tiny, parserToken, WRITE);
    tree->child[0] = Expr(Tiny, parserToken);
    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// expr -> mathexpr [ (<|=) mathexpr ]
TreeNode* Expr(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree = new TreeNode; ////////
    tree = MathExpr(Tiny, parserToken);
   //   [ (<|=) mathexpr ] --> is optional
    if(parserToken->next_token.type == LESS_THAN || parserToken->next_token.type == EQUAL ){
        TreeNode* new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE;
        new_tree->oper = parserToken->next_token.type;
        new_tree->line_num = Tiny->in_file.cur_line_num;
        /////////////////////////////////////////
        new_tree->child[0] = tree;
        MatchNextToken(Tiny, parserToken, parserToken->next_token.type);
        new_tree->child[1]=MathExpr(Tiny, parserToken);

        return new_tree;
    }
    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// mathexpr -> term { (+|-) term }    left associative
TreeNode* MathExpr(CompilerInfo* Tiny, ParseInfo* parserToken){

    TreeNode* tree = new TreeNode;  ////////
    tree  = Term(Tiny, parserToken);
    // zero or more
    while(parserToken->next_token.type == PLUS || parserToken->next_token.type == MINUS){
        TreeNode* new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE; // + - * / ^
        new_tree->oper = parserToken->next_token.type;
        new_tree->line_num = Tiny->in_file.cur_line_num;
       ///////////////////////
        new_tree->child[0] = tree;
        MatchNextToken(Tiny, parserToken, parserToken->next_token.type);
        new_tree->child[1] = Term(Tiny, parserToken);

        tree = new_tree;
    }
    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// term -> factor { (*|/) factor }    left associative
TreeNode* Term(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree = new TreeNode;
    tree = Factor(Tiny, parserToken);
     // { (*|/) factor }    --> zero or more
    while(parserToken->next_token.type == TIMES || parserToken->next_token.type == DIVIDE){
        TreeNode* new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE;
        new_tree->oper = parserToken->next_token.type;
        new_tree->line_num = Tiny->in_file.cur_line_num;
        /////////////////////////
        new_tree->child[0] = tree;
        MatchNextToken(Tiny, parserToken, parserToken->next_token.type);
        new_tree->child[1] = Factor(Tiny, parserToken);
        tree = new_tree;
    }
    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// factor -> newexpr { ^ newexpr }    right associative
TreeNode* Factor(CompilerInfo* Tiny, ParseInfo* parserToken){
    TreeNode* tree = new TreeNode;
    tree = NewExpr(Tiny, parserToken);

    if(parserToken->next_token.type == POWER){
        TreeNode* new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE;
        new_tree->oper = parserToken->next_token.type;
        new_tree->line_num = Tiny->in_file.cur_line_num;
        //////////////////////////
        new_tree->child[0] = tree;
        MatchNextToken(Tiny, parserToken, parserToken->next_token.type);
        new_tree->child[1] = Factor(Tiny, parserToken); // { ^ newexpr } --> zero or more
        return new_tree;
    }
    return tree;
}
/////////////////////////////////////////////////////////////////////////////
/// newexpr -> ( mathexpr ) | number | identifier
TreeNode* NewExpr(CompilerInfo* Tiny, ParseInfo* parserToken){
    // if next token is number
    if(parserToken->next_token.type == NUM ){
        TreeNode* tree = new TreeNode;
        tree->node_kind = NUM_NODE;
        tree->num = atoi(parserToken->next_token.str);
        tree->line_num=Tiny->in_file.cur_line_num;
        MatchNextToken(Tiny, parserToken, parserToken->next_token.type);
        return tree;
    }
    // if next token is identifier
    if(parserToken->next_token.type == ID){
        TreeNode* tree = new TreeNode;
        tree->node_kind = ID_NODE;
        AllocateAndCopy(&tree->id, parserToken->next_token.str);
        tree->line_num=Tiny->in_file.cur_line_num;
        MatchNextToken(Tiny, parserToken, parserToken->next_token.type);
        return tree;
    }
       // if next token is -->( mathexpr )
    if(parserToken->next_token.type == LEFT_PAREN){
        MatchNextToken(Tiny, parserToken, LEFT_PAREN);
        TreeNode* tree = new TreeNode; //////////
        tree = Expr(Tiny, parserToken); // Expr or MathExpr ??
        MatchNextToken(Tiny, parserToken, RIGHT_PAREN);
        return tree;
    }
    // if not follow grammar
    throw 0;
    return 0;
}
/////////////////////////////////////////////////////////////////////////////
void PrintTree(TreeNode* node, int sh=0)
{
    int i, NSH=3;
    for(i=0;i<sh;i++) printf(" ");

    printf("[%s]", NodeKindStr[node->node_kind]);

    if(node->node_kind==OPER_NODE) printf("[%s]", TokenTypeStr[node->oper]);
    else if(node->node_kind==NUM_NODE) printf("[%d]", node->num);
    else if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE) printf("[%s]", node->id);

    if(node->expr_data_type!=VOID) printf("[%s]", ExprDataTypeStr[node->expr_data_type]);

    printf("\n");

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) PrintTree(node->child[i], sh+NSH);
    if(node->sibling) PrintTree(node->sibling, sh);
}

////////////////////////////////////////////////////////////////////////////
/*void StartScanning(CompilerInfo* CompilerObj){

    Token TokenObj;

    cout<<("========== Scanning ==========")<<endl;
    cout<<endl;
    CompilerObj->out_file.Out("========== Scanning ==========");
    while(true){
        GetNextToken(CompilerObj,&TokenObj);
        cout<<"["<<CompilerObj->in_file.cur_line_num<<"] "<<TokenObj.str<<" ["<<TokenTypeStr[TokenObj.type]<<"]";

        // write in file
        string temp_str="["+to_string(CompilerObj->in_file.cur_line_num)+"] "+TokenObj.str+" ["+TokenTypeStr[TokenObj.type]+"]"; //converting number to a string
        char const* char_array= temp_str.c_str(); //converting string to char
        CompilerObj->out_file.Out(char_array);

        cout<<endl;
        if(TokenObj.type==ENDFILE){   // stop scanning
            cout<<endl;
            cout<<("=============================");
            CompilerObj->out_file.Out("=============================");
            break;
        }
    }
}*/
/////////////////////////////////////////////////////////////////////////////
void StartParsing(CompilerInfo* Tiny){
     TreeNode* parse_tree = new TreeNode;
     parse_tree = Parsing(Tiny);
     cout<<("========== Parsing ==========")<<endl;
     PrintTree(parse_tree);
     cout<<("=============================");
}
/////////////////////////////////////////////////////////////////////////////
int main(){

    CompilerInfo Tiny("input.txt", "output.txt", "debug.txt");
    //StartScanning(&Tiny);
     StartParsing(&Tiny);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
