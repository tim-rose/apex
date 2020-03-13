/*
 * INI-PARSE.C --A simple parser for INI-style config files.
 *
 * Contents:
 * ini_pragma_() --Process any #pragma/#line definitions.
 * ini_skip_()   --Skip newlines and comments.
 * ini_value_()  --cleanup the scanf-parsed config value string.
 * ini_err()     --Report an error using the ini's lexer context.
 * ini_parse()   --Parse an ".INI"-style config file.
 * ini_fopen()   --Open a file for ini parsing.
 * ini_close()   --Close an open Ini context, including its file.
 *
 * Remarks:
 * This file contains routines for parsing the much-used, but
 * little-defined "INI" style syntax, initially used by Microsoft and
 * now widely used by many programs.
 *
 * The basics of an INI file are a bunch of "name = value" lines,
 * separated by "[section]" stanzas.  For more details and a defacto
 * definition see the wikipedia article cited below.
 *
 * Features of this implementation:
 *  * blank lines are allowed
 *  * ";" and "#" are treated as comment characters
 *  * quoted values are supported
 *  * definitions can occur before a section is defined
 *  * #line pragma is supported.
 *
 * See Also:
 * http://en.wikipedia.org/wiki/INI_file
 *
 */
#include <xtd.h>                       /* Windows_NT requires this before system headers */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <ini.h>
#include <vector.h>
#include <log.h>

/*
 * ini_pragma_() --Process any #pragma/#line definitions.
 *
 * Remarks:
 * Actually, I'm only handling "#line" at the moment.
 */
static void ini_pragma_(IniPtr ini, char *line)
{
    char filename[LINE_MAX] = "";
    int number;

    if (sscanf(line, " line %d \"%s\"", &number, filename) >= 1)
    {                                  /* #line-processing */
        ini->line = number;
        if (*filename != '\0')
        {
            free((void *) ini->name);
            ini->name = strdup(filename);
        }
    }
}

/*
 * ini_skip_() --Skip newlines and comments.
 *
 * Remarks:
 * These config files are "line" based, and simple enough that a full
 * tokenizer isn't warranted; it's enough to strip some whitespace in
 * the search for comments (which are also stripped), and let the
 * caller use scanf to handled the rest.
 */
static char *ini_skip_(IniPtr ini, char *line)
{
    char *end;

    while (*line == ' ' || *line == '\t')
    {                                  /* skip whitespace */
        ++line;
    }

    if (*line == '#' || *line == ';')
    {                                  /* skip comment lines '#', ';' */
        *line = '\0';
        ini_pragma_(ini, line + 1);
    }
    else
    {                                  /* remove trailing newline char(s) */
        if ((end = strchr(line, '\r')) != NULL)
        {                              /* hello windows! */
            *end++ = '\0';
        }
        if ((end = strchr(line, '\n')) != NULL)
        {
            *end++ = '\0';
        }
    }
    return line;
}

/*
 * ini_value_() --cleanup the scanf-parsed config value string.
 */
static char *ini_value_(char *value)
{
    char *end = value + strlen(value) - 1;

    while (*end == ' ')
    {                                  /* trim trailing space from value */
        *end-- = '\0';
    }
    if ((*value == '"' && *end == '"') || (*value == '\'' && *end == '\''))
    {                                  /* naive quote stripping */
        value += 1;
        *end = '\0';
    }
    return value;
}

/*
 * ini_err() --Report an error using the ini's lexer context.
 *
 * Parameters:
 * ini    --lexer context
 * fmt, ...  --printf varargs
 */
void ini_err(IniPtr ini, const char *fmt, ...)
{
    va_list argptr;

    va_start(argptr, fmt);

    fprintf(stderr, "%s:%d: ", ini->name, ini->line);
    vfprintf(stderr, fmt, argptr);
    fputs("\n", stderr);
    va_end(argptr);
}

/*
 * ini_parse() --Parse an ".INI"-style config file.
 *
 * Parameters:
 * ini  --the ini file etc. context
 * proc --a parser action proc
 * data --parser action proc's context data.
 *
 * Returns:
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * The caller-provided IniProc will be called for every definition
 * as it is parsed.
 */
int ini_parse(IniPtr ini, IniProc proc, void *data)
{
    char line_buf[LINE_MAX];
    char section_buf[LINE_MAX];
    char *line;
    char *section = NULL;
    int status = 1;

    while ((line = fgets(line_buf, sizeof(line_buf), ini->fp)) != NULL)
    {
        char name_buf[LINE_MAX];
        char value_buf[LINE_MAX];

        ini->line += 1;                /* count every line */

        if ((line = ini_skip_(ini, line)) == NULL || *line == '\0')
        {
            continue;                  /* skip blank, comment lines */
        }

        if (sscanf(line, "[ %[^]] ]", name_buf) == 1)
        {                              /* (re)new section name */
            strcpy(section_buf, name_buf);
            section = section_buf;
            continue;
        }
        if (sscanf(line, " %[^= ] = %[^\n]", name_buf, value_buf) == 2)
        {                              /* TODO: STRING escapes!? */
            if (!proc(ini, section, name_buf, ini_value_(value_buf), data))
            {
                status = 0;
                break;                 /* error: callback failed */
            }
            continue;
        }
        ini_err(ini, "unrecognised line: \"%s\"", line);
    }
    return status;
}

/*
 * ini_fopen() --Open a file for ini parsing.
 *
 * Parameters:
 * path --the name of the file to open
 *
 * Returns: (IniPtr)
 * Success: an Ini lexer context; Failure: NULL.
 *
 * Remarks:
 * We wrap a file with some lexer state so that ini_err() can report
 * errors correctly.  If this routine returns failure, errno will be set.
 */
IniPtr ini_fopen(const char *filename)
{
    FILE *fp;
    IniPtr ini = NULL;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return NULL;                   /* error: no such file */
    }
    if ((ini = NEW(Ini, 1)) != NULL)
    {
        ini->fp = fp;
        ini->name = strdup(filename);
        return ini;                    /* success: return w/ open file */
    }

    fclose(fp);                        /* file cleanup */
    return NULL;                       /* error: malloc failed */
}

/*
 * ini_close() --Close an open Ini context, including its file.
 */
void ini_close(IniPtr ini)
{
    if (ini->fp != NULL)
    {
        fclose(ini->fp);
        ini->fp = NULL;
        free((void *) ini->name);
    }
    free(ini);
}
