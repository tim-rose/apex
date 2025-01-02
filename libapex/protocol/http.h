/*
 * HTTP.H --Definitions for the HTTP protocol
 *
 * Remarks:
 * Why aren't I using curl?
 *
 */
#ifndef APEX_HTTP_H
#define APEX_HTTP_H

#include <apex/symbol.h>
#include <apex/url.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */

#define HTTP_V1_0	"1.0"              /* supported protcol versions */
#define HTTP_V1_1	"1.1"

#define HTTP_CONNECT	"CONNECT"      /* methods */
#define HTTP_DELETE	"DELETE"
#define HTTP_GET	"GET"
#define APEX_HTTP_HEAD	"HEAD"
#define HTTP_OPTIONS	"OPTIONS"
#define HTTP_PATCH	"PATCH"
#define HTTP_POST	"POST"
#define HTTP_PUT	"PUT"
#define HTTP_TRACE	"TRACE"

    typedef struct HTTPRequest
    {
        URL url;
        SymbolPtr header;
    } HTTPRequest, *HTTPRequestPtr;

    typedef struct HTTPResponse
    {
        URL url;
        int status;
        SymbolPtr header;
        const char *content;
    } HTTPResponse, *HTTPResponsePtr;

    FILE *http_connect(URLPtr url);
    size_t http_send_request(FILE * fp, const char *method,
                             URLPtr url, const char *version);
    size_t http_send_header(FILE * fp, int n_header, SymbolPtr header);

    HTTPResponsePtr http_request(const char *method,
                                 HTTPRequestPtr http_req,
                                 const char *version);
    void http_free_response(HTTPResponsePtr r);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* APEX_HTTP_H */
