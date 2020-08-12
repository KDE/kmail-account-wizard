# -*- coding: utf-8 -*-
# SPDX-FileCopyrightText: 2015 Sandro Knauß <knauss@kolabsys.com>

# SPDX-License-Identifier: LGPL-2.0-or-later


import BaseHTTPServer
import os.path

class ErrorServer(BaseHTTPServer.BaseHTTPRequestHandler):
    '''a simple server that always answers with the corresponding error code:
    /500 -> error 500
    '''
    def do_GET(self):
        self.send_error(int(os.path.basename(self.path)))

def run(server_class=BaseHTTPServer.HTTPServer,
        handler_class=ErrorServer):
    global keep_running
    server_address = ('localhost', 8000)
    httpd = server_class(server_address, handler_class)
    httpd.serve_forever()

run()
