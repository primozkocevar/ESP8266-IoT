# -*- coding: utf-8 -*-
"""
Created on Wed Apr 20 20:30:15 2016

Studied with:
https://docs.python.org/2/library/simplehttpserver.html
https://docs.python.org/2/library/basehttpserver.html#module-BaseHTTPServer

@author: Primoz Kocevar
"""
import SimpleHTTPServer
import SocketServer

PORT=8080

Handler = SimpleHTTPServer.SimpleHTTPRequestHandler

httpd=SocketServer.TCPServer(("",PORT), Handler)

print "serving at port", PORT
httpd.serve_forever