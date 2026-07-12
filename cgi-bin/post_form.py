#!/usr/bin/env python3
import os
import sys
from urllib.parse import parse_qs

body = ""
content_length = os.environ.get("CONTENT_LENGTH", "0")
if content_length.isdigit() and int(content_length) > 0:
    body = sys.stdin.read(int(content_length))

fields = parse_qs(body)

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>POST form received (python)</h1>")
print(f"<p>METHOD: {os.environ.get('REQUEST_METHOD', '')}</p>")
if fields:
    print("<ul>")
    for key, values in fields.items():
        for value in values:
            print(f"<li><b>{key}</b> = {value}</li>")
    print("</ul>")
else:
    print("<p>(no form fields received)</p>")
print("</body></html>")
