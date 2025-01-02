/*
 * SYMBOL.H --Simple Run-Time Type Information for C data structures.
 *
 * Remarks:
 * The key data structure of this module is a "Symbol" which describes
 * a triple: {name, type, value}.  The supported types correspond
 * to a simplification of those implemented by C:
 *  * integer (one size only)
 *  * float (double only)
 *  * string
 *  * list
 *  * struct
 *
 * Lists and structs are defined recursively in terms of these types, so
 * a Symbol may be viewed as a tree describing arbitrarily complex
 * data structures.
 *
 */
#ifndef APEX_SYMBOL_H
#define APEX_SYMBOL_H
#include <limits.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    /*
     * One of the common differences between systems is the "natural"
     * size of an integer; This ADT chooses to support one size only,
     * and the following macros define the details of that choice.
     */
#define SYMBOL_INT	long int
#define SYMBOL_STRTOINT strtol
#define SYMBOL_INT_FORMAT "%ld"
#define SYMBOL_INT_MAX LONG_MAX
#define SYMBOL_INT_MIN LONG_MIN

    /*
     * These various macros are used to define the values of the
     * corresponding global variables (i.e. null_atom, null_enum,
     * null_symbol, defined below).  However, they are also (more!)
     * useful as terminators for lists of atoms, symbols etc.
     */
#define NULL_ATOM {.type = VOID_TYPE}
#define NULL_ENUM {.name = NULL}
#define NULL_SYMBOL {.type = VOID_TYPE}

    /*
     * Type --The various types understood by the SYMBOL ADT.
     */
    typedef enum Type
    {
        VOID_TYPE = 0,
        REAL_TYPE = 1,
        INTEGER_TYPE = 2,
        STRING_TYPE = 3,
        LIST_TYPE = 4,
        STRUCT_TYPE = 5
    } Type;

    /*
     * Value --A union of the allowed values for symbols.
     */
    typedef union Value
    {
        double real;
        SYMBOL_INT integer;
        char *string;
        struct Atom *list;           /* vector of values */
        struct Symbol *field;        /* vector of fields */
    } Value;

    /*
     * Atom --A value with type information (for representing vectors).
     *
     * Remarks:
     * In style (but not implementation) Atom resembles a Lisp atom.
     * (FWIW: Lisp(s) typically hold the type information in unused
     * bits of the value.)
     */
    typedef struct Atom
    {
        Type type;
        Value value;
    } Atom;

    /*
     * Enum --Named integer values.
     *
     * Remarks:
     * This kind of "run-time enum" is used to store various keyword
     * mappings.  Note that it uses a simple integer (not SYMBOL_INT)
     * so that it can be re-used for some externally defined keywords
     * in syslog (i.e. facilitynames, prioritynames)
     */
    typedef struct Enum
    {
        const char *name;
        int value;
    } Enum;

    /*
     * Symbol --A complete symbol: name, type, value.
     */
    typedef struct Symbol
    {
        char *name;
        Type type;
        Value value;
    } Symbol;

    extern Atom null_atom;
    extern Enum null_enum;
    extern Symbol null_symbol;

    extern const char *sym_type_name[];

    void sym_free_value(Type type, Value value);

    Atom *new_sym_path(const char *path);
    void free_sym_path(Atom *path);
    int sym_path_equal(Atom *s1, Atom *s2);
    int sym_path_match(Atom *ref_path, size_t ref_path_len,
                       Atom *test_path, size_t test_path_len);

    int sprint_sym_path(char *str, Atom *path);
    int fprint_sym_path(FILE * fp, Atom *path);
    int print_sym_path(Atom *path);

    Type sym_get(Symbol *symtab, Atom *path, Value ** value_ptr);
    Type sym_get_value(Symbol *symtab, Atom *path, Value *value);
    int sym_get_int(Symbol *symtab, Atom *path, SYMBOL_INT * value);
    int sym_get_real(Symbol *symtab, Atom *path, double *value);
    int sym_get_str(Symbol *symtab, Atom *path, char **value);
    int sym_get_enum(Symbol *symtab, size_t n_enum, Enum *enums, Atom *path, int **value);    /* not implemented yet! */

    int enum_cmp(const Enum * a, const Enum * b);
    int str_enum(const char *name, size_t n_items, const Enum item[], int *valp);
    int enum_value(const char *name, const Enum *item);
    const char *enum_name(int value, const Enum *item);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_SYMBOL_H */
