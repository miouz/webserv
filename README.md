# 🌐 Webserv

> **A non-blocking HTTP server built from scratch in C++98.**

*This project was created as part of the 42 curriculum by&#x20;****mzhou****,&#x20;****jwuille****, and&#x20;****ceslinge****.*

Webserv explores what happens behind a web server: from accepting a TCP connection to parsing an HTTP request, resolving a route, executing CGI, and sending a response back to the client.

The server uses a **single&#x20;****`poll()`****-based event loop** to multiplex network I/O and handle multiple clients without blocking.

---

## 🐣 Description

The goal of Webserv is to implement an HTTP server in **C++98**, using **Nginx as a reference** for configuration and server behavior.

Instead of relying on an existing HTTP library or framework, we implemented the core mechanisms ourselves:

- TCP socket creation and lifecycle
- non-blocking I/O
- I/O multiplexing with `poll()`
- HTTP request parsing
- HTTP response generation
- configuration parsing
- routing
- static file serving
- file uploads
- CGI execution
- cookies and basic session management
- error handling

The server can host **multiple server configurations** and process multiple client connections through a single event loop.

---

## ✨ Features

### 🌐 HTTP

- `GET`
- `POST`
- `DELETE`
- HTTP request parsing
- HTTP response generation
- status and error responses
- request headers and body handling
- redirects
- custom error pages

### 📁 Static Content

- static file serving
- directory listing
- index files
- MIME type handling
- file uploads
- file deletion

### ⚙️ Server

- TCP stream sockets
- non-blocking file descriptors
- `poll()`-based I/O multiplexing
- multiple listening servers
- simultaneous client connections
- connection cleanup and timeout handling
- signal handling

### 🧩 Dynamic Content

- CGI execution
- Python CGI scripts
- PHP CGI scripts
- CGI environment construction
- CGI timeout handling
- cookies
- basic session management

### 🛠️ Configuration

Server behavior is controlled through a configuration file inspired by Nginx.

Configuration can define server and location behavior such as:

- listening ports
- routes / locations
- accepted HTTP methods
- document roots
- index files
- upload directories
- CGI configuration
- error pages

---

## 🏗️ Architecture

Webserv follows an **event-driven architecture** centered around a single `poll()` loop.

```text
                         ┌──────────────────┐
                         │      Client      │
                         │ Browser / curl   │
                         └────────┬─────────┘
                                  │
                                  │ TCP
                                  ▼
                     ┌────────────────────────┐
                     │    Listening Socket    │
                     └────────────┬───────────┘
                                  │
                              accept()
                                  │
                                  ▼
                     ┌────────────────────────┐
                     │      poll() Loop       │
                     │                        │
                     │   Non-blocking I/O     │
                     └────────────┬───────────┘
                                  │
                         POLLIN / POLLOUT
                                  │
                                  ▼
                     ┌────────────────────────┐
                     │     HTTP Request       │
                     │        Parser          │
                     └────────────┬───────────┘
                                  │
                                  ▼
                     ┌────────────────────────┐
                     │ Configuration & Route  │
                     │       Resolution       │
                     └────────────┬───────────┘
                                  │
                ┌─────────────────┼─────────────────┐
                │                 │                 │
                ▼                 ▼                 ▼
         ┌─────────────┐   ┌─────────────┐   ┌─────────────┐
         │ Static File │   │     CGI     │   │ POST/DELETE │
         │    / GET    │   │ Python/PHP  │   │   Handler   │
         └──────┬──────┘   └──────┬──────┘   └──────┬──────┘
                │                 │                 │
                └─────────────────┼─────────────────┘
                                  │
                                  ▼
                     ┌────────────────────────┐
                     │     HTTP Response      │
                     └────────────┬───────────┘
                                  │
                               POLLOUT
                                  │
                                  ▼
                               Client
```

---

## 🔄 Request Lifecycle

A typical HTTP request goes through the following stages:

```text
Client connects
      │
      ▼
Server accepts connection
      │
      ▼
poll() monitors the client socket
      │
      ▼
POLLIN event
      │
      ▼
Read incoming bytes
      │
      ▼
Parse HTTP request
      │
      ▼
Resolve server configuration
      │
      ▼
Resolve requested location
      │
      ▼
Select request handler
      │
      ├──── GET / static content
      │
      ├──── POST / upload
      │
      ├──── DELETE
      │
      └──── CGI
      │
      ▼
Build HTTP response
      │
      ▼
POLLOUT event
      │
      ▼
Send response
      │
      ▼
Keep or close connection
```

---

## 🧠 Engineering Challenges

### 🔌 Non-blocking I/O

One of the central challenges of the project was handling multiple clients without allowing one connection to block the entire server.

Listening and client sockets are configured for **non-blocking operation** and monitored through a single `poll()` loop.

The event loop reacts to events such as:

```text
POLLIN   → data is ready to be read
POLLOUT  → the socket is ready to send data
```

This required explicitly managing the lifecycle and state of each connection rather than relying on blocking socket calls.

---

### 📬 HTTP Parsing

TCP provides a stream of bytes — it does not provide complete HTTP requests.

The server therefore has to progressively interpret incoming data:

```text
Raw TCP bytes
      │
      ▼
Request line
      │
      ▼
Headers
      │
      ▼
Message body
      │
      ▼
Parsed HTTP request
```

The request parser accumulates received data and determines when enough information is available to process the request.

---

### 🗺️ Configuration & Routing

The server behavior is not hardcoded.

A configuration file inspired by **Nginx** is parsed into server and location configurations.

When a request arrives, Webserv determines which configuration applies before choosing how the request should be processed.

This separates:

```text
Networking
     │
HTTP parsing
     │
Configuration
     │
Routing
     │
Request handling
```

and keeps the server architecture modular.

---

### ⚡ CGI Execution

Webserv supports dynamic content using the **Common Gateway Interface (CGI)**.

For a CGI request, the server must:

1. identify the appropriate CGI executable;
2. construct the CGI environment;
3. prepare the request data;
4. execute the CGI program in a child process;
5. collect its output;
6. handle execution failures or timeouts;
7. transform the result into an HTTP response.

The project supports CGI scripts including:

```text
.py
.php
```

---

### 📂 Static Files & Uploads

GET requests can resolve files, directories and index pages.

Directory requests can either:

- serve an available index file;
- generate a directory listing;
- or redirect to the appropriate URI when necessary.

POST requests can store uploaded files in the configured upload directory while checking paths, permissions and existing files.

---

## 👥 Team & Contributions

Webserv was developed collaboratively by three students.

The areas below describe the **main contributions visible in the project's Git/PR history**. They should not be interpreted as strict ownership boundaries: features were reviewed, integrated, debugged and tested collaboratively throughout the project.

### 🧑‍💻 mzhou — Networking, Event Loop & Integration

Main areas of contribution:

- TCP socket lifecycle
- server socket setup with `socket()`, `bind()` and `listen()`
- non-blocking file descriptor setup
- central `poll()` event loop
- `POLLIN` / `POLLOUT` event dispatching
- client/server connection lifecycle
- multiple-server handling
- connection cleanup and error handling
- signal handling
- `SIGPIPE` handling
- CGI integration
- CGI timeout handling
- cookie/session integration
- project integration and testing

A major part of this work established the **event-driven networking foundation** on which the HTTP server operates.

---

### 🧑‍💻 jwuille — HTTP Parsing & Request Handling

Main areas of contribution:

- HTTP request parser
- progressive request buffering
- request completion detection
- parsing of:
  - HTTP method
  - URI
  - protocol
  - headers
  - request body
- parsing error/status handling
- GET request implementation
- static file serving
- directory listing
- index file resolution
- URI redirection
- POST request implementation
- upload handling
- upload path and permission validation
- request-related testing infrastructure

This work focused primarily on transforming **raw HTTP traffic into structured requests** and implementing the corresponding HTTP behavior.

---

### 🧑‍💻 ceslinge — Configuration & CGI

Main areas of contribution:

- configuration file parsing
- server configuration structures
- location configuration
- configuration validation
- server/location behavior definition
- CGI implementation
- CGI environment construction
- CGI process execution
- child-process handling
- CGI output collection
- integration of configuration data with server behavior

This work provided the **configuration layer and dynamic execution mechanisms** used by the HTTP server.

---

### 🤝 Collaborative Work

Beyond the individual areas above, the project required significant collaboration around:

- architecture decisions
- code reviews
- feature integration
- debugging
- memory and resource management
- HTTP behavior
- edge cases
- testing
- configuration behavior
- CGI integration
- final project validation

We used a feature-branch workflow with pull requests to integrate the different components of the server.

---

## ✍️ Build & Usage

### Compile

```bash
make
```

### Run with the provided configuration

```bash
./webserv webserv.conf
```

### Run with another configuration

```bash
./webserv <configuration_file>
```

---

## 🧪 Testing with curl

Once the server is running, requests can be sent using `curl`.

### GET

```bash
curl -X GET http://localhost:<port>/<uri>
```

### POST

```bash
curl -X POST http://localhost:<port>/<uri>
```

### DELETE

```bash
curl -X DELETE http://localhost:<port>/<uri>
```

The server can also be accessed directly from a browser:

```text
http://localhost:<port>/<uri>
```

---

## 📂 Project Structure

```text
webserv/
│
├── .github/
│   └── workflows/       # Continuous integration
│
├── cgi-bin/             # CGI scripts
├── errors/              # HTTP error pages
├── src/                 # Webserv implementation
├── test/                # Tests
│
├── main.cpp             # Program entry point
├── webserv.conf         # Example configuration
├── mime.types           # MIME type definitions
├── Makefile
├── CONTRIBUTING.md
└── README.md
```

---

## 🧪 Development Workflow

The project used a feature-branch workflow:

```text
main
 │
 ├── feature branch
 │      │
 │      ├── implementation
 │      ├── tests
 │      └── fixes
 │
 └◀──── Pull Request
          │
          ├── review
          └── merge
```

GitHub Actions were also introduced to automate compilation and mandatory tests.

---

## 🎯 What We Learned

Building Webserv required working with concepts that are normally abstracted away by frameworks and production web servers.

### Systems Programming

- file descriptors
- processes
- signals
- pipes
- resource management
- non-blocking I/O

### Networking

- TCP sockets
- client/server architecture
- connection lifecycle
- I/O multiplexing
- `poll()`

### HTTP

- HTTP message structure
- methods
- headers
- bodies
- status codes
- redirects
- cookies
- CGI

### Software Engineering

- modular C++ design
- configuration-driven architecture
- Git collaboration
- pull requests
- code reviews
- integration testing
- debugging multi-component systems

---

## 📖 Resources

Documentation and references used during development:

- [Nginx Beginner's Guide](https://nginx.org/en/docs/beginners_guide.html#conf_structure)
- [Getting Started with Nginx — French](https://www.ganatan.com/tutorials/demarrer-avec-nginx)
- [MDN — HTTP Messages](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages)
- [HTTP/1.0 Reference](https://http.dev/1.0)
- [MDN — Cookie Header](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers/Cookie)

---

## 🤖 Use of AI

AI tools were used during the project for:

- task division and milestone organization;
- explanations and mentoring around unfamiliar concepts and functions;
- suggestions for testing strategies;
- assistance creating test scripts for CGI execution.

The server architecture, implementation, debugging, integration and project decisions were carried out by the project team.

---

## 👨‍💻 Authors

**mzhou · jwuille · ceslinge**

*Webserv — 42 School*
