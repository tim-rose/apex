/*
 * TEXTBUF.H --Simple fixed-size text buffer handling.
 *
 *
 * Remarks:
 * The textbuffer API supports a basic stream-like IO API for read-only
 * strings, with pushback of a single character only.
 */
#ifndef TEXTBUF_H
#define TEXTBUF_H

#include <stddef.h>
#include <sys/types.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    /*
     * TextBuffer --A stateful, fixed-size text buffer, with a file-like interface.
     *
     * Remarks:
     * The buffer itself is "owned" by the caller, who is responsible
     * for malloc/free of the data.
     */
    typedef struct TextBuffer_t
    {
        size_t length;                 /* total length of buffer */
        off_t offset;                  /* offset of next character */
        int push_char;                 /* pushed-back character, if any */
        const char *buffer;            /* the text being buffered */
    } TextBuffer, *TextBufferPtr;


    void textbuf_setstr(TextBufferPtr tb, const char *str);
    void textbuf_setstrn(TextBufferPtr tb, const char *str, size_t size);
    off_t textbuf_tell(TextBufferPtr tb);
    void textbuf_seek(TextBufferPtr tb, off_t position, int whence);

    int textbuf_getc(TextBufferPtr tb);
    int textbuf_ungetc(TextBufferPtr tb, int c);
    char *textbuf_gets(char *buf, int n, TextBufferPtr tb);

#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* TEXTBUF_H */
