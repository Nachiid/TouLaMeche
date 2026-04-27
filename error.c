/************************************************************************
 * File    : error.c
 * =============================================================
 * Description : Implementation of the centralized error-handling
 *               module. Provides human-readable error strings and
 *               formatted diagnostic output to stderr.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#include <stdio.h>
#include "error.h"

/*
 * error_string - Convert an error code to a human-readable string
 * @err: The error code to translate
 *
 * Maps every value of the error_code enumeration to a short,
 * descriptive English label. Unknown codes fall back to a safe
 * default so callers can always print the return value directly.
 *
 * Returns: A pointer to a string literal describing @err.
 *          The pointer is statically allocated and must not be freed.
 */
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

/*
 * error_print - Print a formatted error message to stderr
 * @err:    Error code identifying the failure category
 * @module: Name of the module or subsystem reporting the error
 * @msg:    Free-form description providing additional context
 *
 * Outputs a single line to stderr in the format:
 *   [ERROR] module=<module> | <error string> | <msg>
 *
 * Intended for use at call sites where the source file and line
 * number are not required (e.g. top-level API boundary checks).
 */
void error_print(error_code err, const char *module, const char *msg)
{
    fprintf(stderr,
            "[ERROR] module=%s | %s | %s\n",
            module,
            error_string(err),
            msg);
}

/*
 * error_debug - Print a diagnostic error message with source location
 * @err:  Error code identifying the failure category
 * @file: Source file name where the error was detected (use __FILE__)
 * @line: Line number in @file where the error was detected (use __LINE__)
 * @msg:  Free-form description providing additional context
 *
 * Outputs a single line to stderr in the format:
 *   [ERROR] <file>:<line> | <error string> | <msg>
 *
 * Prefer the ERROR_DEBUG() macro over calling this function directly,
 * as the macro automatically supplies __FILE__ and __LINE__.
 */
void error_debug(error_code err, const char *file, int line, const char *msg)
{
    fprintf(stderr,
            "[ERROR] %s:%d | %s | %s\n",
            file,
            line,
            error_string(err),
            msg);
}