/************************************************************************
  Nom du fichier : entree_sortie.c
=============================================================
  Description : Implémentation de la gestion d'erreurs
=============================================================
  Auteur : Nachid Ayman
=============================================================
************************************************************************/

#include <stdio.h>
#include "error.h"

const char *error_string(error_code err)
{
    switch (err)
    {
    case ERR_OK:
        return "No error";

    case ERR_NULL_POINTER:
        return "Null pointer";

    case ERR_INVALID_ARGUMENT:
        return "Invalid argument";

    case ERR_ALLOC:
        return "Memory allocation failed";

    case ERR_HASH:
        return "Hash table error";

    case ERR_SEQUENCE:
        return "Sequence error";

    case ERR_TREE:
        return "Prediction tree error";

    case ERR_DYNARRAY:
        return "Dynamic array error";

    case ERR_FILE_OPEN:
        return "File open error";

    case ERR_FILE_READ:
        return "File read error";

    case ERR_FILE_WRITE:
        return "File write error";

    case ERR_INTERNAL:
        return "Internal error";

    default:
        return "Unknown error";
    }
}

void error_print(error_code err, const char *module, const char *msg)
{
    fprintf(stderr,
            "[ERROR] module=%s | %s | %s\n",
            module,
            error_string(err),
            msg);
}

void error_debug(error_code err, const char *file, int line, const char *msg)
{
    fprintf(stderr,
            "[ERROR] %s:%d | %s | %s\n",
            file,
            line,
            error_string(err),
            msg);
}