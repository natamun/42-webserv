#!/usr/bin/env python3
import signal
import subprocess
import time

WEBSERV = "./webserv"
CONF = "tests/cgi-test.conf"
HOST = "http://127.0.0.1:8080"

server = subprocess.Popen([WEBSERV, CONF], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
time.sleep(1)

result = subprocess.run(
    ["curl", "-s", "-o", "/dev/null", "-w", "%{http_code} %{size_download}",
     HOST + "/cgi-bin/big_output.py"],
    capture_output=True, text=True,
)

server.send_signal(signal.SIGINT)
server.wait()

code, size = result.stdout.split()
print(f"code: {code}, taille recue: {size} octets (attendu 20000000)")
