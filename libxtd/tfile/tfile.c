/*
 * TFILE.C --Routines for writing timestamp-named files.
 *
 * Contents:
 * tfopen()         --Open a timestamp-named file.
 * tfwrite()        --Write some bytes to a timestamp file.
 * tfwrite_time()   --Write a strftime template to a timestamp file.
 * tfprintf()       --Write some formatted text to a timestamp file.
 * tfclose()        --Close a timestamp file.
 * new_tfile()      --Allocate the resources for a TFILE structure.
 * free_tfile()     --Free the resources associated with a TFILE structure.
 * write_template() --Write some timestamp-templated text to a file.
 * reopen_tfile_()  --(re)open a timestamp-named file.
 *
 * Remarks:
 * This module extends the API for writing files to allow the name of
 * the file being written to to be determined from a timestamp.  This
 * is mostly useful for log files that have date-related names
 * (e.g. data-YYYY-MM-DD.log").
 *
 * The idea is that you name the file with a strftime(3) template string,
 * and this is used to (re) generate the actual filename.  Each time you write
 * to the file, you specify the timestamp, which may cause the existing file
 * to be closed and re-opened with a new name.
 *
 * When a file is freshly opened the tfopen()-specified prologue is
 * written to the file, and when a file is closed, the epilogue is
 * written.  Both the prologue and epilogue are strftime(3) templates,
 * so they will be expanded with the timestamp provided by the
 * tfwrite().
 *
 * The file is always opened in "append" mode, and is set to be
 * line-buffered.
 *
 */
#include <xtd.h>                       /* Windows_NT requires this before system headers */

#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>

#include <xtd/tfile.h>
#include <xtd/estring.h>
#include <xtd/systools.h>
#include <xtd/date.h>
#include <xtd/log.h>


enum TfileConsts
{
    TEXT_MAX = 4096,                   /* max 4K text per write */
};

static TFILE *new_tfile(const char *name_template,
                        const char *prologue, const char *epilogue);
static void free_tfile(TFILE * tfp);
static size_t write_template(const char *record_template, FILE * fp,
                             time_t t);
static FILE *reopen_tfile_(TFILE * tfp, time_t t);

/*
 * tfopen() --Open a timestamp-named file.
 *
 * Parameters:
 * name_template --the strftime(3) template used to generate the filename
 * t        --the timestamp used to expand the template
 * prologue --a prologue (template!) that is written when opening a file
 * epilogue --a epilogue (template!) that is written when closing a file
 *
 * Return: (TFILE *)
 * Success: the TFILE structure; Failure: NULL.
 */
TFILE *tfopen(const char *name_template, time_t t,
              const char *prologue, const char *epilogue)
{
    TFILE *tfp = NULL;

    if ((tfp = new_tfile(name_template, prologue, epilogue)) != NULL)
    {
        if (!reopen_tfile_(tfp, t))
        {
            free_tfile(tfp);
            return (TFILE *) NULL;
        }
    }
    return tfp;
}

/*
 * tfwrite() --Write some bytes to a timestamp file.
 */
size_t tfwrite(const void *ptr, size_t size, size_t nitems,
               TFILE * tfp, time_t t)
{
    if (reopen_tfile_(tfp, t))
    {
        return fwrite(ptr, size, nitems, tfp->fp);
    }
    return 0;                          /* error: no file open!? */
}

/*
 * tfwrite_time() --Write a strftime template to a timestamp file.
 */
size_t tfwrite_time(const char *record_template, TFILE * tfp, time_t t)
{
    if (reopen_tfile_(tfp, t) && record_template != NULL)
    {
        return write_template(record_template, tfp->fp, t);
    }
    return 0;                          /* error: no file open!? */
}

/*
 * tfprintf() --Write some formatted text to a timestamp file.
 */
int tfprintf(TFILE * tfp, time_t t, const char *fmt, ...)
{
    va_list ap;
    int nchar = 0;

    if (reopen_tfile_(tfp, t))
    {
        va_start(ap, fmt);
        nchar = vfprintf(tfp->fp, fmt, ap);
        va_end(ap);
    }
    return nchar;
}

/*
 * tfclose() --Close a timestamp file.
 *
 * Parameters:
 * tfp  --the timestamp file handle
 * t    --the deemed time of closure, (0 => use current time)
 *
 * Returns: (int)
 * Success: 0; Failure: EOF (i.e. same as fclose(3))
 *
 * Remarks:
 * This routine writes the file epilogue, and frees all resources.
 */
int tfclose(TFILE * tfp, time_t t)
{
    int status;

    info("closing output file \"%s\"", tfp->path);
    (void) write_template(tfp->epilogue, tfp->fp, t);
    status = fclose(tfp->fp);
    free_tfile(tfp);

    return status;
}

/*
 * new_tfile() --Allocate the resources for a TFILE structure.
 */
static TFILE *new_tfile(const char *name_template,
                        const char *prologue, const char *epilogue)
{
    TFILE *tfp;

    if ((tfp = calloc(1, sizeof(TFILE))) != NULL)
    {
        tfp->name_template = empty_str;
        if (name_template != NULL)
        {
            if (!strchr(name_template, '%'))
            {                          /* not a time-dependent name... */
                strncpy((char *) tfp->path, name_template,
                        NEL(tfp->path) - 1);
            }
            else
            {
                tfp->name_template = strdup(name_template);
            }
        }
        tfp->prologue = ((prologue == NULL) ? empty_str : strdup(prologue));
        tfp->epilogue = ((epilogue == NULL) ? empty_str : strdup(epilogue));
    }
    return tfp;
}

/*
 * free_tfile() --Free the resources associated with a TFILE structure.
 */
static void free_tfile(TFILE * tfp)
{
    if (!STREMPTY(tfp->name_template))
    {
        free((void *) tfp->name_template);
    }
    if (!STREMPTY(tfp->prologue))
    {
        free((void *) tfp->prologue);
    }
    if (!STREMPTY(tfp->epilogue))
    {
        free((void *) tfp->epilogue);
    }
    free(tfp);
}

/*
 * write_template() --Write some timestamp-templated text to a file.
 *
 * Parameters:
 * record_template --the template containing strftime(3) '%' escape char.s
 * fp       --the file to write to
 * t        --the timestamp (0 => use current time)
 *
 * Returns: (size_t)
 * Same as fwrite(3).
 *
 * Remarks:
 * This routine naively assumes that it will never write more than
 * TEXT_MAX characters.  If the template is empty, or the file pointer
 * is NULL, nothing is written.
 */
static size_t write_template(const char *record_template, FILE * fp, time_t t)
{
    char text[TEXT_MAX];

    if (STREMPTY(record_template))
    {
        return 0;                      /* OK: nothing to write */
    }
    if (strchr(record_template, '%') == NULL)
    {                                  /* no magic characters: simply write */
        return fwrite(record_template, strlen(record_template), 1, fp);
    }
    fmt_time(text, NEL(text), record_template, t);
    return fwrite(text, strlen(text), 1, fp);
}

/*
 * reopen_tfile_() --(re)open a timestamp-named file.
 *
 * Parameters:
 * tfp  --the timestamp file handle
 * t    --a timestamp that controls the name of the output file.
 *
 * Returns: (FILE *)
 * Success: the open file descriptor; Failure: dies!
 *
 * Remarks:
 * The output file is specified by a strftime() string that can
 * change at any time.  This routine handles closing and re-opening
 * the file as needed.
 */
static FILE *reopen_tfile_(TFILE * tfp, time_t t)
{
    char new_path[FILENAME_MAX + 1];
    char dir[FILENAME_MAX + 1];

    if (t == 0)
    {
        time(&t);                      /* default time: now! */
    }

    if (!STREMPTY(tfp->name_template))
    {                                  /* regenerate path */
        fmt_time(new_path, NEL(new_path), tfp->name_template, t);
        if (strcmp(new_path, tfp->path) != 0)
        {                              /* close output if path changed */
            if (tfp->fp != NULL && tfp->fp != stdout)
            {
                info("closing output file \"%s\"", tfp->path);
                write_template(tfp->epilogue, tfp->fp, t);
                fclose(tfp->fp);
            }
            strncpy((char *) tfp->path, new_path, NEL(tfp->path) - 1);
            tfp->fp = (FILE *) NULL;
        }
    }
    if (tfp->fp == NULL)
    {                                  /* open output, or die in the attempt */
        if (STREMPTY(tfp->path))
        {
            tfp->fp = stdout;
            write_template(tfp->prologue, tfp->fp, t);
        }
        else
        {
            if (make_path(path_dirname(tfp->path, NEL(dir), dir)))
            {
                info("opening file \"%s\"", tfp->path);
                if ((tfp->fp = fopen(tfp->path, "a")) == NULL)
                {
                    log_sys(LOG_ERR, "cannot open file \"%s\"", tfp->path);
                    /* REVISIT: return? */
                }
                setlinebuf(tfp->fp);
                if (ftell(tfp->fp) == 0)
                {                      /* at start of file... */
                    write_template(tfp->prologue, tfp->fp, t);
                }
            }
        }
    }
    return tfp->fp;
}
