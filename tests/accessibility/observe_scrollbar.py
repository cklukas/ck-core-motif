"""Native ScrollBar and provider Value regression through actual AT-SPI D-Bus.
Run under Xvfb and dbus-run-session; requires gdbus, xdotool and AT-SPI services.
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
process = monitor = None
passed = False

def run(args, ok=True):
    result = subprocess.run(args, text=True, capture_output=True, timeout=8)
    records.append({"argv": args, "returncode": result.returncode,
                    "stdout": result.stdout, "stderr": result.stderr})
    if ok and result.returncode:
        raise RuntimeError(result.stderr)
    return result.stdout if ok else result

try:
    discovery = run(["gdbus", "call", "--session", "--dest", "org.a11y.Bus",
                     "--object-path", "/org/a11y/bus", "--method", "org.a11y.Bus.GetAddress"])
    address = re.search(r"\('([^']+)'", discovery).group(1)
    process = subprocess.Popen([str(fixture)], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if not select.select([process.stdout], [], [], 10)[0]:
        raise RuntimeError("registry handshake timed out")
    line = process.stdout.readline()
    if not line:
        raise RuntimeError(process.stderr.read())
    nodes = json.loads(line)
    root = "/org/a11y/atspi/accessible/"
    access = "org.a11y.atspi.Accessible"
    value = "org.a11y.atspi.Value"
    props = "org.freedesktop.DBus.Properties"

    def call(node, method, *args, ok=True):
        return run(["gdbus", "call", "--address", address, "--dest", nodes["bus"],
                    "--object-path", root + ("root" if node == 1 else str(node)),
                    "--method", method, *map(str, args)], ok)

    def current(node):
        out = call(node, props + ".Get", value, "CurrentValue")
        return float(re.search(r"<([^>]+)>", out).group(1))

    def setvalue(node, number, ok=True):
        return call(node, props + ".Set", value, "CurrentValue", "<double " + str(number) + ">", ok=ok)

    bar, horizontal, custom = (nodes[k] for k in ("bar", "horizontal", "custom"))
    registry = run(["gdbus", "call", "--address", address, "--dest", "org.a11y.atspi.Registry",
                    "--object-path", root + "root", "--method", access + ".GetChildren"])
    assert nodes["bus"] in registry
    for node in (bar, horizontal, custom):
        assert "uint32 48" in call(node, access + ".GetRole")
        assert value in call(node, access + ".GetInterfaces")
        assert "MinimumIncrement" in call(node, "org.freedesktop.DBus.Introspectable.Introspect")
    allvalues = call(bar, props + ".GetAll", value)
    assert all(token in allvalues for token in ("MinimumValue", "MaximumValue", "CurrentValue", "MinimumIncrement", "Text", "version"))
    assert "80.0" in call(bar, props + ".Get", value, "MaximumValue")
    assert "3.0" in call(bar, props + ".Get", value, "MinimumIncrement")
    assert "90.0" in call(horizontal, props + ".Get", value, "MaximumValue")
    for node, bit in ((bar, 29), (horizontal, 14)):
        state = call(node, access + ".GetState")
        lower = int(re.search(r"uint32 (\d+)", state).group(1))
        assert lower & (1 << bit)
    assert "increment" in call(bar, "org.a11y.atspi.Action.GetName", 0)
    assert "page-decrement" in call(bar, "org.a11y.atspi.Action.GetName", 3)
    signal_file = receipt.with_suffix(".signals.txt")
    with signal_file.open("w") as signals:
        monitor = subprocess.Popen(["stdbuf", "-oL", "gdbus", "monitor", "--address", address,
                                   "--dest", nodes["bus"], "--object-path", root + str(bar)], stdout=signals, stderr=subprocess.STDOUT)
        time.sleep(.2)
        setvalue(bar, 26)
        assert current(bar) == 26
        assert "value 26" in call(nodes["monitor"], props + ".Get", access, "Name"), "native valueChanged callback was not invoked"
        for number in (-1, 81, 26.5):
            assert setvalue(bar, number, ok=False).returncode != 0
            assert current(bar) == 26
        assert "true" in call(bar, "org.a11y.atspi.Action.DoAction", 0)
        assert current(bar) == 29
        assert "true" in call(bar, "org.a11y.atspi.Action.DoAction", 2)
        assert current(bar) == 49
        assert "true" in call(bar, "org.a11y.atspi.Action.DoAction", 5)
        assert current(bar) == 80
        # Real key events run the native translations (including reverse direction).
        run(["xdotool", "windowfocus", str(nodes["window"])])
        assert "true" in call(bar, "org.a11y.atspi.Component.GrabFocus")
        state = call(bar, access + ".GetState")
        assert int(re.search(r"uint32 (\d+)", state).group(1)) & (1 << 12)
        setvalue(bar, 30)
        run(["xdotool", "key", "Down"])
        assert current(bar) == 33
        assert "true" in call(horizontal, "org.a11y.atspi.Component.GrabFocus")
        run(["xdotool", "key", "Right"])
        assert current(horizontal) == 18, "reverse processingDirection was not respected"
        setvalue(custom, -.375)
        assert current(custom) == -.375
        assert "Scalar units" in call(custom, props + ".Get", value, "Text")
        assert setvalue(custom, 2, ok=False).returncode != 0
        # Actual application XtSetValues updates range and value without notify callbacks.
        call(nodes["control"], "org.a11y.atspi.Action.DoAction", 0)
        assert current(bar) == 40
        assert "100.0" in call(bar, props + ".Get", value, "MaximumValue")
        assert "5.0" in call(bar, props + ".Get", value, "MinimumIncrement")
        call(nodes["control"], "org.a11y.atspi.Action.DoAction", 0)
        assert setvalue(bar, 50, ok=False).returncode != 0
        assert "org.a11y.atspi.Action" not in call(bar, access + ".GetInterfaces")
        time.sleep(.2)
        monitor.terminate(); monitor.wait(timeout=3); monitor = None
    signals = signal_file.read_text()
    assert "accessible-value" in signals and "40.0" in signals
    assert "focused" in signals and "PropertiesChanged" in signals
    assert "'editable', 0" in signals
    call(nodes["control"], "org.a11y.atspi.Action.DoAction", 0)
    stale = call(bar, props + ".Get", value, "CurrentValue", ok=False)
    assert stale.returncode != 0 and "UnknownObject" in stale.stderr
    assert current(custom) == -.375
    call(nodes["control"], "org.a11y.atspi.Action.DoAction", 0)
    stdout, stderr = process.communicate(timeout=15)
    assert process.returncode == 0 and "native-after-destroy-ok" in stdout, stderr
    passed = True
finally:
    for child in (monitor, process):
        if child and child.poll() is None:
            child.terminate()
            try:
                child.wait(timeout=3)
            except subprocess.TimeoutExpired:
                child.kill(); child.wait()
    receipt.write_text(json.dumps({"passed": passed, "started": started, "ended": time.time(),
                                  "fixture": str(fixture), "records": records}, indent=2) + "\n")
