/*
 * URL.C --URL manipulation routines.
 *
 * Contents:
 * service[]         --A list of well-known services and their associated ports.
 * encode_tab[]      --A list of characters that must be encoded in URLs
 * parse_url_path_() --Parse the "path" part of a URL.
 * adjust_port_()    --Adjust the port based on well-known schemes.
 * str_url()         --(Destructively) parse a URL string into a URL structure.
 * sprint_url()      --Format a URL into text buffer.
 * url_encode()      --Escape a string with the URL encoding scheme.
 *
 * Remarks:
 * I'm sure this has been done better, elswhere by others, but here's
 * my take on parsing a URL (look ma, no regexes!).
 *
 */
#include <stdio.h>
#include <symbol.h>
#include <strparse.h>
#include <estring.h>
#include <url.h>

URL null_url = NULL_URL;

/*
 * service[] --A list of well-known services and their associated ports.
 *
 * This is used by adjust_port_() to fill in the default port.
 */
static Enum service[] = {              /* REVISIT: use getservbyname(3)? */
    {(char *) "mailto", 25},           /* hmmm. mailto is "unofficial" */
    {(char *) "ftp", 21},
    {(char *) "http", 80},
    {(char *) "https", 443},
    {NULL, 0}
};

/*
 * encode_tab[] --A list of characters that must be encoded in URLs
 *
 * This is used by url_encode().
 */
static char encode_tab[127] = {
    /* ASCII control characters */
    [0] = 1,[1] = 1,[2] = 1,[3] = 1,[4] = 1,
    [5] = 1,[6] = 1,[7] = 1,[8] = 1,[9] = 1,
    [10] = 1,[11] = 1,[12] = 1,[13] = 1,[14] = 1,
    [15] = 1,[16] = 1,[17] = 1,[18] = 1,[19] = 1,
    [20] = 1,[21] = 1,[22] = 1,[23] = 1,[24] = 1,
    [25] = 1,[26] = 1,[27] = 1,[28] = 1,[29] = 1,
    [30] = 1,[31] = 1,
    /* Reserved characters */
    ['$'] = 1,['&'] = 1,['+'] = 1,[','] = 1,['/'] = 1,
    [':'] = 1,[';'] = 1,['='] = 1,['?'] = 1,['@'] = 1,

    /* unsafe characters  */
    [' '] = 1,['<'] = 1,['>'] = 1,['#'] = 1,['%'] = 1,
    ['{'] = 1,['}'] = 1,['|'] = 1,['\\'] = 1,['^'] = 1,
    ['~'] = 1,['['] = 1,[']'] = 1,['`'] = 1
};

/*
 * parse_url_path_() --Parse the "path" part of a URL.
 */
static inline void parse_url_path_(char *opt, URLPtr url)
{
    url->path = opt;
    if ((url->query = strchr(url->path, '?')) != NULL)
    {
        *url->query++ = '\0';
        if ((url->anchor = strchr(url->query, '#')) != NULL)
        {
            *url->anchor++ = '\0';
        }
    }
}

/*
 * adjust_port_() --Adjust the port based on well-known schemes.
 */
static inline void adjust_port_(URLPtr url)
{
    EnumPtr s = service;

    if (url->port != 0 || url->scheme == NULL)
    {
        return;
    }

    for (s = service; s->name != NULL; ++s)
    {
        if (strcmp(url->scheme, s->name) == 0)
        {
            url->port = s->value;
            break;
        }
    }
}

/*
 * str_url() --(Destructively) parse a URL string into a URL structure.
 *
 * Parameters:
 * opt  --the option string containing a (hopefully!) parseable value
 * url --specifies and returns the URL structure
 *
 * Return: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * This routine differs from the other getopt_xxx() routines in two ways:
 * it modifies the passed string, and it (potentially) only partially
 * modifies the URL structure.
 *
 * Basically, it assumes that opt is allocated by the caller, and it
 * initialises url with pointers into the opt string.  Because many of
 * the syntactic elements of a URL optional, it only sets the elements it
 * sees, so that the caller can pre-initialise the URL structure with
 * default values.
 *
 * e.g., the following should work:
 *
 *     URL email = { .scheme = "mailto" };
 *     char url[] = "someuser@host";
 *     getopt_url(url, &email);
 *
 * Note: the leaading "/" in the path component is removed, because
 * it is typically overwritten with '\0' to end the domain string.
 * This may not suit applications that wish to distinguish
 * between absolute and relative URLs.
 */
int str_url(char *opt, URLPtr url)
{
    char *match = strstr(opt, "://");

    if (match != NULL)
    {
        url->scheme = opt;
        *match = '\0';
        opt = match + 3;
    }
    else if (strstr(opt, "mailto:") == opt)
    {                                  /* "mailto:" lacks "//" */
        url->scheme = opt;
        opt += 6;
        *opt++ = '\0';
    }
    if ((match = strchr(opt, '/')) != NULL)
    {
        *match++ = '\0';
        parse_url_path_(match, url);
    }

    if ((match = strchr(opt, '@')) != NULL)
    {
        *match++ = '\0';
        url->user = opt;
        url->domain = match;
        if ((match = strchr(url->user, ':')) != NULL)
        {
            *match++ = '\0';
            url->password = match;
        }
    }
    else
    {
        url->domain = opt;
    }
    if ((match = strchr(url->domain, ':')) != NULL)
    {
        *match++ = '\0';
        if (!str_int(match, &url->port))
        {
            return 0;
        }
    }
    adjust_port_(url);
    return 1;
}

/*
 * sprint_url() --Format a URL into text buffer.
 *
 * Parameters:
 * str  --returns the formatted string
 * n    --specifies the available space in the string
 * url --specifies and returns the URL structure
 *
 * Return: (int)
 * The number of characters printed, or required.
 */
int snprint_url(char *str, size_t n, URLPtr url)
{
    char *start = str;

    size_t len =
        (url->scheme == NULL ? 0 : strlen(url->scheme) + 3)
        + (url->user == NULL ? 0 : strlen(url->user) + 1)
        + (url->password == NULL ? 0 : strlen(url->password) + 1)
        + (url->domain == NULL ? 0 : strlen(url->domain) + 1)
        + (url->port == 0 ? 0 : (size_t) snprintf(NULL, 0, ":%d", url->port))
        + (url->path == NULL ? 0 : strlen(url->path) + 1)
        + (url->query == NULL ? 0 : strlen(url->query) + 1)
        + (url->anchor == NULL ? 0 : strlen(url->anchor) + 1);

    if (len > n)
    {
        return len;
    }

    *str = '\0';
    str = vstrcat(str,
                  (url->scheme == NULL ? "" : url->scheme),
                  (url->scheme == NULL
                   ? ""
                   : (strcmp(url->scheme, "mailto") == 0
                      ? ":"
                      : "://")),
                  (url->user == NULL ? "" : url->user),
                  (url->password == NULL ? "" : ":"),
                  (url->password == NULL ? "" : url->password),
                  (url->user == NULL ? "" : "@"),
                  (url->domain == NULL ? "" : url->domain), NULL);

    if (url->port != 0)
    {
        str += sprintf(str, ":%d", url->port);
    }
    str = vstrcat(str,
                  (url->path == NULL ? "" : "/"),
                  (url->path == NULL ? "" : url->path),
                  (url->query == NULL ? "" : "?"),
                  (url->query == NULL ? "" : url->query),
                  (url->anchor == NULL ? "" : "#"),
                  (url->anchor == NULL ? "" : url->anchor), NULL);
    return str - start;
}

/*
 * url_encode() --Escape a string with the URL encoding scheme.
 *
 * Parameters:
 * text --the text of the string to be URL-encoded
 * n    --the size of a buffer to store the encoded result
 * buf  --returns the encoded string.
 *
 * Returns: (size_t)
 * The number of characters used/required in the buffer.
 *
 * Remarks:
 * This routine will return the number of characters required to
 * encode, and will encode as much as it can.  It is up to the caller
 * to check that the return value is less than n.
 *
 * Note that not all parts of a URL need to be encoded; it is up to the
 * caller to decide which parts require it (e.g. path, query, anchor)
 */
size_t url_encode(const char *text, size_t n, char *buf)
{
    int n_used = 0;
    int c;

    while ((c = *text++) != '\0')
    {
        int encode = c >= 127 || encode_tab[c];
        size_t n_needed = 1 + 2 * (size_t) encode;

        if (n - (size_t) n_used > n_needed)
        {
            if (encode)
            {
                buf += sprintf(buf, "%%%x", c);
            }
            else
            {
                *buf++ = c;
            }
        }
        n_used += n_needed;
    }
    *buf = '\0';
    return (size_t) n_used;
}
