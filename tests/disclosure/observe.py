#!/usr/bin/env python3
"""External protocol observer; run under dbus-run-session with an X display."""
import json
import pathlib
import re
import select
import subprocess
import sys
import time

records = []
process = monitor = None
receipt = {"passed": False, "started": time.time(), "commands": records}
def run(args):
    result = subprocess.run(args, capture_output=True, text=True, timeout=8)
    records.append({"argv": args, "returncode": result.returncode,
                    "stdout": result.stdout, "stderr": result.stderr})
    if result.returncode:
        raise RuntimeError(result.stderr)
    return result.stdout
try:
    address = re.search(r"\('([^']+)'", run([
        "gdbus", "call", "--session", "--dest", "org.a11y.Bus", "--object-path",
        "/org/a11y/bus", "--method", "org.a11y.Bus.GetAddress"])).group(1)
    process = subprocess.Popen([str(pathlib.Path(sys.argv[1]).resolve(strict=True)), "--observe"],
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    assert select.select([process.stdout], [], [], 15)[0], "registry handshake timeout"
    line = process.stdout.readline()
    assert line, process.stderr.read()
    nodes = json.loads(line)
    path = "/org/a11y/atspi/accessible/" + str(nodes["section"])
    def call(method, *args):
        return run(["gdbus", "call", "--address", address, "--dest", nodes["bus"],
                    "--object-path", path, "--method", method, *map(str, args)])
    registry = run(["gdbus", "call", "--address", address, "--dest", "org.a11y.atspi.Registry",
                    "--object-path", "/org/a11y/atspi/accessible/root", "--method",
                    "org.a11y.atspi.Accessible.GetChildren"])
    assert nodes["bus"] in registry
    assert "Section title" in call("org.freedesktop.DBus.Properties.Get",
                                    "org.a11y.atspi.Accessible", "Name")
    def states():
        return int(re.search(r"uint32 (\d+)", call("org.a11y.atspi.Accessible.GetState")).group(1))
    assert states() & (1 << 9) and states() & (1 << 10)
    monitor = subprocess.Popen(["stdbuf", "-oL", "gdbus", "monitor", "--address", address,
                                "--dest", nodes["bus"], "--object-path", path],
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    time.sleep(.2)
    assert "true" in call("org.a11y.atspi.Action.DoAction", 0)
    assert not states() & (1 << 10)
    assert "true" in call("org.a11y.atspi.Component.GrabFocus")
    assert "true" in call("org.a11y.atspi.Action.DoAction", 0)
    assert states() & (1 << 10)
    time.sleep(.2)
    monitor.terminate()
    signals, errors = monitor.communicate(timeout=5)
    receipt["signals"] = signals
    assert "StateChanged ('expanded', 0" in signals
    assert "StateChanged ('expanded', 1" in signals
    receipt["passed"] = True
finally:
    for child in (monitor, process):
        if child and child.poll() is None:
            child.terminate()
            child.communicate(timeout=5)
    receipt["ended"] = time.time()
    pathlib.Path(sys.argv[2]).write_text(json.dumps(receipt, indent=2) + "\n")
print("disclosure AT-SPI registry/state/action/focus/signals passed")
