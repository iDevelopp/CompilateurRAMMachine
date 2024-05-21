#ifndef ASA_H
#define ASA_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define  MAX_SIZE_FIELD_NAME  12
#define  MAX_SIZE_FIELD_VAL   32
#define  TXT_RED    "\x1b[31m"
#define  TXT_GREEN  "\x1b[32m"
#define  TXT_BLUE   "\x1b[34m"
#define  TXT_BOLD   "\x1b[1m"
#define  TXT_NULL   "\x1b[0m"

#define TYPE_ENTIER  'e'
#define TYPE_TABLEAU 't'
#define TYPE_PTR     'p'
#define TYPE_FCT     'f'

typedef enum {typeNB, typeOP, typeID, typeDECLA_VAR, typeAFFECTATION, typeFCT, typeLIST_INSTS, typePROGAlgo, typeLIST_ID, typeCOND, typeBOUCLE, typeBOOL} typeNoeud;
typedef enum {typeTQ, typePOUR, typeDO_W} type_boucle;
typedef enum {typeINF_EQ, typeSUP_EQ, typeDIFT} type_cmp;
typedef enum {typeET, typeOU, typeNON} type_opl;


typedef struct {
  int val;
} feuilleNb;

typedef struct {
  char id[32];
} feuilleID;


typedef struct{
  int booleen;
}feuilleBOOL;


typedef struct {
  int ope;
  struct asa* noeud[2];
} noeudOp;

typedef struct {
  struct asa* id;
  struct asa* droite;
}noeudAFFECTATION;

typedef struct{
  char func_name[32];
  struct asa* param;
  struct asa* decla_list;
  struct asa* inst;
} noeudFCT;

typedef struct{
  struct asa* decla_list;
  struct asa* main;
} noeudPROGAlgo;

typedef struct{
  struct asa* inst;
  struct asa* suite;
}noeudLIST_INSTS;

typedef struct{
  struct asa* id;
  struct asa* decla_exp;
  struct asa* decla_suiv;
}noeudDECLA_VAR;

typedef struct{
  struct asa* exp;
  struct asa* cond_si;
  struct asa* cond_sinon;
}noeudCOND;

typedef struct{
  type_boucle type;
  struct asa* exp;
  struct asa* list_insts;
}noeudBOUCLE;

typedef struct{
  type_opl type;
  struct asa* exp1;
  struct asa* exp2;
}noeudOpl;

typedef struct{
  type_cmp type;
  struct asa* exp1;
  struct asa* exp2;
}noeudCMP;

typedef struct asa{
  typeNoeud type;
  int memadr;
  int codelen;
  union {
    feuilleNb nb;
    feuilleBOOL bool_value;
    noeudOp op;
    noeudOpl op_l;
    noeudCMP op_cmp;
    feuilleID id;
    noeudDECLA_VAR decla_var;
    noeudAFFECTATION affect;
    noeudFCT fct;
    noeudBOUCLE noeud_boucle;
    noeudCOND cond;
    noeudLIST_INSTS list_insts;
    noeudPROGAlgo prog_algo;
  };
} asa;

//creer_noeudCMP et creer_noeudOPL prototypes non définit
asa * creer_noeudCMP(type_cmp type, asa* p1, asa* p2);
asa * creer_noeudOPL(type_opl type, asa* p1, asa* p2);

asa * creer_noeudCOND(asa* exp, asa* p1, asa* p2);
asa * creer_noeudBOUCLE(type_boucle type, asa* exp, asa* p1);

asa * creer_noeudFCT(char* name, asa* param, asa* decla_list, asa* inst);
asa * creer_noeudPROGAlgo(asa* param, asa* inst);
asa * creer_noeudLIST_INSTS(asa* inst, asa* suite);
asa* creer_noeudAFFEC(asa* id, asa* p1);
asa * creer_noeudDECLA_VAR(asa* id, asa* p1, asa* p2);
asa * creer_feuilleID(char* s);
asa * creer_feuilleNB( int value );
asa*  creer_feuilleBOOL(int bv);
asa * creer_noeudOP( int ope, asa * p1, asa * p2 );
void free_asa(asa *p);

//incomplets pour les autres types
void print_asa(asa * p);
void print_typeOP(asa *p);
void print_typeNB(asa *p);
void print_typeID(asa *p);
void print_typeDECLA_VAR(asa *p);
void print_typeAFFECTATION(asa *p);

void error_asa(const char * s);

#endif
