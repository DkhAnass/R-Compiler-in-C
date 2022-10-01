// Les definitions : 
#define Already 0
#define TRUE    1
#define FALSE   0

/// les includes : 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tokens
typedef enum
{
ID,
/// les types des variables 
INT,
FLOAT,
STRING,
VECTOR,
/// Conditions if / else 
IF,
ELSE,
/// les boucles
WHILE,
REPEAT,
FOR,
IN,
/// Fonction
FUNCTION,
CALLFUNC,
RETURN,
PRINT,
READLINE,
/// Symbols Speciales
LEFT_ASSIGN,
RIGHT_ASSIGN,
EQUAL_ASSIGN,
OPEN_PAR,
CLOSE_PAR,
OPEN_BRACKET,
CLOSE_BRACKET,
OPEN_BRACE,
CLOSE_BRACE,
SEMICOLON,
EQUAL_OP,
INF_OP,
SUP_OP,
INF_EQ_OP,
SUP_EQ_OP,
PLUS_OP,
MINUS_OP,
MULT_OP,
DIV_OP,
COLON,
END
} TOKEN_CODE;

char* Token_String[] ={
"ID",
///  les types des variables
"INT",
"FLOAT",
"STRING",
"VECTOR",
/// Condition  If/else
"IF",
"ELSE",
/// Les Boucles 
"WHILE",
"REPEAT",
"FOR",
"IN",
/// Function
"FUNCTION",
"CALLFUNC",
"RETURN",
"PRINT",
"READLINE",
/// Symbols Speciales
"LEFT_ASSIGN",
"RIGHT_ASSIGN",
"EQUAL_ASSIGN",
"OPEN_PAR",
"CLOSE_PAR",
"OPEN_BRACKET",
"CLOSE_BRACKET",
"OPEN_BRACE",
"CLOSE_BRACE",
"SEMICOLON",
"EQUAL_OP",
"INF_OP",
"SUP_OP",
"INF_EQ_OP",
"SUP_EQ_OP",
"PLUS_OP",
"MINUS_OP",
"MULT_OP",
"DIV_OP",
"COLON",
"END"
};




// Declarations : 

FILE *program;

////////// table de symboles
#define lenValue    40
#define lenName     40
#define MaxSymbols  50

typedef struct{
    TOKEN_CODE  Type;
    char Name[lenName];
    char Value[lenValue];
}Symbol;

Symbol SymbTable[MaxSymbols];
int nSymbol = 0;

//Prototypages des fonctions

void ignorer_Separateurs();
int est_Special();
int est_Mot();
int est_Nombre();
int isdigit();
int isalpha();
int isalnum();
int ID_Declarer();
int est_boucle();
int est_Decision();
int est_Fonction();
int est_Expr();
int ID_Appfct_Donnee_Operation();
int AssignerOp();
int est_ID();
int est_Condition();
int est_Retourner();
int est_vecteur();
int est_Donnee();
int est_Operateur();
int Appel_fct();