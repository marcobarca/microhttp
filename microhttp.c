#include "microhttp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // for read, write, close
#include <arpa/inet.h>  // for sockaddr_in, inet_ntoa, htons

// Route structure used to map an HTTP method + path to a handler function
typedef struct {
    char method[8];            // HTTP method (e.g., GET, POST)
    char path[256];            // Request path (e.g., /, /submit)
    HttpHandler handler;       // Pointer to the function that handles the route
} Route;

// Array of registered routes
static Route routes[MAX_ROUTES];
static int route_count = 0;    // Counter of registered routes
static int server_fd;          // File descriptor for the server socket

// Initializes the server socket and binds it to the given port
void http_server_start(int port) {
    struct sockaddr_in server_addr;

    // Create a TCP socket (IPv4)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set socket options (IPv4, accept connections from any IP)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);               // Convert port to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;         // Listen on all interfaces

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Start listening for incoming connections (up to 10 in the queue)
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("✅ Server listening on http://localhost:%d\n", port);
}

// Registers a new route handler for a specific method and path
void http_on(const char *method, const char *path, HttpHandler handler) {
    if (route_count >= MAX_ROUTES) {
        fprintf(stderr, "Too many handlers registered\n");
        return;
    }

    // Store method, path, and handler in the routes array
    strncpy(routes[route_count].method, method, sizeof(routes[route_count].method));
    strncpy(routes[route_count].path, path, sizeof(routes[route_count].path));
    routes[route_count].handler = handler;
    route_count++;
}

// Sends an HTTP response to the client with given status, content type, and body
void http_send(HttpResponse *res, int status_code, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    int body_len = strlen(body);

    // Format the HTTP response with headers and body
    snprintf(response, sizeof(response),
             "HTTP/1.1 %d OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "\r\n"
             "%s",
             status_code, content_type, body_len, body);

    // Write the response to the client socket
    write(res->client_fd, response, strlen(response));
}

// Starts accepting connections and dispatching requests to handlers
void http_server_run() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Infinite loop: accept and handle incoming client requests
    while (1) {
        // Accept a new client connection
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Clear the buffer and read the request
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            close(client_fd);
            continue;
        }

        // Prepare request and response structures
        HttpRequest req = {0};
        HttpResponse res = { .client_fd = client_fd };

        // Extract HTTP method and path from the request line
        sscanf(buffer, "%s %s", req.method, req.path);
        req.client_fd = client_fd;

        // Locate the end of headers (\r\n\r\n) to separate headers and body
        char *headers_end = strstr(buffer, "\r\n\r\n");
        if (headers_end) {
            headers_end += 4; // Move past \r\n\r\n to point to the body
            strncpy(req.headers, buffer
