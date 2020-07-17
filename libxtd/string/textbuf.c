/*
 * TEXTBUF.C --Simple stream-like API for text buffers.
 *
 * Contents:
 * textbuf_setstr()  --Initialise a text buffer from a character string.
 * textbuf_setstrn() --Initialise a text buffer from explicit length string.
 * textbuf_tell()    --Report the current position in a text buffer.
 * textbuf_seek()    --Seek to a specific place in the text buffer.
 * textbuf_getc()    --Get the next character from a textbuffer.
 * textbuf_ungetc()  --Push a character back onto a textbuffer.
 * textbuf_gets()    --Get the next line from a textbuffer.
 *
 * TODO: implement textbuf_tell()
 * TODO: replace this with funopen()!
 */

#include <string.h>

#include <xtd.h>
#include <textbuf.h>

/*
 * textbuf_setstr() --Initialise a text buffer from a character string.
 */
void textbuf_setstr(TextBufferPtr tb, const char *str)
{
    textbuf_setstrn(tb, str, strlen(str));
}

/*
 * textbuf_setstrn() --Initialise a text buffer from explicit length string.
 */
void textbuf_setstrn(TextBufferPtr tb, const char *str, size_t size)
{
    tb->length = size;
    tb->offset = 0;
    tb->buffer = str;
    tb->push_char = 0;
}

/*
 * textbuf_tell() --Report the current position in a text buffer.
 */
off_t textbuf_tell(TextBufferPtr tb)
{
    return tb->offset;
}

/*
 * textbuf_seek() --Seek to a specific place in the text buffer.
 */
void textbuf_seek(TextBufferPtr tb, off_t offset, int whence)
{
    switch (whence)
    {
    case SEEK_SET:
        tb->offset = offset;
        break;
    case SEEK_CUR:
        tb->offset += offset;
        break;
    case SEEK_END:
    {
        off_t new_position = (signed) tb->length + offset;

        if (new_position < 0)
        {
            tb->offset = 0;
        }
        else if (new_position >= (off_t) tb->length)
        {
            tb->offset = (off_t) tb->length;
        }
        else
        {
            tb->offset = new_position;
        }
    }
        break;
    default:
        /* @todo: avoid this via enum-whence */
        break;
    }
}

/*
 * textbuf_getc() --Get the next character from a textbuffer.
 */
int textbuf_getc(TextBufferPtr tb)
{
    if (tb->push_char > 0)
    {
        int c = tb->push_char;

        tb->push_char = 0;
        return c;
    }
    if ((size_t) tb->offset < tb->length)
    {
        return tb->buffer[tb->offset++];
    }
    return EOF;
}

/*
 * textbuf_ungetc() --Push a character back onto a textbuffer.
 */
int textbuf_ungetc(TextBufferPtr tb, int c)
{
    if (tb->push_char == 0)
    {
        return tb->push_char = c;
    }
    return EOF;
}

/*
 * textbuf_gets() --Get the next line from a textbuffer.
 */
char *textbuf_gets(char *buf, int n, TextBufferPtr tb)
{
    char *bp = buf;

    while (n-- > 0)
    {
        int c = textbuf_getc(tb);

        if (c == EOF)
        {
            if (bp == buf)
            {
                return NULL;           /* EOF after no characters */
            }
            break;                     /* EOF, but we read something too */
        }
        if (c == '\n')
        {
            break;                     /* EOL */
        }
        *bp++ = c;
    }
    *bp = '\0';                        /* terminate string */
    return buf;
}
