#include "microhttp.h"
#include <stdio.h>
#include <string.h>

// Handler for GET /
void handle_root(HttpRequest *req, HttpResponse *res) {
    http_send(res, 200, "text/plain", "Welcome to the minimal HTTP server.");
}

// Handler for POST /submit
void handle_post(HttpRequest *req, HttpResponse *res) {
    // Print the raw body to the terminal for debugging
    printf("[INFO] POST /submit received:\n%s\n", req->body);

    // Build a response message echoing the received body
    char response[256];
    snprintf(response, sizeof(response), "Received body:\n%s", req->body);

    // Send the response to the client
    http_send(res, 200, "text/plain", response);
}

int main() {
    // Start the server on port 8080
    http_server_start(8080);

    // Register routes
    http_on("GET", "/", handle_root);
    http_on("POST", "/submit", handle_post);

    // Enter the request handling loop
    http_server_run();

    return 0;
}
