# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    echo.py                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/10 02:28:28 by nmunari           #+#    #+#              #
#    Updated: 2026/07/10 02:28:29 by nmunari          ###   ########.fr        #
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
print("<h1>CGI Echo</h1>")
print("<h2>Environment</h2><ul>")
for key, value in os.environ.items():
    print(f"<li><b>{key}</b> = {value}</li>")
print("</ul>")
if body:
    print(f"<h2>Body</h2><pre>{body}</pre>")
print("</body></html>")
