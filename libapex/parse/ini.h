/*
 * INI.H --Definitions for an INI parser.
 *
 *
 * Remarks:
 * This module implements a simple call-back style parser for
 * ye olde ".ini" format files.
 */
#ifndef APEX_INI_H
#define APEX_INI_H

#include <stddef.h>
#include <stdio.h>
#include <apex/ini.h>
#include <apex/symbol.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    typedef struct Ini_t
    {
        char *name;                    /* file being parsed */
        int line;                      /* current line number in file */
        FILE *fp;
    } Ini, *IniPtr;

    /*
     * IniProc() --Ini Parser action routine.
     *
     * Parameters:
     * ini      --the ini parser context
     * section  --the current section, or NULL
     * name     --the name of the "variable" just parsed
     * value    --the value just parsed
     * data     --caller context  data
     *
     * Returns: (int)
     * keep-parsing: 1; abort-parsing: 0.
     *
     * Remarks:
     * When using ini_parse(), the caller provides an IniProc parser
     * action routine that is called with the definition just parsed.  The
     * parse aborts if the IniProc returns 0.  If definitions are parsed
     * before a section stanza has been seen, the supplied section
     * argument will be NULL.
     */
    typedef int (*IniProc)(IniPtr ini, const char *section, const char *name,
                           const char *value, void *data);

    void ini_err(IniPtr c, const char *fmt, ...);
    int ini_parse(IniPtr ini, IniProc proc, void *data);
    IniPtr ini_fopen(const char *filename);
    void ini_close(IniPtr ini);

    SymbolPtr ini_load(IniPtr ini, SymbolPtr symtab);
    Type ini_sym_get(SymbolPtr sym, const char *section,
                     const char *name, char *default_value, ValuePtr value);
    void ini_sym_free(SymbolPtr sym);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_INI_H */
