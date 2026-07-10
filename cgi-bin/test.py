# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test.py                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/09 00:36:19 by nmunari           #+#    #+#              #
#    Updated: 2026/07/10 02:15:40 by nmunari          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/usr/bin/env python3
import os
import sys

body = ""
content_length = os.environ.get("CONTENT_LENGTH", "0")
if content_length.isdigit() and int(content_length) > 0:
    body = sys.stdin.read(int(content_length))

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>CGI Test</h1>")
print(f"<p>METHOD: {os.environ.get('REQUEST_METHOD', '')}</p>")
print(f"<p>QUERY_STRING: {os.environ.get('QUERY_STRING', '')}</p>")
if body:
    print(f"<p>BODY: {body}</p>")
print("</body></html>")