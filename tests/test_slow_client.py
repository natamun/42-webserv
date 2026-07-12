#!/usr/bin/env python3
import signal
import subprocess
import time

WEBSERV = "./webserv"
CONF = "default.conf"
HOST = "http://127.0.0.1:8080"

server = subprocess.Popen([WEBSERV, CONF], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
time.sleep(1)

slow = subprocess.Popen(
    ["curl", "-s", "-o", "/dev/null", "-w", "%{http_code}", "--limit-rate", "10",
     "-X", "POST", "-d", "x" * 50, HOST + "/upload/slow.txt"],
    stdout=subprocess.PIPE, text=True,
)
time.sleep(1)

start = time.time()
fast = subprocess.run(["curl", "-s", "-o", "/dev/null", "-w", "%{http_code}", HOST + "/"],
                      capture_output=True, text=True)
fast_elapsed = time.time() - start

slow_code = slow.communicate()[0]

server.send_signal(signal.SIGINT)
server.wait()

print(f"client rapide: {fast.stdout} en {fast_elapsed:.2f}s (pendant que le lent envoyait)")
print(f"client lent: {slow_code}")
