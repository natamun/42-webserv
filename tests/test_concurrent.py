#!/usr/bin/env python3
import signal
import subprocess
import time

WEBSERV = "./webserv"
CONF = "default.conf"
HOST = "http://127.0.0.1:8080"
N = 30

server = subprocess.Popen([WEBSERV, CONF], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
time.sleep(1)

start = time.time()
procs = [
    subprocess.Popen(["curl", "-s", "-o", "/dev/null", "-w", "%{http_code}", HOST + "/"],
                     stdout=subprocess.PIPE, text=True)
    for _ in range(N)
]
codes = [p.communicate()[0] for p in procs]
elapsed = time.time() - start

server.send_signal(signal.SIGINT)
server.wait()

print(f"{codes.count('200')}/{N} requetes a 200, en {elapsed:.2f}s")
