# Webserv

*This project has been created as part of the 42 curriculum by mzhou, jwuille, ceslinge*

## **Description**
___
	This project consists in building a web server based on HTTP 1.0 protocol in C++98 by taking Nginx as reference. The program can host one or multiple servers who accept clients' requests via TCP stream sockets.
	It handles GET, POST and DELETE HTTP methods with CGI executions (with .py and .php extensions). It also allows basic cookies session management, file uploads. 

	The servers are constructed with a non-blocking multiplexing architecture handled by a single poll() loop, which allows to never block on system I/O.

## **Instructions**
___
	Compile with: make
	Exec with our configuration file: ./webserv webserv.conf
	Exec with your own configuration file: ./webserv [configuration file]
	Once the program is running, use in another terminal:
		- "curl -X [METHOD] http://localhost:[port]/uri" 
		- browser such as firefox/brave with url: http://localhost:[port]/uri

## **Resources**
___
- Nginx beginner's guide: https://nginx.org/en/docs/beginners_guide.html#conf_structure
- Start with Nginx (fr): https://www.ganatan.com/tutorials/demarrer-avec-nginx
- HTTP messages guide: https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages
- HTTP 1.0 features: https://http.dev/1.0
- Cookie ref: https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers/Cookie
