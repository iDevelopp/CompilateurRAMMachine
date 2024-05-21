#include "asa.h"

static const char str_type[][16] = {"NB", "OP", "ID", "DECLA_VAR", "AFFECTATION", "FCT", "LIST_INSTS", "PROG_ALGO", "DECLA_FCT", "COND","BOUCLE", "OPL", "CMP"};
static const char line[36] = "-----------------------------------";

static char buffer[32];
static char TABULATION[256] = "";
static int indent = 0;


asa * creer_noeudCOND(asa* exp, asa* p1, asa* p2){
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");
  p->type = typeCOND;
  p->cond.exp = exp; 
  p->cond.cond_si = p1;
  p->cond.cond_sinon = p2;
  return p;
}

asa * creer_noeudBOUCLE(type_boucle type, asa* exp, asa* p1){
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");
  p->type = typeBOUCLE;
  p->noeud_boucle.type = type;
  p->noeud_boucle.exp = exp;
  p->noeud_boucle.list_insts = p1;
  return p;
}

asa * creer_noeudFCT(char* name, asa* param, asa* decla_list, asa* inst){
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typeFCT;
  strcpy(p->fct.func_name, name);
  p->fct.param = param;
  p->fct.decla_list = decla_list;
  p->fct.inst = inst;
  return p;
}

asa * creer_noeudPROGAlgo(asa* param, asa* inst){
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typePROGAlgo;
  p->prog_algo.decla_list = param;
  p->prog_algo.main = inst;
  return p;
}

asa* creer_noeudLIST_INSTS(asa* inst, asa* suite){
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typeLIST_INSTS;
  p->list_insts.inst = inst;
  p->list_insts.suite = suite;
  return p;
}

asa* creer_noeudAFFEC(asa* id, asa* p1){
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typeAFFECTATION;
  p->affect.id = id;
  p->affect.droite = p1;
  return p;
}

asa * creer_noeudDECLA_VAR(asa* id, asa* p1, asa* p2){
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typeDECLA_VAR;
  p->decla_var.id = id;
  p->decla_var.decla_exp = p1;
  p->decla_var.decla_suiv = p2;
  return p;
}


asa * creer_feuilleNB(int val)
{
  asa *p;

  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typeNB;
  p->nb.val = val;
  return p;
}

asa* creer_feuilleID(char* s){
  asa *p;

  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typeID;
  strcpy(p->id.id, s);
  return p;
}


asa* creer_feuilleBOOL(int bv){
  asa *p;

  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typeBOOL;
  p->bool_value.booleen =  bv;
  return p;
}


asa * creer_noeudOP( int ope, asa * p1, asa * p2)
{
  asa * p;

  if ((p = malloc(sizeof(asa))) == NULL)
    error_asa("echec allocation mémoire");

  p->type = typeOP;
  p->op.ope = ope;
  p->op.noeud[0]=p1;
  p->op.noeud[1]=p2;

  return p;
}


void free_asa(asa *p)
{
  if (!p) return;
  switch (p->type) {
  case typePROGAlgo:
    free_asa(p->prog_algo.decla_list);
    free_asa(p->prog_algo.main);
    break;
  case typeLIST_INSTS:
    free_asa(p->list_insts.inst);
    free_asa(p->list_insts.suite);
    break;
  case typeDECLA_VAR:
    free_asa(p->decla_var.id);
    free_asa(p->decla_var.decla_exp);
    free_asa(p->decla_var.decla_suiv);
    break;
  case typeID:
    free_asa(p);
    break;
  case typeOP:
    free_asa(p->op.noeud[0]);
    free_asa(p->op.noeud[1]);
    break;
  case typeAFFECTATION:
    free_asa(p->affect.id);
    free_asa(p->affect.droite);
  case typeNB:
  default: break;
  }
  free(p);
}

static void print_field(char * field_name, char * field_val, char *TABULATION, char *escape_seq)
{
  char buffer[128] = "";
  sprintf(buffer, "%s|%s%*s| ",
	  TABULATION,
	  field_name,
	  MAX_SIZE_FIELD_NAME-(int)strlen(field_name),"");
  printf("%s%s%s" TXT_NULL "%*s\n",
	 buffer,
	 escape_seq,
	 field_val,
	 MAX_SIZE_FIELD_VAL-MAX_SIZE_FIELD_NAME-(int)strlen(field_val),"|");
}

void print_asa(asa * p){
  int i;
  if (!p) return;
  for (i=0; i<indent; i++){ TABULATION[i] = '\t'; }
  TABULATION[i]='\0';

  printf("%s%s\n", TABULATION, line);

  sprintf(buffer, "%p", p);
  print_field("  noeud", buffer, TABULATION,TXT_BLUE TXT_BOLD);

  sprintf(buffer, "%s", str_type[p->type]);
  print_field("  typeNoeud", buffer, TABULATION,"");

  printf("%s%s\n" \
	 "%s|%*s|%*s|\n",TABULATION, line,
	 TABULATION, MAX_SIZE_FIELD_NAME,"",MAX_SIZE_FIELD_VAL-MAX_SIZE_FIELD_NAME,"");

  sprintf(buffer, "%d", p->memadr);
  print_field("adr mem", buffer, TABULATION,"");

  sprintf(buffer, "%d", p->codelen);
  print_field("taille code", buffer, TABULATION,"");

  switch (p->type) {
  case typeNB:
    print_typeNB(p);
    break;
  case typeOP:
    print_typeOP(p);
    break;
  case typeID:
    print_typeID(p);
    break;
  case typeDECLA_VAR:
    print_typeDECLA_VAR(p);
    break;
  case typeAFFECTATION:
    print_typeAFFECTATION(p);
    break;
  default:
    break;
  }
}

void print_typeOP(asa *p)
{
  sprintf(buffer, "'%c'", p->op.ope);
  print_field("operateur", buffer, TABULATION,"");

  sprintf(buffer, "%p", p->op.noeud[0]);
  print_field("ope gauche", buffer, TABULATION,TXT_GREEN);
  sprintf(buffer, "%p", p->op.noeud[1]);
  print_field("ope droite", buffer, TABULATION,TXT_GREEN);

  printf("%s%s\n", TABULATION,line);
  indent++; print_asa(p->op.noeud[0]); indent--;
  indent++; print_asa(p->op.noeud[1]); indent--;

}

void print_typeNB(asa *p)
{
  sprintf(buffer, "%d", p->nb.val);
  print_field("val", buffer, TABULATION,"");
  printf("%s%s\n", TABULATION,line);
}

void print_typeID(asa *p)
{
  sprintf(buffer, "%s", p->id.id);
  print_field("ident", buffer, TABULATION,"");
  printf("%s%s\n", TABULATION,line);
}

void print_typeDECLA_VAR(asa *p)
{

  printf("%s%s\n", TABULATION,line);

  sprintf(buffer, "%p", p->decla_var.id);
  print_field("declavar", buffer, TABULATION, "");
  sprintf(buffer, "%p", p->decla_var.decla_exp);
  print_field("exp", buffer, TABULATION,TXT_GREEN);
  sprintf(buffer, "%p", p->decla_var.decla_suiv);
  print_field("suiv", buffer, TABULATION,TXT_GREEN);


  printf("%s%s\n", TABULATION,line);
  indent++; print_asa(p->decla_var.id); indent--;
  indent++; print_asa(p->decla_var.decla_exp); indent--;
  indent++; print_asa(p->decla_var.decla_suiv); indent--;

}

void print_typeAFFECTATION(asa *p)
{

  printf("%s%s\n", TABULATION,line);

  sprintf(buffer, "%p", p->affect.id);
  print_field("id affect", buffer, TABULATION,TXT_GREEN);
  sprintf(buffer, "%p", p->affect.droite);
  print_field("val", buffer, TABULATION,TXT_GREEN);


  printf("%s%s\n", TABULATION,line);
  indent++; print_asa(p->affect.id); indent--;
  indent++; print_asa(p->affect.droite); indent--;

}


void error_asa(const char * s){
  fprintf(stderr, TXT_BOLD TXT_RED "[erreur ASA]" TXT_NULL " %s", s);
  exit(1);
}
