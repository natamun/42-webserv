#!/usr/bin/env python3
# Lance chaque config d'erreur, envoie la requete correspondante, affiche
# le code et le HTML renvoye.
# Usage: python3 tests/test_bad_conf_errors.py (depuis la racine du projet)
import signal
import subprocess
import time

WEBSERV = "./webserv"
HOST = "http://127.0.0.1:8080"

CASES = [
    ("tests/bad_conf/error-301.conf", "redirection",
     ["curl", "-s", f"{HOST}/redirect-me"]),
    ("tests/bad_conf/error-403.conf", "dossier sans autoindex",
     ["curl", "-s", f"{HOST}/forbidden/"]),
    ("tests/bad_conf/error-403.conf", "path traversal",
     ["curl", "-s", "--path-as-is", f"{HOST}/forbidden/../etc/passwd"]),
    ("tests/bad_conf/error-404.conf", "fichier introuvable",
     ["curl", "-s", f"{HOST}/doesnotexist"]),
    ("tests/bad_conf/error-405.conf", "methode refusee",
     ["curl", "-s", "-X", "POST", "-d", "x", f"{HOST}/get-only"]),
    ("tests/bad_conf/error-413.conf", "body trop gros",
     ["curl", "-s", "-X", "POST", "-d", "This body is over 20 bytes long", f"{HOST}/"]),
    ("tests/bad_conf/error-500.conf", "upload sans upload_path",
     ["curl", "-s", "-X", "POST", "-d", "x", f"{HOST}/no-upload/test.txt"]),
    ("tests/bad_conf/error-502.conf", "cgi cassee",
     ["curl", "-s", f"{HOST}/cgi-bin/broken_output.py"]),
    ("tests/bad_conf/error-504.conf", "cgi timeout",
     ["curl", "-s", "--max-time", "15", f"{HOST}/cgi-bin/infinite_loop.py"]),
]

for conf, desc, curl_cmd in CASES:
    print(f"--- {desc} ---")
    server = subprocess.Popen([WEBSERV, conf], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(1)

    result = subprocess.run(curl_cmd + ["-w", "\n%{http_code}"], capture_output=True, text=True)
    body, code = result.stdout.rsplit("\n", 1)

    server.send_signal(signal.SIGINT)
    server.wait()
    time.sleep(1)

    print(f"code: {code}")
    print(body)
    print()
