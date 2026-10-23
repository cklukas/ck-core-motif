"""Actual XmText/XmTextField clipboard round trips against a separate Xlib peer.
Run with LC_ALL=C.UTF-8 under an isolated Xvfb; no clipboard manager is needed.
Both directions verify the actual INCR handshake and exact received UTF-8 bytes.
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
started = time.time()
records = []
children = []
passed = False
sample = "Cafe\u0301 e\u0301 שלום العربية 中文 👩\u200d💻 🚀 | ".encode()
payloads = {"unicode": sample, "large": sample * (1048576 // len(sample) + 1)}


def launch(*args):
    process = subprocess.Popen([str(fixture), *map(str, args)], stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE, text=True)
    children.append(process)
    return process


def message(process):
    if not select.select([process.stdout], [], [], 35)[0]:
        raise RuntimeError("fixture did not complete clipboard operation")
    line = process.stdout.readline()
    if not line:
        raise RuntimeError("fixture exited: " + process.stderr.read())
    return json.loads(line)


def request_payload(path):
    reply = subprocess.run([str(fixture), "request", str(path)], capture_output=True,
                           text=True, timeout=35)
    if reply.returncode:
        records.append({"request_argv": reply.args, "returncode": reply.returncode,
                        "stdout": reply.stdout, "stderr": reply.stderr})
        raise RuntimeError("raw selection request failed: " + reply.stderr)
    return json.loads(reply.stdout)


def stop(process):
    if process.poll() is None:
        process.terminate()
    stdout, stderr = process.communicate(timeout=5)
    children.remove(process)
    if stderr:
        records.append({"fixture_stderr": stderr, "remaining_stdout": stdout})
        raise RuntimeError(stderr)


try:
    for name, payload in payloads.items():
        source = output / (name + ".utf8")
        source.write_bytes(payload)
        fingerprint = hashlib.sha256(payload).hexdigest()
        for kind in ("text", "field"):
            native = launch("native-owner", kind, source)
            assert message(native)["ready"]
            received = output / (name + "-" + kind + "-native-to-raw.utf8")
            stats = request_payload(received)
            assert received.read_bytes() == payload, "native copy changed UTF-8 bytes"
            if name == "large":
                assert stats["incr"] and stats["chunks"] > 1, "large export did not exercise native INCR"
            records.append({"case": name, "widget": kind, "direction": "native-to-raw",
                            "payload_sha256": fingerprint, **stats})
            stop(native)
            owner = launch("own", source)
            assert message(owner)["ready"]
            native = launch("native-import", kind, source)
            imported = message(native)
            assert imported["native_import"] and imported["bytes"] == len(payload)
            incoming = message(owner)
            assert incoming["transfer"] and incoming["bytes"] == len(payload)
            if name == "large":
                assert incoming["incr"] and incoming["chunks"] > 1, "large paste did not exercise INCR"
            # Copy back from the native destination, independently verify the
            # complete transfer rather than accepting a paste admission result.
            received = output / (name + "-" + kind + "-roundtrip.utf8")
            outgoing = request_payload(received)
            assert received.read_bytes() == payload, "native paste/copy round trip changed UTF-8 bytes"
            records.append({"case": name, "widget": kind, "direction": "raw-to-native-to-raw",
                            "payload_sha256": fingerprint, "incoming": incoming,
                            "outgoing": outgoing})
            stop(native)
            stop(owner)
    passed = True
finally:
    for process in children:
        running = process.poll() is None
        if running:
            process.terminate()
        try:
            stdout, stderr = process.communicate(timeout=3)
        except subprocess.TimeoutExpired:
            process.kill(); stdout, stderr = process.communicate()
        records.append({"fixture_argv": process.args, "returncode": process.returncode,
                        "stopped_by_driver": running, "stdout": stdout, "stderr": stderr})
    (output / "receipt.json").write_text(json.dumps({"passed": passed, "started": started,
        "ended": time.time(), "fixture": str(fixture), "records": records}, indent=2) + "\n")
    if passed:
        # Keep hashes/lengths/protocol evidence; discard superseded payload copies.
        for path in output.glob("*.utf8"):
            path.unlink()
if passed:
    print("Unicode native clipboard and bidirectional INCR round trips passed")
