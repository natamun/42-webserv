#!/usr/bin/env python3
import subprocess
import sys

WEBSERV = "./webserv"
TIMEOUT = 2

CASES = [
    ("tests/bad_conf/duplicate-port.conf", "deux serveurs sur le meme port"),
    ("tests/bad_conf/privileged-port.conf", "port < 1024 sans root"),
    ("tests/bad_conf/unassigned-ip.conf", "IP valide mais non assignee localement"),
    ("tests/bad_conf/invalid-host.conf", "host invalide (pas une IP)"),
]


def run_case(conf_path, description):
    print(f"--- {conf_path} ({description}) ---")
    try:
        result = subprocess.run(
            [WEBSERV, conf_path], timeout=TIMEOUT, capture_output=True, text=True
        )
    except subprocess.TimeoutExpired:
        print(f"  FAIL: toujours en cours apres {TIMEOUT}s (devrait refuser de demarrer)")
        return False

    if result.returncode == 0:
        print("  FAIL: exit code 0 (le serveur a demarre alors qu'il aurait du refuser)")
        return False

    output = (result.stdout + result.stderr).strip()
    if "Error" not in output:
        print(f"  FAIL: exit code {result.returncode} mais pas de message clair: {output!r}")
        return False

    print(f"  OK: exit code {result.returncode}, message: {output}")
    return True


def main():
    results = [run_case(conf, desc) for conf, desc in CASES]
    passed = sum(results)
    total = len(results)
    print(f"\n{passed}/{total} cas corrects.")
    sys.exit(0 if passed == total else 1)


if __name__ == "__main__":
    main()
