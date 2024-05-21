%{
  #include <stdio.h>
  #include <ctype.h>
  #include <string.h>

  #include "asa.h"
  #include "semantic.h"
  #include "codegen.h"

  extern int yylex();
  static void print_file_error(char * s, char *errmsg);

  table_symb TABLE_SYMBOLES = NULL;
  struct asa * ARBRE_ABSTRAIT = NULL;

  void yyerror(const char * s);

  char srcname[64];
  char exename[64] = "a.out";
  FILE * exefile;
%}

//union pour associer les différents types pour constuire la structure d'arbre syntaxique

%union{
  int nb;
  char id[32];
  struct asa* arbre;
 };

%define parse.error verbose
%locations

//Définitions des tokens associés aux lexèmes de notre analyseur lexical (flex)
%token <nb> NB
%token <id> ID
%token DEBUT FIN VAR
%token SI ALORS SINON FSI TQ FAIRE FTQ
%token FLECHE "<-"
%right FLECHE
%token ALGO
%token INF_EQ
%token SUP_EQ
%token PROGRAMME
%token B_TRUE B_FALSE
%token LIRE
%token ECRIRE

//Directive pour déclarer les types associés aux règles de grammaire pour notre analyse syntaxique
%type <arbre> PROGRAMME_ALGO
%type <arbre> PROG
%type <arbre> EXP
%type <arbre> DECLA_VAR
%type <arbre> LIST_DECLA_VAR
%type <arbre> LIST_ID
%type <arbre> LIST_INSTS
%type <arbre> L_PARAM
%type <arbre> AFFECTATION
%type <arbre> DECLA_FCT
%type <arbre> DECLA_GBL
%type <arbre> INST
%type <arbre> COND
%type <arbre> BOUCLE


//Définition de la priorité des opérateurs pour éviter les ambiguïtés dans la grammaire
%left '+' '-'
%left '*' '/' '%'
%left '(' ')'
%left INF_EQ SUP_EQ DIFT '<' '>' 
%left ET OU NON


//Directive de départ (non-terminal)
%start PROGRAMME_ALGO

%%



PROGRAMME_ALGO :
DECLA_GBL             
PROG                  { $$ = creer_noeudPROGAlgo($1, $2); }
;

PROG :
 PROGRAMME '(' ')' SEP
 LIST_DECLA_VAR
DEBUT SEP
   LIST_INSTS       
FIN EOP                     {$$=creer_noeudFCT("PROGRAMME", NULL, $5, $8); semantic($$); codegen($$);}
;

//Règle définissant la séparation de lignes du fichier (pas d'action ou de structure liée)
SEP: '\n'                {  }
| SEP '\n'               {  }
;

//Règle définissant la fin du programme (ex: programme vide ou finissant par \n)
EOP: %empty              {  }
| SEP                    {  }
;


L_PARAM: %empty         { $$ = NULL;}
| LIST_ID               { $$ = $1;}
;

//Règle définissant la déclaration d'une fonction ou d'une variable globale
DECLA_GBL: %empty       { $$ = NULL;}
|DECLA_VAR DECLA_GBL    { $$ = creer_noeudDECLA_VAR($1, $2, NULL);}
|DECLA_FCT DECLA_GBL    { $$ = creer_noeudDECLA_VAR($1, $2, NULL);}
;

//Règle définissant la déclaration d'une fonction
DECLA_FCT: 
ALGO ID '(' L_PARAM ')' SEP    
LIST_DECLA_VAR                      
DEBUT SEP
  LIST_INSTS                    
FIN SEP                    {$$ =creer_noeudFCT($2, $4, $7, $10);}
;

//Règle définissant une liste de déclarations de plusieurs variables
LIST_DECLA_VAR: %empty       {$$=NULL;}
| DECLA_VAR  LIST_DECLA_VAR  {$$=creer_noeudDECLA_VAR($1, $2, NULL);}
;

//Règle définissant la déclaration de variables
DECLA_VAR:
 VAR LIST_ID SEP { $$=$2; print_asa($$);}
;

//Règle définissant une liste d'idenficateurs dans une déclaration (ex : VAR toto, titi, tutu...)
LIST_ID: ID                 { $$ = creer_noeudDECLA_VAR(creer_feuilleID($1), NULL, NULL);}
| ID ',' LIST_ID            { $$ = creer_noeudDECLA_VAR(creer_feuilleID($1), NULL, $3);}
| ID "<-" EXP               { $$ = creer_noeudDECLA_VAR(creer_feuilleID($1), $3, NULL);}
| ID "<-" EXP ',' LIST_ID   { $$ = creer_noeudDECLA_VAR(creer_feuilleID($1),$3,$5);}
;

/*
Règle définissant une liste d'instructions
La récursivité permet une COND, une BOUCLE ou une INST (INSTRUCTION) d'apparaitre dans une autre.
*/
LIST_INSTS: INST SEP           { $$ = creer_noeudLIST_INSTS($1, NULL);}
| INST SEP LIST_INSTS          { $$ = creer_noeudLIST_INSTS($1, $3);}
;

INST: EXP SEP {print_asa($1);}
| BOUCLE 
| COND 
| AFFECTATION
;

// À prévoir: autres types de boucles (Pour, Do While etc...) code évolutif dans asa.h
BOUCLE: TQ EXP FAIRE 
LIST_INSTS 
FTQ {$$ = creer_noeudBOUCLE(typeTQ,$2,$4);}
;

COND: SI EXP ALORS LIST_INSTS SINON LIST_INSTS FSI {$$ = creer_noeudCOND($2,$4,$6);}
|SI EXP ALORS LIST_INSTS FSI {$$ = creer_noeudCOND($2,$4,NULL);}
;

// Règle définissant l'affectation qui associe une expression à un identificateur
AFFECTATION:
ID "<-" EXP SEP          { $$ = creer_noeudAFFEC(creer_feuilleID($1),NULL);}
| ID "<-" AFFECTATION    { $$ = creer_noeudAFFEC(creer_feuilleID($1),$3);}
;

EXP : NB                 { $$ = creer_feuilleNB($1); }
| EXP '+' EXP            { $$ = creer_noeudOP('+', $1, $3); }
| EXP '-' EXP            { $$ = creer_noeudOP('-', $1, $3); }
| EXP '*' EXP            { $$ = creer_noeudOP('*', $1, $3); }
| EXP '/' EXP            { $$ = creer_noeudOP('/', $1, $3); }
| EXP '%' EXP            { $$ = creer_noeudOP('%', $1, $3); }
| EXP '<' EXP            { $$ = creer_noeudOP('<', $1, $3); }
| EXP '>' EXP            { $$ = creer_noeudOP('>', $1, $3); }
| EXP INF_EQ EXP         { $$ = creer_noeudCMP("<=", $1, $3); }
| EXP SUP_EQ EXP         { $$ = creer_noeudCMP(">=", $1, $3); }
| EXP DIFT EXP           { $$ = creer_noeudCMP("!=", $1, $3); }
| EXP ET EXP             { $$ = creer_noeudOPL("&&", $1, $3); }
| EXP OU EXP             { $$ = creer_noeudOPL("||", $1, $3); }
| NON EXP                { $$ = creer_noeudOPL('!', $2, NULL); }
| '(' EXP ')'            { $$ = $2;  }
| ID                     { $$ = creer_feuilleID($1); }
| B_TRUE                 { $$ = creer_feuilleBOOL(1);}
| B_FALSE                { $$ = creer_feuilleBOOL(0);}
| LIRE                   { pass(); }
| ECRIRE                 { pass(); }
;




%%

int main( int argc, char * argv[] ) {
  extern FILE *yyin;

  if (argc > 1){
    strcpy(srcname, argv[1]);
    if ( (yyin = fopen(srcname,"r"))==NULL ){
      char errmsg[256];
      sprintf(errmsg,"fichier \x1b[1m\x1b[33m' %s '\x1b[0m introuvable",srcname);
      print_file_error(argv[0],errmsg);
      exit(1);
    }
  }  else {
    print_file_error(argv[0],"aucun fichier en entree");
    exit(1);
  }
  if (argc == 3){
    strcpy(exename, argv[2]);
  }
  exefile = fopen(exename,"w");
  TABLE_SYMBOLES = ts_init_table("global");
  yyparse();
  fclose(yyin);
}



static void print_file_error(char * prog, char *errmsg){
  fprintf(stderr,
	  "\x1b[1m%s:\x1b[0m \x1b[31m\x1b[1merreur fatale:\x1b[0m %s\nechec de la compilation\n",
	  prog, errmsg);
}

void yyerror(const char * s)
{
  fprintf(stderr, "\x1b[1m%s:%d:%d:\x1b[0m %s\n", srcname, yylloc.first_line, yylloc.first_column, s);
  exit(0);
}
