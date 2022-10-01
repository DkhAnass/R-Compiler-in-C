#include "analyseurlex.c"
#include "analyseursem.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEFT    0
#define RIGHT   1
#define EQUAL   2

#define NORMAL 0
#define CONDIT 1

#define ISID        2
#define ISCALLFUNC  3
#define ISDATA      4
#define ISOPERATION 5


int main(){

    program = fopen("test_1.txt", "r");
    if(program == NULL){
        perror("Erreur lors de l'ouverture du fichier");
        exit(1);
    }
    else{
        printf("\n\n|____________________________ DÉBUT DE COMPILATION ____________________________|\n");

        Char_suivant();
        Token_suivant();

        while(Token.Type != END){
            if(est_boucle()){}
            else if(est_Decision()){}
            else if(est_Fonction()){}
            else if(est_Expr()){}
            else{
                Erreur_Syntax("\nExpression Invalide\n");
            }
        }
        fclose(program);
    }
    printf("\n\n\n\n\n");
    printSymbolTable();
    printf("\n\n|____________________________ FIN DE COMPILATION ____________________________|\n\n");
    return 0;
}


/*Declarations*/

Symbol tempSymbol;
int IDorDataorCallFuncorOP;

/// Tokens Test
int TestToken(TOKEN_CODE TokenToTest){
    if(Token.Type != TokenToTest)
        return FALSE;
    // SUCCES END
    Token_suivant();
    return TRUE;
}

/// End Expr
void EndExpr(){
    if(Nv_ligne == TRUE){
        return;
    }
    else if(TestToken(SEMICOLON))   {
        return;
    }
    else{
        Erreur_Syntax("Fin d'expression");
    }
}

/// Pas de nouveu ligne
void NoNewline(){
    if(Nv_ligne == TRUE) Erreur_Syntax("Nouvelle ligne Invalide");
}




/**
    est_Expr:       id <- ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]*
                | id <- function
                | id -> id
                | function -> id
                | ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]* -> id
**/
int est_Expr(){
    IDorDataorCallFuncorOP = ID_Appfct_Donnee_Operation();
    if(IDorDataorCallFuncorOP == ISID){
        Symbol IDSymbol = Symb_courant;

        // id -> id
        if(AssignerOp(RIGHT)){
            if(!est_ID()) Erreur_Syntax("Incorrect assign data");
            SemanticID(IDSymbol, Symb_courant);
            // SUCCES END
            EndExpr();
            return TRUE;
        }
        else if(AssignerOp(LEFT) || AssignerOp(EQUAL) ){
            int operation = FALSE;
            int n = 0;
            Symbol assignLeft;
            memset(assignLeft.Name, '\0', lenValue);
            memset(assignLeft.Value, '\0', lenValue);
            //id <- function
            if( est_Fonction() ){
                return TRUE;
            }
            //id <- ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]*
            IDorDataorCallFuncorOP = ID_Appfct_Donnee_Operation();

            if(IDorDataorCallFuncorOP == ISID){
                SemanticID(tempSymbol, IDSymbol);
            }
            else if(IDorDataorCallFuncorOP == ISDATA) {
                SemanticData(tempSymbol, IDSymbol);
            }
            else if(IDorDataorCallFuncorOP == ISCALLFUNC){
            }
            else if(IDorDataorCallFuncorOP == ISOPERATION){
                SemanticData(tempSymbol, IDSymbol);
            }
            EndExpr();
            // SUCCES END
            return TRUE;
        }
    }
    /// Données ou opération
    else if( IDorDataorCallFuncorOP == ISDATA ){
        NoNewline();
        if(!AssignerOp(RIGHT)) Erreur_Syntax("Incorrect Assign Data");

        if(!est_ID()) Erreur_Syntax("Incorrect Assign Data");

        SemanticData(tempSymbol,Symb_courant);
        // SUCCES END
        EndExpr();
        return TRUE;
    }
    else if( IDorDataorCallFuncorOP == ISOPERATION ){

        if(!AssignerOp(RIGHT)) Erreur_Syntax("Incorrect Assign Data");
        NoNewline();
        if(!est_ID()) Erreur_Syntax("Incorrect Assign Data");

        SemanticData(tempSymbol, Symb_courant);
        // SUCCES END
        EndExpr();
        return TRUE;
    }
    /// CallFucntion
    else if(IDorDataorCallFuncorOP == ISCALLFUNC ){

        if(AssignerOp(RIGHT)){
            NoNewline();
            if(!est_ID()) Erreur_Syntax("Incorrect Assign Data");
        }
        else if(AssignerOp(LEFT)){
            Erreur_Syntax("Incorrect Assign Call Function");
        }
        // SUCCES END
        EndExpr();
        return TRUE;
    }
    else{return FALSE;}
}

/**
    est_Decision:   if ( condition ) { expr }
                | if ( condition ) { expr } else { expr }
**/
int est_Decision(){

    if(!TestToken(IF)) return FALSE;
    NoNewline();
    if(!TestToken(OPEN_PAR)) Erreur_Syntax("Open Parentheses");
    NoNewline();
    if(!est_Condition()) Erreur_Syntax("Invalid Condition");
    if(!TestToken(CLOSE_PAR)) Erreur_Syntax("Close Parentheses");
    if(!TestToken(OPEN_BRACE)) Erreur_Syntax("Open Brace");
    while(est_Expr());
    if(!TestToken(CLOSE_BRACE)) Erreur_Syntax("Close Brace");

    // ELSE Statement
    if(TestToken(ELSE)){
        if(!TestToken(OPEN_BRACE)) Erreur_Syntax("Open Brace");
        while(est_Expr());
        if(!TestToken(CLOSE_BRACE)) Erreur_Syntax("Close Brace");
    }

    return TRUE;
}

int est_Fonction(){

    if(!TestToken(FUNCTION)) return FALSE;
    NoNewline();
    if(!TestToken(OPEN_PAR)) Erreur_Syntax("Invalid Function");
    NoNewline();
    if(est_ID()){
        NoNewline();
        while(TestToken(COLON)){
            NoNewline();
            if(!est_ID())
                Erreur_Syntax("Args Function");
            NoNewline();
        }
    }
    NoNewline();
    if(!TestToken(CLOSE_PAR)) Erreur_Syntax("Args Function");
    if(!TestToken(OPEN_BRACE)) Erreur_Syntax("Invalid Function");
    while(est_Retourner() || est_Expr() );
    if(! TestToken(CLOSE_BRACE)) Erreur_Syntax("Invalid Function");

    if(AssignerOp(RIGHT)) {
        NoNewline();
        if(!est_ID()) Erreur_Syntax("Incorrect Assign Function");
    }
    // SUCCES END
    EndExpr();
    return TRUE;
}

int est_Retourner(){
    if( !TestToken(RETURN)) return FALSE;
    NoNewline();
    IDorDataorCallFuncorOP = ID_Appfct_Donnee_Operation();
    if( IDorDataorCallFuncorOP != ISID  && IDorDataorCallFuncorOP != ISDATA   &&    IDorDataorCallFuncorOP != ISOPERATION )
        Erreur_Syntax("Bad return");

    // SUCCES END
    EndExpr();
    return TRUE;
}

/**
    est_boucle:     for ( ID in vector ) { expr }
                |   while condition { expr }
                |   repeat { expr }
**/
int est_boucle(){

    switch(Token.Type){

    /// for ( ID in vector ) { expr }
    case FOR:
        Token_suivant();
        NoNewline();
        if(!TestToken(OPEN_PAR)) Erreur_Syntax("Invalid For");
        NoNewline();
        if(!est_ID()) Erreur_Syntax("ID in For");
        NoNewline();
        if(!TestToken(IN)) Erreur_Syntax("IN in For");
        NoNewline();
        if( ! (est_vecteur() || est_ID()) )Erreur_Syntax("Not vector in For");
        NoNewline();
        if(!TestToken(CLOSE_PAR)) Erreur_Syntax("Invalid For");

        if(!TestToken(OPEN_BRACE)) Erreur_Syntax("Invalid For");
        while(est_Expr());
        if(!TestToken(CLOSE_BRACE)) Erreur_Syntax("Invalid For");

        // SUCCES END
        return TRUE;
        break;

    /// while ( condition ) { expr }
    case WHILE:
        Token_suivant();
        NoNewline();
        if(!TestToken(OPEN_PAR)) Erreur_Syntax("Invalid While");
        NoNewline();
        if(!est_Condition()) Erreur_Syntax("Not Condition");
        NoNewline();
        if(!TestToken(CLOSE_PAR)) Erreur_Syntax("Invalid While");
        if(!TestToken(OPEN_BRACE)) Erreur_Syntax("Invalid While");
        while(est_Expr());
        if(!TestToken(CLOSE_BRACE)) Erreur_Syntax("Invalid While");

        // SUCCES END
        return TRUE;
        break;

    /// repeat { expr }
    case REPEAT:
        Token_suivant();
        if(!TestToken(OPEN_BRACE)) Erreur_Syntax("Invalid Repeat loop");
        while(est_Expr());
        if(!TestToken(CLOSE_BRACE)) Erreur_Syntax("Invalid Repeat loop");

        // SUCCES END
        return TRUE;
        break;
    /// Not a Loop statement
    default: return FALSE;
    }
}
/** is Vector
    vector : c( id [, id]* )
**/
int est_vecteur(){
    if(!TestToken(VECTOR)) return FALSE;
    if(!TestToken(OPEN_PAR)) Erreur_Syntax("Invalid Vector");
    if(!est_Donnee()) Erreur_Syntax("Invalid Vector");
    while(TestToken(COLON))
        if(!est_Donnee())   Erreur_Syntax("Invalid Vector");
    if(!TestToken(CLOSE_PAR)) Erreur_Syntax("Invalid Vector");
    return TRUE;
}

/* ID */
int est_ID(){
    if(!TestToken(ID)) return FALSE;
    // SUCCES END
    return TRUE;
}

/* <- -> = */
int AssignerOp(int assignop){

    switch(assignop){
    case LEFT: if(!TestToken(LEFT_ASSIGN)) return FALSE;
        break;
    case RIGHT: if(!TestToken(RIGHT_ASSIGN)) return FALSE;
        break;
    case EQUAL: if(!TestToken(EQUAL_ASSIGN)) return FALSE;
        break;
    }
    // SUCCES END
    return TRUE;
}

/* condition:   ( id | CallFunction | data ) operator (id | CallFunction | data ) */
int est_Condition(){

    if(! ( est_ID() || Appel_fct() || est_Donnee() ) ) Erreur_Syntax("Incorrect Condition");
    NoNewline();
    if(!  est_Operateur(CONDIT)) Erreur_Syntax("Incorrect Operator for Condition");
    NoNewline();
    if(! ( est_ID() || Appel_fct() || est_Donnee() ) ) Erreur_Syntax("Incorrect Condition");

    // SUCCES END
    return TRUE;
}

/**
isID_CallFunc_Data_Operation :  ( id | CallFunction | data )  [operator (id | CallFunction | data) ]*
**/
int ID_Appfct_Donnee_Operation(){

    int a = 0 ,b = 0, DeclaredID = TRUE;
    int cntPar = 0;
    int operation = FALSE;
    memset(tempSymbol.Name, '\0', lenValue);
    memset(tempSymbol.Value, '\0', lenValue);

    // Verify if there is an open parentheses
    while(TestToken(OPEN_PAR)) {
        NoNewline();
        strcat(tempSymbol.Value , Symb_courant.Value);
        cntPar++;
    }

    if(est_ID()){
        IDorDataorCallFuncorOP = ISID;
        NoNewline();
        // Verify if it's undeclared in case we have an operation
        if( ( a= ID_Declarer(Symb_courant) ) == UNDECLARED)
                DeclaredID = FALSE;
        else{
            tempSymbol.Type = SymbTable[a].Type;
            strcat(tempSymbol.Value , SymbTable[a].Value);
        }
    }
    else if(Appel_fct()){IDorDataorCallFuncorOP = ISCALLFUNC;}
    else if(est_Donnee()){
        strcat(tempSymbol.Value , Symb_courant.Value);
        tempSymbol.Type = Symb_courant.Type;
        IDorDataorCallFuncorOP = ISDATA;
    }
    else {return FALSE;}

    // Verify if we have a close parentheses
    while(TestToken(CLOSE_PAR)) {
        NoNewline();
        strcat(tempSymbol.Value , Symb_courant.Value);
        cntPar--;
    }

    // Verify if it's an Operation
    while(est_Operateur(NORMAL) == TRUE){
        NoNewline();
        //Verify if the previous ID is already declared
        if(DeclaredID == FALSE) ERRORSemantic("Undeclared Variable");

        // Add the Operator
        strcat(tempSymbol.Value, Symb_courant.Value);

        // Open parentheses after the operator
        while(TestToken(OPEN_PAR)) {
            NoNewline();
            strcat(tempSymbol.Value , Symb_courant.Value);
            cntPar++;
        }

        if(est_ID() ){
            //Verify if it's declared and it's type
            if( ( b= ID_Declarer(Symb_courant) ) == UNDECLARED) ERRORSemantic("Undeclared Id");
            if(SymbTable[b].Type != tempSymbol.Type) ERRORSemantic("Type mismatch");

            strcat(tempSymbol.Value , SymbTable[b].Value);

        }else if(est_Donnee()){
            if(Symb_courant.Type != tempSymbol.Type)  ERRORSemantic("Type mismatch");
            strcat(tempSymbol.Value , Symb_courant.Value);
        }else if(Appel_fct()){

        }else{
            Erreur_Syntax("Operation");
        }

        while(TestToken(CLOSE_PAR)){
            NoNewline();
            strcat(tempSymbol.Value , Symb_courant.Value);
            cntPar--;
        }

        operation = TRUE;
    }

    if(cntPar != 0) Erreur_Syntax("Error in Parentheses");

    // If not operation
    if(!operation) return IDorDataorCallFuncorOP;
    // If it's an operation
    return ISOPERATION;
}

/* Operator : == | < | <= | > | >= | + | - | * | /    */
int est_Operateur(int type){
    ///Normal Operator
    if(type == NORMAL){
        if( !( TestToken(PLUS_OP)|| TestToken(MINUS_OP) || TestToken(DIV_OP) || TestToken(MULT_OP) ) )  return FALSE;
    }
    ///Condition Operator
    else if(type == CONDIT){
        if( !( TestToken(EQUAL_OP) || TestToken(INF_OP) || TestToken(INF_EQ_OP) || TestToken(SUP_OP) || TestToken(SUP_EQ_OP) ) )  return FALSE;
    }

    // SUCCES END
    return TRUE;
}

/* CallFunc: callfunc( data | [,data] ) **/
int Appel_fct(){
    if(!TestToken(CALLFUNC))    return FALSE;
    NoNewline();
    if(!TestToken(OPEN_PAR)) Erreur_Syntax("Invalid CallFunction");
    NoNewline();
    // Arguments
    if(est_Donnee() || est_ID()){
        NoNewline();
        while(TestToken(COLON)){
            NoNewline();
            if(! (est_Donnee() || est_ID()) ) Erreur_Syntax("Invalid Args for CallFunction");
        }
    }
    NoNewline();
    if(!TestToken(CLOSE_PAR)) Erreur_Syntax("Invalid CallFunction");
    // SUCCES END
    return TRUE;
}

/* data:       INT | FLOAT | STRING | VECTOR   */
int est_Donnee(){
    if(!(TestToken(INT) || TestToken(FLOAT) || TestToken(STRING) || est_vecteur() ) )  return FALSE;
    // SUCCES END
    return TRUE;
}