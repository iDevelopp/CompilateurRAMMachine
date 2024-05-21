#include "semantic.h"

char CTXT[32] = "GLOBAL";

void semantic(asa *p){
  if(!p) return;
  switch (p->type){
  case typeNB:
    semantic_NB(p);
    break;
  case typeOP:
    semantic_OP(p);
    break;
  case typeDECLA_VAR:
    semantic_DECLA_VAR(p);
    break;
  default:
    break;
  }
}

void semantic_NB(asa * p)
{
  p->codelen = 1;
  int mem_max = 2<<15; 
  if (!(p->nb.val<(mem_max)-1 && p->nb.val>-(mem_max)-1)){
    printf("warning : taille de NB trop grande ! Débordement mémoire. 16 bits max");
  }
}

void semantic_OP(asa * p)
{
  switch (p->op.ope){
  case '+':  // operateur binaire associatif a gauche (+ - * / % )
  case '-':
  case '*':
  case '/':
  case '%':
    semantic(p->op.noeud[0]);
    semantic(p->op.noeud[1]);
    break;
  }
}

void semantic_DECLA_VAR(asa * p){
  char* id = p->decla_var.id->id.id;
  if(ts_rechercher_identificateur(TABLE_SYMBOLES, CTXT, id)){
    //si oui, warning
    printf("warning id retrouvé dans TABLE_SYMBOLES, CTXT");
  }else{
    //sinon ajouter id avec  contexte globale
    ts_ajouter_identificateur(TABLE_SYMBOLES, id, CTXT, TYPE_ENTIER, 1);
    semantic(p->decla_var.decla_suiv);
  }
}
