# Client-Server Project (HTTP)

**Languages:** [Portuguese](README.md) | [English](README.en.md)

---

## General Description

This project implements a **Client-Server system in C**, using **TCP sockets** and the **HTTP/1.1** protocol, capable of handling requests from browsers (such as Chrome, Firefox, and Edge) as well as from a custom C client. The server lists files, serves HTML pages, and returns the appropriate HTTP status codes (200, 404, etc). The client performs HTTP file requests and saves them to the local folder.

---

## Project Structure

```
.
├── Client/
│ ├── meunavegador.c
├── Server/
│ ├── meuservidor.c
├── README.md
└── Makefile
```

- `Server/` - HTTP server source code.  
- `Client/` - Client source code for sending requests.  
- `Makefile` - Build automation file.  
- `README.md` - Project documentation.

---

## Technologies Used
- **C language**
- **BSD Sockets (TCP/IP)**
- **HTTP/1.1 protocol**
- **Linux (Ubuntu)**

---

## How to Compile

> **Requirements**  
> Make sure the GCC compiler is installed
> ```bash
> sudo apt install build-essential
> ```

In the project root directory, run in the terminal:

```bash
make
```

The object and executable files will be created inside their respective source folders, as shown below:

```
.
├── Client/
│ ├── meunavegador.c
| ├── meunavegador.o
| ├── meunavegador
├── Server/
│ ├── meuservidor.c
│ ├── meuservidor.o
│ ├── meuservidor
```

To remove object and executable files, run:

```bash
make clean
```

To rebuild, run:

```bash
make rebuild
```

---

## How to Run

### Server

Open a terminal, navigate to the `Server` folder and execute:

```bash
./meuservidor /home/usr/path/to/directory
```

This command starts the server and serves the specified directory.
The server supports multiple file types such as **.html, .gif, .pdf, .jpeg, .png**, among others.

If the requested file does not exist, the server returns **404 Not Found**.

Additionally, the server has a two-minute timeout, if a client remains inactive, the connection is closed and the server waits for new requests.

### Client

Open another terminal, navigate to the `Client` folder, and execute:

```bash
./meunavegador http//host:port/index.html
```

This command performs an request. If no port is specified, the client defaults to port **80 (HTTP standard)**.

If no specific file is requested or index.html is missing, the client receives an HTML response listing the files available in the server directory.

You can also access the server using a standard browser — just type the host, port, and file path in the address bar.

---

## Request Example

Request:

```
GET /index.html HTTP/1.1
Host: 127.0.0.1
Connection: close
```

Response:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Lenght: 48
Connection: close

<html>
  <body>
    <h1>Hello World!</h1>
  </body>
</html>
```

---

## Features

- ✅ Support for multiple simultaneous connections (multi-client)
- ✅ Complete HTTP header handling
- ✅ Compatible with real browsers
- ✅ Error handling (404, 400)
- ✅ Automatic directory listing
- ✅ Connection logs displayed in the terminal
- ✅ Client inactivity timeout

---

## Author

`Arthur Carvalho Rodrigues Oliveira`

Project developed for studying Network Programming.

---

## License

This project is licensed under the MIT License — feel free to use or modify it as you wish.