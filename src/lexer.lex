%{
  #include <string.h>
  #include "parser.h"

  extern void yyerror(char *);

  char errmsg[256]="";
  const char charerr[] = "\x1b[1m\x1b[31m[erreur lexicale]\x1b[0m caractère \x1b[41m%c\x1b[0m inattendu";

  /* MACRO défini
   * Action executee avant chaque action semantique (meme vide)
   * et rempli la variable Bison `yylloc` avec la position du token
   */
#define YY_USER_ACTION                                             \
  yylloc.first_line = yylloc.last_line;                            \
  yylloc.first_column = yylloc.last_column;                        \
  if (yylloc.last_line == yylineno)                                \
    yylloc.last_column += yyleng;                                  \
  else {                                                           \
    yylloc.last_line = yylineno;                                   \
    yylloc.last_column = 1;					   \
  }



%}

%option nounput
%option noinput
%option yylineno

CHIFFRE  [0-9]
IDENT    [a-zA-Z]+
NOMBRE   [0-9]+



%%

{NOMBRE}        {  yylval.nb = atoi(yytext); return NB; }
[-*+/=%<>,\n()]   {return *yytext;}
"PROGRAMME"       {return PROGRAMME;}
"VAR"             {return VAR;}
"DEBUT"           {return DEBUT;}
"FIN"             {return FIN;}
"<-"              {return FLECHE;}
"ALGO"            {return ALGO;}
"<="              {return INF_EQ;}
">="              {return SUP_EQ;}
"!="              {return DIFT;}
"SI"              {return SI;}
"ALORS"           {return ALORS;}
"SINON"           {return SINON;}
"FSI"             {return FSI;}
"TQ"              {return TQ;}
"FAIRE"           {return FAIRE;}
"FTQ"             {return FTQ;}
"VRAI"            {return B_TRUE;}
"FAUX"            {return B_FALSE;}
"LIRE()"          {return LIRE;}
"ECRIRE()"        {return ECRIRE;}
"ET"              {return ET;}
"OU"              {return OU;}
"NON"             {return NON;}
[ \t]           { /* ignorer les blancs */ int car;  while(car==' ' || car=='\t') car=getchar(); }
{IDENT}         { strcpy(yylval.id, yytext); return ID;}


.         {
	    sprintf(errmsg,charerr, yytext[0]);
            yyerror(errmsg);
            return 1;
          }

%%
