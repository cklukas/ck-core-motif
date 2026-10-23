"""Verify a clipboard owner survives a peer disconnect at the INCR terminator.

Run on an isolated display with explicit fresh output. The wait uses the owner's
reported native selection timeout; the same owner must then serve a new peer.
"""
import hashlib
import json
import os
import pathlib
import select
import subprocess
import sys
import time

fixture = pathlib.Path(sys.argv[1]).resolve(strict=True)
output = pathlib.Path(sys.argv[2]).absolute()
output.mkdir()
sample = "Cafe\u0301 e\u0301 שלום العربية 中文 👩\u200d💻 🚀 | ".encode()
payload = sample * (1048576 // len(sample) + 1)
source = output / "payload.utf8"
source.write_bytes(payload)
records = []
passed = False
started = time.time()
try:
    for kind in ("text", "field"):
        with (output / (kind + "-owner.log")).open("w") as log:
            owner = subprocess.Popen([str(fixture), "native-owner", kind, str(source)],
                                     stdout=subprocess.PIPE, stderr=log, text=True)
            record = {"widget": kind, "pid": owner.pid}
            records.append(record)
            try:
                assert select.select([owner.stdout], [], [], 60)[0], "owner readiness timeout"
                ready = json.loads(owner.stdout.readline())
                assert ready["ready"] and ready["bytes"] == len(payload)
                record["ready"] = ready
                record["mapped_libraries"] = sorted({line.split()[-1] for line in
                    pathlib.Path(f"/proc/{owner.pid}/maps").read_text().splitlines()
                    if "/" in line and ".so" in line})
                cancelled = output / (kind + "-cancelled.utf8")
                peer = subprocess.run([str(fixture), "request-cancel", str(cancelled)],
                                      capture_output=True, text=True, timeout=35)
                record["cancel_peer"] = {"exit": peer.returncode, "stdout": peer.stdout,
                                         "stderr": peer.stderr}
                assert peer.returncode == 0 and cancelled.read_bytes() == payload
                assert json.loads(peer.stdout)["cancelled"]
                wait = ready["selection_timeout_ms"] / 1000 + 0.5
                record["wait_seconds"] = wait
                time.sleep(wait)
                assert owner.poll() is None, "owner exited during cancelled-transfer cleanup"
                received = output / (kind + "-fresh.utf8")
                peer = subprocess.run([str(fixture), "request", str(received)],
                                      capture_output=True, text=True, timeout=35)
                record["fresh_peer"] = {"exit": peer.returncode, "stdout": peer.stdout,
                                        "stderr": peer.stderr}
                assert peer.returncode == 0 and received.read_bytes() == payload
                assert owner.poll() is None, "owner exited after a fresh complete transfer"
                record["passed"] = True
            finally:
                if owner.poll() is None:
                    owner.terminate()
                owner.wait(timeout=5)
                record["owner_exit"] = owner.returncode
        errors = (output / (kind + "-owner.log")).read_text()
        assert not errors, errors
    passed = True
finally:
    (output / "receipt.json").write_text(json.dumps({"passed": passed,
        "started": started, "ended": time.time(), "fixture": str(fixture),
        "payload_bytes": len(payload), "payload_sha256": hashlib.sha256(payload).hexdigest(),
        "records": records}, indent=2) + "\n")
    if passed:
        for path in output.glob("*.utf8"):
            path.unlink()
if passed:
    print("Cancelled INCR cleanup and fresh transfers passed for Text and TextField")
