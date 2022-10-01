#include "global.h"

/** Variables **/

char Char_courant;  // Char_courant
int Nv_ligne;       // Nv_ligne
int ligne = 1;
Symbol  Symb_courant;  //  Symb_courant
Symbol  Token;  

char Char_suivant(){     // avoir le charactere suivant 
    Char_courant = getc(program);
    return (Char_courant);
}

/// Erreur syntax fonction : 
void Erreur_Syntax(char* ErreurTexte){
    printf("\n\n %s \t", ErreurTexte);
    printf("Syntax Erreur !! ");
    printf("\t in line : %d \n\n" , ligne);
    exit(1);
}

    // avoir le token suivant 
void Token_suivant(){
    Symb_courant = Token;
    // Clear Token
    memset(Token.Name, '\0', lenValue);
    memset(Token.Value, '\0', lenValue);

    // Ignorer tous les séparateurs
    ignorer_Separateurs();

    if(Char_courant == EOF){ Token.Type = END; return; }
    else if( est_Nombre() ){}
    else if( est_Mot() ){}
    else if( est_Special() ){}
    else{ Erreur_Syntax("Token non valid"); }

    //Print Tokens
    if(Nv_ligne == TRUE)
        printf("\n");
    printf("%s\t", Token_String[Token.Type]);

return;
}
/**
    Ignorer les espaces, 9les tabulations, les retours au ligne et les commentaires.
**/


void ignorer_Separateurs(){
    Nv_ligne = FALSE;
    while( Char_courant == ' ' || Char_courant == '\t' || Char_courant == '\n' || Char_courant == '#'){
        if(Char_courant == '\n'){
            ligne++;
            Nv_ligne = TRUE;
        }
        else if(Char_courant == '#'){
            while( Char_courant != '\n' && Char_courant != EOF){
                Char_suivant();
                if(Char_courant == '\n'){
                   ligne++;
                }

            }
        }
        Char_suivant();
    }
}

/**
    INT:    digit+
    FLOAT:  digit+ . digit*
**/
int est_Nombre(){
    // Si c'est pas un caractère
    if( !isdigit(Char_courant) )
        return FALSE;

    // Construction du nombreConstructing our Number Symbol
    int position = 0;
    Token.Value[position]= Char_courant;
    Char_suivant();
    //atteint la fin du nombre
    while ( isdigit(Char_courant) ){
        Token.Value[++position] = Char_courant;
        Char_suivant();
    }
    Token.Type = INT;
    // Si c'est un réel
    if(Char_courant == '.'){
        Token.Value[++position] = Char_courant;
        Token.Type = FLOAT;
        Char_suivant();
        while ( isdigit(Char_courant) ){
            Token.Value[++position] = Char_courant;
            Char_suivant();
        }
    }
    // un nombre ne contient alphabet
    if( isalpha(Char_courant) )  Erreur_Syntax("Nombre invalide");

    // Nous pouvons trouver plusieurs extrémités de l’opérateur
    int charNormal=FALSE;
    char ignorer_[] = {' ', ',', '(', ')', '\n', EOF, '+','-','*','/', '{', '}',';', '\t'};

    for(int j=0; j<14; j++){
        if(Char_courant == ignorer_[j]){
            charNormal = TRUE;
        }
    }
    if(charNormal == FALSE)
        Erreur_Syntax("Nombre invalide");

    // SUCCES END
    return TRUE;
}

/**
            ***** Mot : ***********
**/
int est_Mot(){
    // si le premier mot est une lettre 
    if( !isalpha(Char_courant) )
        return FALSE;

    char* instNom[]=       {"while",   "repeat", "for", "in" , "if", "else", "function", "return", "print", "readline"};
    TOKEN_CODE instToken[]= {WHILE,     REPEAT,    FOR,  IN,    IF,   ELSE,   FUNCTION,     RETURN,   PRINT,  READLINE};

    //construire le mot 
    int position = 0;
    char word[40];
    memset(word, '\0', 40);
    word[position++] = Char_courant;
    // si c'est un vecteur c( , , , )
    if(Char_courant == 'c'){
        Char_suivant();
        if(Char_courant == '('){
            Token.Type = VECTOR;
            return TRUE;
        }
    }else{
        Char_suivant();
    }
    // construire le mot 
    while (isalnum(Char_courant) || Char_courant == '_'){
        word[position++] = Char_courant;
       Char_suivant();
    }

    // Verifier si c'est une instruction
    for(int i=0; i < 9; i++){
        if (!strcmp(word, instNom[i])){
            Token.Type = instToken[i];
            return TRUE;
        }
    }

    // verifier s'il s'agit d'une fonction appellante 
    if(Char_courant == '('){
        Token.Type = CALLFUNC;
        strcpy(Token.Name, word);
        return TRUE;
       }

    // If not all of this, it is just a an ID
    Token.Type = ID;
    strcpy(Token.Name, word);

    //SUCCES END
    return TRUE;
}

int Operateur(){
    char operateur[] = {'+','-','*','/'};
    for(int i=0; i<4; i++){
        if(Char_courant == operateur[i]){
            Char_suivant();
            return TRUE;
        }
    }
    return FALSE;
}

/// Verifier s'il s'agit d'un caractere special 
int est_Special(){
    // Special :  <- | -> | = | ( | ) | { | } | ; | == | < | > | <= | >= | + | - | * | /
    char* special_Chars[] =       {   "<-",        "->",        "=",        "(",        ")",      "{",          "}",          ";",      "==",     "<",    ">",    "<=",      ">=",      "+",    "-",       "*",    "/",      "[",            "]",        ","};
    TOKEN_CODE special_Token[] =   {LEFT_ASSIGN, RIGHT_ASSIGN, EQUAL_ASSIGN, OPEN_PAR, CLOSE_PAR, OPEN_BRACE, CLOSE_BRACE, SEMICOLON, EQUAL_OP, INF_OP, SUP_OP, INF_EQ_OP, SUP_EQ_OP, PLUS_OP, MINUS_OP, MULT_OP, DIV_OP, OPEN_BRACKET, CLOSE_BRACKET, COLON};

    // Construire notre caractere special 
    char special[3];
    special[0] = Char_courant;
    special[1] = '\0';

    // Verifier s'il s'agit d'une chaine de caracteres 
    if(Char_courant == '\"'){
        // construction de notre chaine de caracteres 
        char text[100];
        memset(text, '\0', 100);
        int position = 0;
        // Arriver a la fin du chaine de caractere
        Char_suivant();
        while(Char_courant != '\"'){
            if(Char_courant == EOF || Char_courant == '\n' )
                Erreur_Syntax("String not completed");

            text[position++] = Char_courant;
            Char_suivant();
        }
        Char_suivant();
        Token.Type = STRING;
        strcpy(Token.Value, text);

        return TRUE;
    }

    // s'il se compose de deux characteres comme :  == | <= | >= | <- | ->
    Char_suivant();
    if(Char_courant == '=' || Char_courant == '-' || Char_courant == '>' ){
        special[1] = Char_courant;
        special[2]= '\0';
    }

    //  chercher le symbol special qui convient 
    for(int i = 0; i < 20 ; i++){
        if( !strcmp(special_Chars[i], special) ){
            Token.Type = special_Token[i];
            strcpy(Token.Value, special);
            // s'il ya deux caracteres dans un symbol on  besoin de passer au caractere suivant 
            // sinon donc c'est deja fait
            if(special[1] != '\0'){
                Char_suivant();
            }
            return TRUE;
        }
    }
    return FALSE;
}