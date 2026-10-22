#!/usr/bin/env python3
"""Run inside a real D-Bus session with X display and AT-SPI service installed.

Usage: dbus-run-session -- python3 observe.py /absolute/provider /absolute/receipt.json
Requires gdbus only as an external protocol observer; application links libdbus/Xm.
Failure to embed/register is a test failure, never an in-memory-only acceptance.
"""
import json
import os
import pathlib
import re
import select
import subprocess
import sys
import time

fixture = pathlib.Path(sys.argv[1]).resolve(strict=True)
receipt = pathlib.Path(sys.argv[2]).absolute()
started = time.time()
records = []

def run(args):
    result = subprocess.run(args, text=True, capture_output=True, timeout=8)
    records.append({"argv": args, "returncode": result.returncode,
                    "stdout": result.stdout, "stderr": result.stderr})
    if result.returncode:
        raise RuntimeError(result.stderr)
    return result.stdout

process = None
passed = False
try:
    discovery = run(["gdbus", "call", "--session", "--dest", "org.a11y.Bus",
                     "--object-path", "/org/a11y/bus", "--method", "org.a11y.Bus.GetAddress"])
    address = re.search(r"\('([^']+)'", discovery).group(1)
    process = subprocess.Popen([str(fixture)], stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE, text=True)
    readable, _, _ = select.select([process.stdout], [], [], 10)
    if not readable:
        raise RuntimeError("fixture did not complete registry handshake")
    line = process.stdout.readline()
    if not line:
        raise RuntimeError("fixture failed: " + process.stderr.read())
    nodes = json.loads(line)
    root = "/org/a11y/atspi/accessible/"

    def call(node, method, *args):
        return run(["gdbus", "call", "--address", address, "--dest", nodes["bus"],
                    "--object-path", root + ("root" if node == 1 else str(node)),
                    "--method", method, *map(str, args)])

    registry = run(["gdbus", "call", "--address", address, "--dest", "org.a11y.atspi.Registry",
                    "--object-path", root + "root", "--method", "org.a11y.atspi.Accessible.GetChildren"])
    assert nodes["bus"] in registry, "application missing from actual registry"
    assert "uint32 75" in call(1, "org.a11y.atspi.Accessible.GetRole")
    assert "uint32 43" in call(nodes["button"], "org.a11y.atspi.Accessible.GetRole")
    assert "true" in call(nodes["button"], "org.a11y.atspi.Action.DoAction", 0)
    assert "Activated by accessibility" in call(nodes["button"], "org.freedesktop.DBus.Properties.Get",
                                                "org.a11y.atspi.Accessible", "Name")
    assert "true" in call(nodes["text"], "org.a11y.atspi.Component.GrabFocus")
    assert "Aé😀" in call(nodes["text"], "org.a11y.atspi.Text.GetText", 0, 3)
    assert "é😀" in call(nodes["cell"], "org.a11y.atspi.Text.GetText", 1, 3)
    assert "128512" in call(nodes["cell"], "org.a11y.atspi.Text.GetCharacterAtOffset", 2)
    assert "uint32 55" in call(nodes["table"], "org.a11y.atspi.Accessible.GetRole")
    assert "/" + str(nodes["cell"]) in call(nodes["table"], "org.a11y.atspi.Table.GetAccessibleAt", 0, 0)
    passed = True
finally:
    if process:
        process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()
    receipt.write_text(json.dumps({"passed": passed, "started": started, "ended": time.time(),
                                    "fixture": str(fixture), "records": records}, indent=2) + "\n")
if not passed:
    sys.exit(1)
