/************************************************************************
 * File    : error.h
 * =============================================================
 * Description : Public interface for the centralized error-handling
 *               module. Defines the project-wide error code enumeration,
 *               reporting functions, and the ERROR_DEBUG convenience macro.
 * =============================================================
 * Author  : Nachid Ayman
 * =============================================================
 ************************************************************************/

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

/*
 * error_code - Project-wide error code enumeration
 *
 * Groups all failure categories that can be produced by the project's
 * modules. ERR_OK (0) is guaranteed to be falsy so that error codes
 * can be tested directly in boolean expressions.
 */
typedef enum
{
    ERR_OK = 0,             /* Operation completed successfully            */

    /* Generic errors */
    ERR_NULL_POINTER,       /* A required pointer argument was NULL        */
    ERR_INVALID_ARGUMENT,   /* An argument failed a precondition check     */

    /* Memory errors */
    ERR_ALLOC,              /* malloc / realloc returned NULL              */

    /* Data-structure errors */
    ERR_HASH,               /* Hash table operation failed                 */
    ERR_SEQUENCE,           /* Circular-buffer sequence operation failed   */
    ERR_TREE,               /* Prediction tree operation failed            */
    ERR_DYNARRAY,           /* Dynamic array operation failed              */

    /* I/O errors */
    ERR_FILE_OPEN,          /* fopen() or equivalent failed                */
    ERR_FILE_READ,          /* Read operation on an open file failed       */
    ERR_FILE_WRITE,         /* Write operation on an open file failed      */

    /* Internal errors */
    ERR_INTERNAL            /* Unexpected internal state (should not occur)*/

} error_code;


/* -------------------------------------------------------------------
 * Reporting functions
 * ------------------------------------------------------------------- */

/*
 * error_string - Convert an error code to a human-readable string
 * @err: The error code to translate
 *
 * Returns: A pointer to a statically allocated string literal.
 *          Must not be freed by the caller.
 */
const char *error_string(error_code err);

/*
 * error_print - Print a formatted error message to stderr
 * @err:    Error code identifying the failure category
 * @module: Name of the subsystem reporting the error
 * @msg:    Additional context describing the failure
 *
 * Output format: [ERROR] module=<module> | <error string> | <msg>
 */
void error_print(error_code err, const char *module, const char *msg);

/*
 * error_debug - Print a diagnostic error message with source location
 * @err:  Error code identifying the failure category
 * @file: Source file where the error was detected (typically __FILE__)
 * @line: Line number where the error was detected (typically __LINE__)
 * @msg:  Additional context describing the failure
 *
 * Output format: [ERROR] <file>:<line> | <error string> | <msg>
 * Prefer the ERROR_DEBUG() macro to avoid passing __FILE__/__LINE__ manually.
 */
void error_debug(error_code err, const char *file, int line, const char *msg);


/* -------------------------------------------------------------------
 * Convenience macro
 * ------------------------------------------------------------------- */

/*
 * ERROR_DEBUG - Shorthand wrapper around error_debug()
 *
 * Automatically captures the current source file and line number so
 * call sites only need to supply the error code and a message string.
 *
 * Usage: ERROR_DEBUG(ERR_NULL_POINTER, "seq was NULL");
 */
#define ERROR_DEBUG(err, msg) error_debug(err, __FILE__, __LINE__, msg)

#endif /* ERROR_H */