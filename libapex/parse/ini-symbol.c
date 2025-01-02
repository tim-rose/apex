/*
 * INI-SYMBOL.C --Routines for manipulating ini-definitions as symbols.
 *
 * Contents:
 * load_()       --Callback proc for loading a INI file into a symbol table.
 * ini_load()    --Load an ini file into a symbol table.
 * ini_sym_get() --Get a value, or some default.
 *
 * Remarks:
 * Information recorded as INI-syntax files can be loaded into
 * a symbol tree, and due to the simple syntax of INI files, that
 * tree will be a two-level struct-of-structs shape (i.e. no lists,
 * and sections aren't recursively defined).
 *
 * Consistent with the shell and perl implementations of INI files,
 * these routines allow for a "default" section, which will provide
 * values if a name is not present in a section.
 */
#include <apex.h>                       /* Windows_NT requires this before system headers */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <apex/ini.h>
#include <apex/vector.h>
#include <apex/log.h>

/*
 * load_() --Callback proc for loading a INI file into a symbol table.
 *
 * Remarks:
 * This proc converts the "ini" file definitions into a two-level
 * symbol hierarchy.  Currently, the values are all interned as
 * strings, but I plan to change this to apply a list of conversions,
 * probably passed as part of the callback data.
 */
static int load_(Ini *ini, const char *section, const char *name,
                 const char *value, void *data)
{
    Symbol **sym_ptr = (Symbol **) data;
    static Atom path[] = { {.type = STRING_TYPE}, {.value.integer = 0} };
    Type node_type;
    Value *node_vptr;
    Value *section_vptr;
    Symbol node = {.type = STRING_TYPE };

    if (*sym_ptr == NULL)
    {                                  /* first time(?): create symbol table */
        if ((*sym_ptr = NEW_VECTOR(Symbol, 1, &null_symbol)) == NULL)
        {
            return 0;                  /* error: malloc failed */
        }
    }
    if (section != NULL)
    {                                  /* find or create the section */
        path[0].value.string = (char *) section;

        switch (node_type = sym_get(*sym_ptr, path, &section_vptr))
        {
        case VOID_TYPE:               /* new section: insert it */
            do
            {
                size_t slot = (size_t) vector_len(*sym_ptr) - 1;

                debug("load_: adding section %s", section);
                if ((node.name = strdup(section)) == NULL)
                {
                    return 0;          /* error: strdup failed */
                }
                node.type = STRUCT_TYPE;
                if ((node.value.field =
                     NEW_VECTOR(Symbol, 1, &null_symbol)) == NULL)
                {
                    free(node.name);
                    return 0;          /* error: malloc failed */
                }
                *sym_ptr = vector_insert(*sym_ptr, slot, 1, &node);
                section_vptr = &(*sym_ptr)[slot].value;
                sym_ptr = &section_vptr->field;
            }
            while (0);
            break;
        case STRUCT_TYPE:
            sym_ptr = &section_vptr->field;
            break;
        default:
            ini_err(ini,
                    "cannot create section \"%s\": it already has a %s value",
                    section, sym_type_name[node_type]);
            return 0;                  /* error: bad section name */
        }
    }
    path[0].value.string = (char *) name;
    switch (node_type = sym_get(*sym_ptr, path, &node_vptr))
    {
    case VOID_TYPE:                   /* new value */
        do
        {
            size_t slot = (size_t) vector_len(*sym_ptr) - 1;

            if ((node.name = strdup(name)) == NULL)
            {
                return 0;              /* error: strdup failed */
            }
            node.type = STRING_TYPE;
            node.value.string = strdup(value);
            if ((*sym_ptr = vector_insert(*sym_ptr, slot, 1, &node)) == NULL)
            {
                return 0;              /* error: malloc failed */
            }
        }
        while (0);
        break;
    case STRING_TYPE:                 /* replace existing */
        free(node_vptr->string);
        node_vptr->string = strdup(value);
        break;
    default:
        ini_err(ini, "cannot overwrite \"%s\": it already has a %s value",
                name, sym_type_name[node_type]);
        return 0;                      /* error: bad value? */
    }
    return 1;
}

/*
 * ini_load() --Load an ini file into a symbol table.
 *
 * Parameters:
 * ini  --the ini parser object
 * symtab   --the symbol table (or NULL)
 *
 * Remarks:
 * The symbol table will be allocated as needed by the ini-proc, however
 * if an initialised symbol table is passed, the ini-proc will replace
 * existing values.
 */
Symbol *ini_load(Ini *ini, Symbol *sym)
{
    if (ini_parse(ini, load_, &sym))
    {
        return sym;
    }
    return NULL;
}

/*
 * ini_sym_get() --Get a value, or some default.
 *
 * Parameters:
 * sym  --the symbol tree loaded by ini_load()
 * section  --the name of the section to search
 * name --the variable name to search
 * default_value    --specifies a default value, or NULL
 * value    --returns the requested value
 *
 * Returns: (Type)
 * Success: the type of the found value; Failure: VOID_TYPE.
 *
 * Remarks:
 * This routine relies on assumptions about being an INI-style
 * symbol tree:
 *  * it's two-level only (i.e. "section.name")
 *  * there may be a "default" section.
 */
Type ini_sym_get(Symbol *sym, const char *section,
                 const char *name, char *default_value, Value *value)
{
    Type type;
    Atom path[3] = {
        {.type = STRING_TYPE},
        {.type = STRING_TYPE},
        {.type = VOID_TYPE}
    };

    path[0].value.string = (char *) section;
    path[1].value.string = (char *) name;

    if ((type = sym_get_value(sym, path, value)) != VOID_TYPE)
    {
        return type;
    }
    path[0].value.string = (char *) "default";  /* REVISIT: hardcoded default? */
    if ((type = sym_get_value(sym, path, value)) != VOID_TYPE)
    {
        return type;
    }
    if (default_value != NULL)
    {
        value->string = default_value;
        return STRING_TYPE;
    }
    return VOID_TYPE;
}

void ini_sym_free(Symbol *sym)
{
    Value value = {.field = sym };

    sym_free_value(STRUCT_TYPE, value);
}
