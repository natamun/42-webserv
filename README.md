# webserv

*This project has been created as part of the 42 curriculum by dufama, nmuari.*

[![Build](https://github.com/natamun/42-webserv/actions/workflows/build.yml/badge.svg)](https://github.com/natamun/42-webserv/actions/workflows/build.yml)
![C++](https://img.shields.io/badge/language-C%2B%2B98-blue.svg)

HTTP/1.1 server written from scratch in C++98. A single `poll()` call drives every read and write between the server and its clients (listening sockets, connections, and CGI pipes included); no blocking I/O outside of regular disk files.

## Features

- Single event loop based on `poll()`, non-blocking sockets and pipes
- GET, POST, DELETE
- CGI execution based on file extension (Python, PHP), with chunked request bodies un-chunked before being passed to the script
- File uploads to a configurable storage directory
- Static file serving, with optional directory listing (`autoindex`) and a configurable default index file
- Configuration file: multiple server blocks (ports/hosts), virtual host matching on the `Host` header, custom error pages, request body size limit, per-location allowed methods, redirects
- Client identification and per-client session tracking through cookies

## Requirements

- A C++98-compatible compiler (`c++`/`g++`/`clang++`)
- POSIX headers for sockets, `poll()`, and process management (Linux, macOS)
- Python 3 and/or `php-cgi` if you want to exercise the demo CGI scripts

## Build

| Target | Description |
|---|---|
| `make` / `make all` | Builds the `webserv` binary |
| `make clean` | Removes compiled object files |
| `make fclean` | Removes object files and the `webserv` binary |
| `make re` | `fclean` then `all` |
| `make debug` | Rebuilds with `-g` |
| `make test FILE=tests/x.cpp` | Compiles and runs `x.cpp` against the rest of the codebase, excluding `main.cpp`: `x.cpp` supplies its own `main()` in place of the real one |

Every pull request to `main` is checked by CI before it can be merged: `clang-format` first, then `make`.

## Usage

```bash
./webserv <config_file>
```

The configuration file path is required; running the binary with zero or more than one argument prints a usage message and exits with a non-zero status.

```bash
./webserv default.conf
```

## Configuration

The configuration file groups one or more `server` blocks, each containing one or more `location` blocks.

**Server-level directives**

| Directive | Description |
|---|---|
| `listen <port>;` | Port to listen on |
| `host <ip>;` | Interface to bind to |
| `server_name <name>;` | Matched against the `Host` header for virtual hosting |
| `root <path>;` | Default root directory for this server's locations |
| `client_max_body_size <bytes>;` | Maximum accepted request body size |
| `error_page <code> <path>;` | Custom error page for a given status code |
| `location <path> { ... }` | Route-specific rules, see below |

**Location-level directives**

| Directive | Description |
|---|---|
| `allow_methods <METHOD ...>;` | HTTP methods accepted on this route (required) |
| `root <path>;` | Directory this location's files are read from |
| `index <file>;` | Default file served when the request targets a directory |
| `autoindex on\|off;` | Enables or disables directory listing |
| `upload_path <path>;` | Directory POST bodies are written to |
| `return <path>;` | Redirects any request to this route (HTTP 301) |
| `cgi <extension> <interpreter path>;` | Maps a file extension to a CGI interpreter |

Example:

```nginx
server {
    listen 8080;
    host 127.0.0.1;
    server_name localhost;
    root www;
    client_max_body_size 10000000;
    error_page 404 www/errors/404.html;

    location / {
        allow_methods GET;
        root www;
        index index.html;
        autoindex off;
    }

    location /upload {
        allow_methods GET POST DELETE;
        root www/upload;
        upload_path www/upload;
    }

    location /cgi-bin {
        allow_methods GET POST;
        root cgi-bin;
        cgi .py /usr/bin/python3;
        cgi .php /usr/bin/php-cgi;
    }
}
```

An unrecognized directive at either level, or a location without `allow_methods`, causes the parser to throw and the server to exit before binding any socket.

## Running the demo

`default.conf` at the repository root serves `www/` on `127.0.0.1:8080`.

```bash
./webserv default.conf &

curl http://127.0.0.1:8080/
curl -X POST -d "hello" http://127.0.0.1:8080/upload/test.txt
curl http://127.0.0.1:8080/upload/test.txt
curl -X DELETE http://127.0.0.1:8080/upload/test.txt
curl http://127.0.0.1:8080/cgi-bin/echo.py
```

`tests/` contains additional configs, each isolating one scenario:

```bash
./webserv tests/multi-port.conf        # two servers, two ports
./webserv tests/multi-hostname.conf    # virtual hosting on the Host header
./webserv tests/cgi-test.conf          # CGI only
./webserv tests/bad_conf/duplicate-port.conf   # expected to fail to start
```

## Project structure

```bash
.
├── Makefile
├── default.conf               # ready-to-run demo config
├── includes/                  # one header per class/module
├── srcs/
│   ├── main.cpp
│   ├── WebServer.cpp          # event loop: poll(), accept, read, write, CGI dispatch
│   ├── Client.cpp             # per-connection state
│   ├── ConfigParser.cpp       # tokenizes and parses the .conf grammar
│   ├── HttpRequest.cpp        # incremental HTTP request parser
│   ├── HttpResponse.cpp       # HTTP response builder
│   ├── Router.cpp             # stateless server/location/CGI matching
│   ├── CookieManager.cpp      # client identification and session tracking
│   └── handlers/
│       ├── GetHandler.cpp     # serves static files and directory listings
│       ├── PostHandler.cpp    # writes uploaded bodies under upload_path
│       ├── DeleteHandler.cpp  # removes the resolved file
│       └── CgiHandler.cpp     # fork + pipe + execve bridge to CGI scripts
├── cgi-bin/                   # demo CGI scripts (Python and PHP)
├── www/, www2/                # demo static sites served by the sample configs
└── tests/                     # unit tests, integration scripts, and fixture configs
```

## How webserv works

### The event loop

`WebServer.cpp` calls `poll()` once per iteration on a single array holding every listening socket, every connected client, and every open CGI pipe: sockets, connections and pipes are all just fds sitting in the same set, so nothing outside of `poll()` ever decides whether an fd is ready. `revents` tells the loop what to do next: `accept()` on a listening socket, read or write on a client, read or write on a CGI pipe. Every fd is non-blocking, and `read()`/`write()`/`recv()`/`send()` are only ever called once `poll()` has actually reported that fd ready, never speculatively. A client fd watches `POLLIN` while its request is still incomplete and switches to `POLLOUT` the moment a response is queued, never both at once, so the loop always knows which direction a given fd is currently moving in.

Closing a client or a CGI process removes its entry from that array mid-iteration, which shifts every following index down by one; the loop compensates by decrementing its own index whenever that happens, so no fd is silently skipped. Connections are closed after every response (no keep-alive): this is also why session state can't just live "on the connection", see [Cookies and sessions](#cookies-and-sessions) below. A CGI process running longer than 5 seconds is killed with `SIGKILL` and answered with `504`, so one hung script can never freeze the loop for anyone else.

Before the loop even starts, `SIGCHLD` is set to `SIG_IGN`: the kernel then reaps every CGI child itself the moment it exits, with no explicit `waitpid()` call and no zombie processes ever accumulating. `SIGPIPE` is ignored too, since writing to a socket or pipe the other side already closed would otherwise raise it and kill the whole server; the `write()`/`send()` call just fails with `EPIPE` instead, and is handled like any other failed write.

### Configuration parsing

`ConfigParser.cpp` tokenizes the config file on whitespace, `{`, `}`, `;`, and `#` comments, then parses that token stream directly with a small hand-written recursive descent: no external grammar library, no generated parser. Any directive it doesn't recognize, at either the server or the location level, throws immediately and the server exits before binding a single socket, so a typo in the config is caught at startup rather than surfacing later as a confusing runtime error. `allow_methods` is the one directive a location can't omit: it's what the router consults before dispatching any request, so a location without it would leave every request to it ambiguous.

### Routing

`Router.cpp` is a small set of stateless free functions rather than a class: given a request and the already-parsed config, `findServer()`, `findLocation()` and `isCgi()` just compute an answer, with nothing to keep between calls. `findServer()` matches the request's `Host` header against the `server_name` of servers listening on the connection's actual bound port, read back with `getsockname()` rather than trusted from the config, since a server bound to `0.0.0.0` can be reached through more than one interface. If the `Host` header doesn't match any configured `server_name` on that port, it falls back to the last server declared on that port instead: unlike nginx, whose default server for a port is the first one declared, this fallback keeps overwriting itself as it walks the list, so whichever matching-port server comes last in the config wins. `findLocation()` then picks the longest configured location path that prefixes the request path, so `/upload/images` matches a `/upload` location over a `/` one. `isCgi()` checks the request's file extension against that location's configured CGI map to decide whether `CgiHandler.cpp` gets involved at all.

### Request parsing

A request rarely arrives in a single `read()`: a slow client or a large body can spread it across many `poll()` cycles, so `HttpRequest.cpp` can't assume it ever sees the whole thing at once. It's a small state machine instead, `PARSING_REQUEST_LINE` → `PARSING_HEADERS` → `PARSING_BODY` or `PARSING_CHUNKED_BODY` → `COMPLETE`/`ERROR`, fed one `appendData()` call per readable event; only the current parsing position is kept as state, never a full re-parse of everything received so far. Chunked bodies are decoded as their chunks arrive rather than being buffered whole and unchunked at the end. `client_max_body_size` is checked after every read that grows the body, not only once the full body is in: a client sending an oversized body one small chunk at a time is rejected as soon as the limit is crossed, not after the entire oversized body has already been buffered.

### Static files: GET, POST, DELETE

`handlers/GetHandler.cpp`, `PostHandler.cpp` and `DeleteHandler.cpp` all resolve a request path the same way: strip the matched location's prefix, then append what's left to that location's `root` (or `upload_path` for POST). GET serves the resulting file directly; if the path resolves to a directory, it first tries the location's configured `index` file, and only falls back to generating a directory listing if there's no index file and `autoindex` is on (anything else, a directory with neither, answers `403`). POST writes the request body to a file under `upload_path`. DELETE removes the resolved file. All three refuse to resolve any path containing `..`, so a request can't walk back out of the directory it's supposed to be confined to.

### CGI

`handlers/CgiHandler.cpp` forks after opening two non-blocking pipes, one for the child's stdin and one for its stdout, and folds both into the same `poll()` set as every socket: a slow CGI script blocks nothing else in the loop. The child `dup2`s the pipes onto its standard streams, `chdir()`s into the location's root so the script's own relative paths behave as if it had been launched from its own directory, builds a CGI/1.1 environment (`REQUEST_METHOD`, `QUERY_STRING`, `SCRIPT_FILENAME`, `CONTENT_LENGTH`, `CONTENT_TYPE`, `SERVER_NAME`, `SERVER_PORT`, `HTTP_COOKIE`, ...), and `execve`s the interpreter configured for that extension. Any chunked request body is un-chunked before reaching the script, since CGI/1.1 only understands a plain body announced through `CONTENT_LENGTH`. The parent streams the body into the child's stdin and reads its stdout until EOF: no `Content-Length` is required from the script either, the pipe closing is what marks the end of its output. A script still running after 5 seconds is killed with `SIGKILL` and answered with `504`.

### Cookies and sessions

Because connections close after every response, there's no keep-alive state to recognize "the same browser" across two separate requests: the only thing that survives between them is whatever the browser itself stores and resends, a cookie. `CookieManager.cpp` assigns an opaque `client_id` cookie to a client that shows up without one, once, and never rewrites it as long as the client keeps sending it back (`ensureClientId`). The actual state, currently a per-client request counter, lives server-side in a map keyed by that id rather than inside the cookie itself (`trackRequest`): the client could edit or forge the cookie's value, but not the counter, since the counter was never in the cookie to begin with. That's what makes it a session rather than plain client-side storage. The counter is exposed to the browser through a `Request-Count` response header, and the same cookie is forwarded into CGI scripts as `HTTP_COOKIE`, so a script can recognize the client too.

## Testing

```bash
make test FILE=tests/test_requesthttp.cpp
make test FILE=tests/test_httpresponse.cpp
make test FILE=tests/test_configparser.cpp
```

Unit tests cover `HttpRequest`, `HttpResponse`, and `ConfigParser` directly, no sockets involved. Integration scripts drive the compiled binary against a real client:

```bash
python3 tests/test_bad_conf_init.py     # every startup failure mode
python3 tests/test_bad_conf_errors.py   # every HTTP error status code
python3 tests/test_concurrent.py        # concurrent requests aren't serialized
python3 tests/test_big_download.py      # a large CGI response is sent in full
python3 tests/test_slow_client.py       # a throttled client doesn't block others
```

---

Made with ❤️ by [Dus4n808](https://github.com/Dus4n808) and [natamun](https://github.com/natamun)
