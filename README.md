# cHTTP
[![](https://img.shields.io/badge/marekvospel%2Fchttp-gh?logo=github&labelColor=555555&color=8da0cb)](https://github.com/marekvospel/chttp)
![C language](https://img.shields.io/badge/language-C-blue)  
[![GitHub License](https://img.shields.io/github/license/marekvospel/chttp)](https://github.com/marekvospel/chttp)

To improve my C programming skills, I decided to implement a HTTP server in C,
before my university C classes begin. The goal is to make something
"good enough" - not production ready in particular, but with no mistakes such
as use after free, memory leaks, buffer overflows etc.

## Licensing
I don't really care about how you use the code from this project. If you
understand licenses, feel free to chose Apache 2.0 or MIT at your option. If
you don't understand what I'm talking about, don't worry I won't sue you. :D

## TODO
- [X] Create a socket, bind it to an address and listen for connections
- [X] Parse The most important parts of the request such as request-line
- [ ] Parse headers
- [ ] Respond in HTTP compliant format
- [ ] Respond with 400 when bad request occurs
- [ ] Read coresponding file, without path traversal xd
- [ ] Multithreading (per request thread / thread pool)
- [ ] Connection timeout
