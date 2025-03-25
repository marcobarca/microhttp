#ifndef MICROHTTP_H
#define MICROHTTP_H

#define MAX_ROUTES 16
#define BUFFER_SIZE 8192

typedef struct {
    char method[8];
    char path[256];
    char headers[2048];
    char body[4096];
    int client_fd;
} HttpRequest;

typedef struct {
    int client_fd;
} HttpResponse;

typedef void (*HttpHandler)(HttpRequest *, HttpResponse *);

void http_server_start(int port);
void http_on(const char *method, const char *path, HttpHandler handler);
void http_server_run();
void http_send(HttpResponse *res, int status_code, const char *content_type, const char *body);

#endif
