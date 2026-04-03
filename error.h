/************************************************************************
  Nom du fichier : error.h
=============================================================
  Description : Gestion centralisée des erreurs du projet
=============================================================
  Auteur : Nachid Ayman
=============================================================
************************************************************************/


#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

/* Codes d'erreurs globaux du projet */
typedef enum
{
    ERR_OK = 0,

    /* erreurs génériques */
    ERR_NULL_POINTER,
    ERR_INVALID_ARGUMENT,

    /* erreurs mémoire */
    ERR_ALLOC,

    /* erreurs de structures */
    ERR_HASH,
    ERR_SEQUENCE,
    ERR_TREE,
    ERR_DYNARRAY,

    /* erreurs entrées/sorties */
    ERR_FILE_OPEN,
    ERR_FILE_READ,
    ERR_FILE_WRITE,

    /* erreurs internes */
    ERR_INTERNAL

} error_code;


/* Convertit un code d'erreur en texte */
const char *error_string(error_code err);

/* Affiche une erreur avec contexte */
void error_print(error_code err, const char *module, const char *msg);

/* Version debug avec fichier et ligne */
void error_debug(error_code err, const char *file, int line, const char *msg);


/* Macro pour debug */
#define ERROR_DEBUG(err,msg) error_debug(err,__FILE__,__LINE__,msg)

#endif