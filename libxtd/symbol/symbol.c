/*
 * SYMBOL.C --Symbol structure query routines.
 *
 * Contents:
 * sym_free_value()  --Recursively release symbol resources.
 * sym_add_node_()   --Add a node to a path in progress.
 * new_sym_path()    --Convert a path spec. into a list of symbol components.
 * free_sym_path()   --Free all resources associated with a compiled sympath.
 * sym_path_equal()  --Compare two paths for equality.
 * sym_path_match()  --Compare two paths of known length.
 * sprint_sym_path() --Format a path to a string.
 * fprint_sym_path() --Print a path to file.
 * print_sym_path()  --Print a path to stdout.
 * enum_cmp()        --Compare two Enums for sorting.
 * list_get_()       --Find a list item specified by a (compiled) symbol path.
 * sym_get()         --Find the a symbol's value by a (compiled) symbol path.
 * sym_get_value()   --Find the value of a symbol by a (compiled) symbol path.
 * sym_get_int()     --Return a symbol's value coerced to an int.
 * sym_get_real()    --Return a symbol's value coerced to a real.
 * sym_get_str()     --Return a symbol's value coerced to a string.
 * str_enum()        --Set an enum opts value.
 *
 * Remarks:
 * TBD
 *
 */
#include <xtd.h>                       /* Windows_NT requires this before system headers */

#include <stdlib.h>
#include <errno.h>

#include <estring.h>
#include <strparse.h>
#include <vector.h>
#include <log.h>
#include <symbol.h>

Atom null_atom = NULL_ATOM;
Enum null_enum = NULL_ENUM;
Symbol null_symbol = NULL_SYMBOL;

const char *sym_type_name[] = {
    "void", "real", "integer", "string", "list", "struct"
};

static const char *sym_match_any_name = "*";

/*
 * sym_free_value() --Recursively release symbol resources.
 */
void sym_free_value(Type type, Value value)
{
    switch (type)
    {
    default:
        break;
    case STRING:
        free(value.string);
        break;
    case STRUCT:
        for (SymbolPtr symbol = value.field; symbol->type != VOID; ++symbol)
        {
            free(symbol->name);
            sym_free_value(symbol->type, symbol->value);
        }
        free_vector(value.field);
        break;
    case LIST:
        for (AtomPtr atom = value.list; atom->type != VOID; ++atom)
        {
            sym_free_value(atom->type, atom->value);
        }
        free_vector(value.list);
        break;
    }
}

/*
 * sym_add_node_() --Add a node to a path in progress.
 *
 * Parameters:
 * sym      --specifies the symbol path to append to
 * node --specifies the new node with initialised type
 * name --specifies the element name to append
 * path --specifies the full path (in progress of) being parsed
 *
 * Returns: (AtomPtr)
 * Success: the updated symbol path; Failure: NULL.
 *
 * Remarks:
 * This routine frees the sym variable on error.
 */
static AtomPtr sym_add_node_(AtomPtr sym_path, AtomPtr node,
                             char *name, const char *path)
{
    int idx;
    int error = 0;

    if (sym_path == NULL)
    {
        return NULL;                   /* error: quietly do nothing */
    }
    switch (node->type)
    {
    default:
        err("%s: unrecognised path element \"%s\"", path, name);
        error = 1;
        break;
    case STRING:
        if (strcmp(name, sym_match_any_name) == 0)
        {                              /* wild-card match */
            node->value.string = (char *) sym_match_any_name;
        }
        else if (STREMPTY(name))
        {
            return sym_path;
        }
        else
        {
            node->value.string = strdup(name);
        }
        break;
    case INTEGER:
        if (strcmp(name, sym_match_any_name) == 0)
        {                              /* wild-card match */
            node->value.integer = -1;
        }
        else if (!str_int(name, &idx))
        {
            err("%s: unrecognised array index \"%s\"", path, name);
            error = 1;
        }
        else
        {
            node->value.integer = idx;
        }
        break;
    }
    if (error)
    {
        free_sym_path(sym_path);
        return NULL;
    }
    return vector_add(sym_path, 1, node);
}

/*
 * new_sym_path() --Convert a path spec. into a list of symbol components.
 *
 * Parameter:
 * pathname --the path to compile
 *
 * Returns: (SymbolPtr)
 * Success: the compiled symbol path; Failure: NULL.
 *
 * Remarks:
 * The path specification is designed to be similar to C's struct/array
 * selector syntax: ".field" for structs, and [<integer>] for arrays.
 * e.g.
 *  * foo.bar
 *  * foo[0].bar
 *  * foo[0].bar[15]
 *  * [1][15].foo.bar
 *  * etc.
 */
AtomPtr new_sym_path(const char *path)
{
    const char *str = path;
    char name_buf[NAME_MAX + 1] = { 0 }, *name = name_buf;
    AtomPtr sym;
    Atom node = {.type = STRING, {.integer = 0} };
    Atom sentinel = {.type = VOID, {.integer = 0} };
    int c;

    if (path == NULL)
    {
        return NULL;
    }

    sym = new_vector(sizeof(*sym), 0, NULL);

    while ((c = *str++) != '\0')
    {
        switch (c)
        {
        case '.':
            if (vector_len(sym) == 0 && name == name_buf)
            {
                err("%s: unexpected \".\"", path);
                free_sym_path(sym);
                return NULL;
            }
            *name++ = '\0';
            sym = sym_add_node_(sym, &node, name_buf, path);
            *(name = name_buf) = '\0';
            break;
        case '[':
            *name++ = '\0';
            sym = sym_add_node_(sym, &node, name_buf, path);
            node.type = INTEGER;
            *(name = name_buf) = '\0';
            break;
        case ']':
            *name++ = '\0';
            node.type = INTEGER;
            sym = sym_add_node_(sym, &node, name_buf, path);
            *(name = name_buf) = '\0';
            node.type = STRING;
            break;
        default:
            *name++ = c;
            break;
        }
    }
    *name++ = '\0';
    sym = sym_add_node_(sym, &node, name_buf, path);
    sym = vector_add(sym, 1, &sentinel);
    return sym;
}

/*
 * free_sym_path() --Free all resources associated with a compiled sympath.
 *
 * Parameter:
 * path --the sympath to free
 *
 * Remarks:
 * The string components of the path are all strdup'd, and must be freed
 * individually.
 */
void free_sym_path(AtomPtr path)
{
    size_t n = (size_t) vector_len(path);

    if (path != NULL)
    {
        for (size_t i = 0; i < n; ++i)
        {
            if (path[i].type == STRING
                && path[i].value.string != sym_match_any_name)
            {
                free(path[i].value.string);
            }
        }
        free_vector(path);
    }
}

/*
 * sym_path_equal() --Compare two paths for equality.
 *
 * Parameters:
 * ref_path, test_path   --the two paths to compare.
 *
 * Returns: (int)
 * Equal: 1; Error, or not equal: 0.
 * REVISIT: change this to return an ordered compare result!
 */
int sym_path_equal(AtomPtr ref_path, AtomPtr test_path)
{
    while (1)
    {
        if (ref_path->type != test_path->type)
        {
            return 0;
        }
        switch (ref_path->type)
        {
        case VOID:
            return 1;                  /* all previous items matched... */
        case INTEGER:
            if (ref_path->value.integer != test_path->value.integer)
            {
                return 0;              /* indices don't match */
            }
            break;
        case STRING:
            if (strcmp(ref_path->value.string, test_path->value.string) != 0)
            {
                return 0;              /* path elements don't match */
            }
            break;
        default:
            return 0;                  /* actually, an error! */
        }
        ref_path++;
        test_path++;
    }
}

/*
 * sym_path_match() --Compare two paths of known length.
 *
 * Remarks:
 * Because the lengths are known in advance, this routine can apply a
 * number of optimisations:
 *  * trivially reject paths of different lengths
 *  * compare the elements in the "reverse" direction.
 *
 * It also handles a wildcard match.
 * REVISIT: change this to return an ordered compare result?
 */
int sym_path_match(AtomPtr ref_path, size_t ref_path_len,
                   AtomPtr test_path, size_t test_path_len)
{
    AtomPtr r, t;
    int n = test_path_len - ref_path_len;

    if (n != 0)
    {
        return 0;                      /* trivial: lengths are different */
    }

    r = ref_path + ref_path_len - 1;
    t = test_path + test_path_len - 1;

    for (; r >= ref_path; --r, --t)
    {
        if (t->type != r->type)
        {
            return 0;
        }
        switch (r->type)
        {
        case INTEGER:
            if (r->value.integer == -1)
            {                          /* always match! */
                break;
            }
            if (r->value.integer != t->value.integer)
            {
                return 0;              /* indices don't match */
            }
            break;
        case STRING:
            if (r->value.string == sym_match_any_name)
            {                          /* wildcard match */
                break;
            }
            if (strcmp(r->value.string, t->value.string) != 0)
            {
                return 0;              /* path elements don't match */
            }
            break;
        default:
            return 0;                  /* actually, an error! */
        }
    }
    return 1;
}

/*
 * sprint_sym_path() --Format a path to a string.
 */
int sprint_sym_path(char *str, AtomPtr path)
{
    char *start = str;

    for (size_t i = 0; path[i].type != VOID; ++i)
    {
        if (path[i].type == INTEGER)
        {
            str += sprintf(str, "[" SYMBOL_INT_FORMAT "]",
                           path[i].value.integer);
        }
        else
        {
            str += sprintf(str, "%s%s",
                           i == 0 ? "" : ".", path[i].value.string);
        }
    }
    return str - start;
}

/*
 * fprint_sym_path() --Print a path to file.
 */
int fprint_sym_path(FILE * fp, AtomPtr path)
{
    int n = 0;

    for (size_t i = 0; path[i].type != VOID; ++i)
    {
        if (path[i].type == INTEGER)
        {
            n += fprintf(fp, "[" SYMBOL_INT_FORMAT "]",
                         path[i].value.integer);
        }
        else
        {
            n += fprintf(fp, "%s%s", i == 0 ? "" : ".", path[i].value.string);
        }
    }
    return n;
}

/*
 * print_sym_path() --Print a path to stdout.
 */
int print_sym_path(AtomPtr path)
{
    return fprint_sym_path(stdout, path);
}

/*
 * enum_cmp() --Compare two Enums for sorting.
 */
int enum_cmp(const Enum * a, const Enum * b)
{
    return strcmp(a->name, b->name);
}


/*
 * list_get_() --Find a list item specified by a (compiled) symbol path.
 *
 * Parameters:
 * list  --the dom to be searched
 * path --the path of the symbol to find
 * value    --returns the symbol's value
 *
 * Returns: (Type)
 * Success: the type of symbol; Failure: VOID.
 */
static Type list_get_(AtomPtr list, AtomPtr path, ValuePtr * vptr)
{
    if (path->type == INTEGER)
    {
        AtomPtr element = &list[path->value.integer];

        if (path[1].type == VOID)
        {                              /* plugh! return the slot's value */
            *vptr = &element->value;
            return element->type;
        }
        switch (element->type)
        {
        case STRUCT:
            return sym_get(element->value.field, path + 1, vptr);
        case LIST:
            return list_get_(element->value.list, path + 1, vptr);
        default:
            return VOID;               /* error: at a leaf! */
        }
    }
    return VOID;                       /* error: path isn't an index */
}

/*
 * sym_get() --Find the a symbol's value by a (compiled) symbol path.
 *
 * Parameters:
 * symtab  --the dom to be searched
 * path --the path of the symbol to find
 * value    --returns a pointer to the value in the symbol table
 *
 * Returns: (Type)
 * Success: the type of symbol; Failure: VOID.
 *
 * Remarks:
 * Note that this routine returns a pointer to the symbol table's
 * actual value item.
 */
Type sym_get(SymbolPtr symtab, AtomPtr path, ValuePtr * vptr)
{
    if (path->type != STRING)
    {
        return VOID;                   /* error: path isn't a name */
    }
    for (size_t i = 0; symtab[i].type != VOID; ++i)
    {
        SymbolPtr sym = &symtab[i];

        if (strcmp(sym->name, path->value.string) == 0)
        {
            if (path[1].type == VOID)
            {                          /* plugh! you're at end of road again */
                *vptr = &sym->value;
                return sym->type;
            }
            switch (sym->type)
            {
            case STRUCT:
                return sym_get(sym->value.field, path + 1, vptr);
            case LIST:
                return list_get_(sym->value.list, path + 1, vptr);
            default:
                return VOID;           /* error: at a leaf */
            }
        }
    }
    return VOID;                       /* UNREACHED */
}

/*
 * sym_get_value() --Find the value of a symbol by a (compiled) symbol path.
 *
 * Parameters:
 * symtab  --the dom to be searched
 * path --the path of the symbol to find
 * value    --returns the symbol's value
 *
 * Returns: (Type)
 * Success: the type of symbol; Failure: VOID.
 */
Type sym_get_value(SymbolPtr symtab, AtomPtr path, ValuePtr value)
{
    ValuePtr vptr;
    Type type = sym_get(symtab, path, &vptr);

    if (type != VOID)
    {
        *value = *vptr;
    }
    return type;
}

/*
 * sym_get_int() --Return a symbol's value coerced to an int.
 *
 * Parameters:
 * symtab  --the dom to be searched
 * path --the path of the symbol to find
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int sym_get_int(SymbolPtr symtab, AtomPtr path, SYMBOL_INT * value)
{
    Value v;

    switch (sym_get_value(symtab, path, &v))
    {
    default:
        break;
    case REAL:
        *value = (int) v.real;
        return 1;
    case INTEGER:
        *value = (int) v.integer;
        return 1;
    case STRING:
    {
        char *end;
        SYMBOL_INT i = SYMBOL_STRTOINT(v.string, &end, 0);

        if (STREMPTY(end))
        {
            *value = i;
            return 1;
        }
        return 0;
    }
    }
    return 0;
}

/*
 * sym_get_real() --Return a symbol's value coerced to a real.
 *
 * Parameters:
 * symtab  --the dom to be searched
 * path --the path of the symbol to find
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int sym_get_real(SymbolPtr symtab, AtomPtr path, double *value)
{
    Value v;

    switch (sym_get_value(symtab, path, &v))
    {
    default:
        break;
    case REAL:
        *value = v.real;
        return 1;
    case INTEGER:
        *value = (double) v.integer;
        return 1;
    case STRING:
    {
        char *end;
        double r = strtod(v.string, &end);

        if (STREMPTY(end))
        {
            *value = r;
            return 1;
        }
        return 0;
    }
    }

    return 0;
}

/*
 * sym_get_str() --Return a symbol's value coerced to a string.
 *
 * Parameters:
 * symtab  --the dom to be searched
 * path --the path of the symbol to find
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int sym_get_str(SymbolPtr symtab, AtomPtr path, char **value)
{
    Value v;

    if (sym_get_value(symtab, path, &v) == STRING)
    {
        *value = v.string;
        return 1;
    }
    return 0;
}

/*
 * str_enum() --Set an enum opts value.
 *
 * Parameters:
 * opt  --the string option to be parsed
 * n_item, item     --the enum items to match against "opt"
 * valp    --returns the selected item's value
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * Although the n_items parameter must be specified, note that this
 * code will bail at the first "empty" item slot anyway, so it's OK to
 * supply a "sufficiently large" value, as long as there's an empty
 * slot at the end.
 */
int str_enum(const char *opt, size_t n_items, Enum item[], int *valp)
{
    for (size_t i = 0; i < n_items; ++i)
    {
        if (item[i].name == NULL)
        {
            break;                     /* whoops: end of enum list */
        }
        if (strcasecmp(opt, item[i].name) == 0)
        {
            *valp = item[i].value;
            return 1;
        }
    }
    return 0;
}
