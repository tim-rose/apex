/*
 * HTTP.C --A simple API to the HTTP protocol.
 *
 * Contents:
 * http_connect()       --Connect to a host specified by a URL.
 * http_send_request()  --Send a request line to an open HTTP session.
 * http_send_header()   --Send a list of HTTP headers to an open HTTP session.
 * http_request()       --Perform an HTTP request on a url, and get the response.
 * http_free_response() --Free the memory resources of an HTTP response.
 *
 * Remarks:
 * At some point I'll start using the curl library, but this is
 * enough to get me going for now...
 *
 */

#include <xtd.h>                       /* Windows_NT requires this before system headers */

#include <stdio.h>

#include <http.h>
#include <estring.h>
#include <protocol.h>
#include <vector.h>
#include <log.h>


static const char *rfc_eol = "\r\n";

/*
 * http_connect() --Connect to a host specified by a URL.
 *
 * Returns: (FILE *)
 * Success: the open file pointer; Failure: NULL.
 */
FILE *http_connect(URLPtr url)
{
    int fd;
    char host[FILENAME_MAX];
    FILE *fp;

    sprintf(host, "%s:%d", url->domain, url->port);
    if ((fd = open_connect(host, AF_INET, SOCK_STREAM)) == -1)
    {
        return NULL;
    }
    fp = fdopen(fd, "r+");             /* convert to buffered stream */
    return fp;
}

/*
 * http_send_request() --Send a request line to an open HTTP session.
 *
 * Returns: (int)
 * Success: No. of bytes written; Failure: 0.
 */
size_t http_send_request(FILE * fp, const char *method,
                         URLPtr url, const char *version)
{
    char buf[FILENAME_MAX];
    char *end;

    if (strcmp(version, "1.0") == 0)
    {
        end = vstrcat(buf, method, " /", url->path,
                      (url->query != NULL) ? "?" : "",
                      (url->query != NULL) ? url->query : "",
                      (url->anchor != NULL) ? "#" : "",
                      (url->anchor != NULL) ? url->anchor : "", NULL);
    }
    else if (strcmp(version, "1.1") == 0)
    {
        end = vstrcat(buf, method, " ",
                      url->scheme, "://",
                      url->domain, ":", url->port, "/", url->path,
                      (url->query != NULL) ? "?" : "",
                      (url->query != NULL) ? url->query : "",
                      (url->anchor != NULL) ? "#" : "",
                      (url->anchor != NULL) ? url->anchor : "", NULL);
    }
    else
    {
        debug("unrecognised HTTP version: \"%s\"", version);
        errno = EINVAL;
        return 0;                      /* failure: unknown protocol version */
    }
    debug("http_send_request: \"%s\"", buf);

    end = vstrcat(end, " HTTP/", version, rfc_eol, NULL);
    return fwrite(buf, 1, (size_t) (end - buf), fp) == (size_t) (end - buf)
        ? (size_t) (end - buf) : 0;
}

/*
 * http_send_header() --Send a list of HTTP headers to an open HTTP session.
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
size_t http_send_header(FILE * fp, int n_header, SymbolPtr header)
{
    for (int i = 0; i < n_header; ++i)
    {
        if (fprintf(fp, "%s: %s%s",
                    header[i].name, header[i].value.string, rfc_eol) < 0)
        {
            debug("http_send_header: failed to send \"%s\"", header[i].name);
            return 0;                  /* failure: write failed somehow */
        }
    }
    return 1;                          /* success */
}

/*
 * http_request() --Perform an HTTP request on a url, and get the response.
 *
 * Parameters:
 * method   --specifies the protocol method (e.g. "GET")
 * http_req --specifies the details of the HTTP request
 * version  --specifies the version of HTTP protocol to use
 *
 * Returns: (HTTPResponsePtr)
 * Success: a malloc'd HTTP response; Failure: NULL.
 *
 * Remarks:
 * Note that the HTTP may "successfully" respond with a HTTP error.
 * In such cases, the http_get() succeeds, and returns the error response.
 */
HTTPResponsePtr http_request(const char *method,
                             HTTPRequestPtr http_req, const char *version)
{
    char buf[FILENAME_MAX];
    char *end;
    int n;
    FILE *fp;
    HTTPResponsePtr r;
    URLPtr url = &http_req->url;

    if ((fp = http_connect(url)) == NULL)
    {
        return NULL;
    }
    if (!http_send_request(fp, method, url, version))
    {
        return NULL;
    }
    if (http_req->header != NULL && (n = vector_len(http_req->header)) != 0)
    {
        if (!http_send_header(fp, n, http_req->header))
        {
            return NULL;
        }
    }

    fprintf(fp, "%s", rfc_eol);        /* blank line at end */
    fflush(fp);

    if (fgets(buf, NEL(buf) - 1, fp))
    {
        char protocol[FILENAME_MAX];
        char v[FILENAME_MAX];
        char msg[FILENAME_MAX];
        int code;

        buf[NEL(buf) - 1] = '\0';
        if (sscanf(buf, "%[^/]/%s %d %[^\r]", protocol, v, &code, msg) != 4)
        {
            debug("unrecognised HTTP header: \"%s\"", buf);
            errno = EINVAL;
            return NULL;               /* failure: unrecognised heaeder */
        }
        if ((r = NEW(HTTPResponse, 1)) == NULL)
        {
            return NULL;               /* failure: malloc error */
        }
        r->status = code;
    }
    else
    {
        return NULL;                   /* failure: fgets error */
    }

    /*
     * process headers...
     */
    while (fgets(buf, NEL(buf) - 1, fp) != NULL
           && (strcmp(buf, rfc_eol) != 0 && *buf != '\n'))
    {
        Symbol sym = {.type = STRING };

        if (r->header == NULL
            && (r->header = NEW_VECTOR(Symbol, 0, NULL)) == NULL)
        {
            return NULL;               /* failure: malloc error */
        }
        if ((end = strstr(buf, rfc_eol)) != NULL || (end = strchr(buf, '\n')))
        {
            *end = '\0';               /* zap EOL */
        }
        sym.name = strdup(buf);
        if ((end = strchr(sym.name, ':')) != NULL)
        {
            *end++ = '\0';
            while (*end == ' ')
            {
                ++end;
            }
            sym.value.string = end;
        }
        r->header = vector_add(r->header, 1, &sym);
    }
    /*
     * process body
     */
    while ((n = fread(buf, 1, NEL(buf), fp)) > 0)
    {
        if (r->content == NULL
            && (r->content = NEW_VECTOR(char, 0, NULL)) == NULL)
        {
            http_free_response(r);
            return NULL;               /* failure: malloc error */
        }
        r->content = vector_add((void *) r->content, (size_t) n, buf);
    }
    fclose(fp);
    return r;
}

/*
 * http_free_response() --Free the memory resources of an HTTP response.
 *
 * Parameters:
 * r    --the response to free.
 *
 * Remarks:
 * This routine makes some assumptions about the way that the
 * response was built; basically, it assumes that the header
 * field is a vector of Symbols, and that only the name field
 * needs to be freed.
 */
void http_free_response(HTTPResponsePtr r)
{
    if (r->header != NULL)
    {
        int n = vector_len(r->header);
        for (int i = 0; i < n; ++i)
        {
            free((void *) r->header[i].name);
        }
        free_vector(r->header);
    }
    if (r->content != NULL)
    {
        free_vector((void *) r->content);
    }
    free(r);
}
