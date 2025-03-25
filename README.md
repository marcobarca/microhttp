# MicroHTTP

**MicroHTTP** is a minimalist HTTP server written in pure C with no external dependencies.  
It's designed for educational purposes or lightweight embedded use cases, allowing you to understand how HTTP servers work under the hood.

---

## Features

- Supports `GET` and `POST` requests
- Custom route handlers
- Parses headers and body manually (no HTTP libraries used)
- Works with plain text and JSON payloads
- Zero external dependencies — just standard C and sockets

---

## Project Structure

```
microhttp/
├── microhttp.h       # Public API header
├── microhttp.c       # HTTP server implementation
└── main.c            # Example app using the library
```

---

## Build & Run

### Compile
```bash
gcc main.c microhttp.c -o microhttp_server
```

### Run
```bash
./microhttp_server
```

The server will start and listen on `http://localhost:8080`.

---

## Usage Examples

### GET Request

```bash
curl http://localhost:8080/
```

### POST Request with JSON

```bash
curl -X POST http://localhost:8080/submit \
  -H "Content-Type: application/json" \
  -d '{"nome":"Marco", "cognome":"Barca"}'
```

Server response:

```
Nome: Marco
Cognome: Barca
```

---

## Limitations

- Only handles one request at a time (no concurrency)
- No HTTPS (pure HTTP only)
- No routing wildcards or query parameter parsing (yet)
- Only supports simple body formats (text / raw JSON)

---

## Example: Define Routes

In `main.c`:

```c
void handle_root(HttpRequest *req, HttpResponse *res) {
    http_send(res, 200, "text/plain", "Welcome to microhttp!");
}

void handle_post(HttpRequest *req, HttpResponse *res) {
    http_send(res, 200, "application/json", "{\"status\":\"ok\"}");
}

int main() {
    http_server_start(8080);
    http_on("GET", "/", handle_root);
    http_on("POST", "/submit", handle_post);
    http_server_run();
}
```

---

## Future Ideas

- Multithreading or forking to handle concurrent clients
- Support wildcard routes: `/user/:id`
- Handle query strings and URL decoding
- Static file serving (HTML, CSS, etc.)

---

## License

MIT License — free to use, modify and share.
