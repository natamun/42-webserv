# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test.py                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/09 00:36:19 by nmunari           #+#    #+#              #
#    Updated: 2026/07/09 00:37:07 by nmunari          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/usr/bin/env python3
import os

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>CGI Test</h1>")
for key, value in os.environ.items():
    print(f"<p>{key} = {value}</p>")
print("</body></html>")