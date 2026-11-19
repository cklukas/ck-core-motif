#!/usr/bin/env python3
"""Compile and run check-auto-atspi-bridge.c under Xvfb + isolated session D-Bus
and AT-SPI2 registry, verifying XM_ACCESSIBILITY=1 automatic VendorShell AT-SPI2
registration, roles, names, text, list items, focus events, and Action.DoAction(0).
"""
from __future__ import annotations

import json
import os
from pathlib import Path
import re
import select
import subprocess
import sys
import time


INNER_RUNNER = r"""
import json
import os
from pathlib import Path
import re
import select
import subprocess
import sys
import time

binary = Path(sys.argv[1]).resolve(strict=True)
prefix_lib = Path(sys.argv[2]).resolve(strict=True)
results = Path(sys.argv[3]).resolve(strict=True)

def gdbus_call(bus_flag, dest, path, method, *args):
    cmd = ["gdbus", "call", *bus_flag, "--dest", dest,
           "--object-path", path, "--method", method, "--", *map(str, args)]
    res = subprocess.run(cmd, text=True, capture_output=True, timeout=8)
    if res.returncode != 0:
        raise RuntimeError(f"gdbus failed ({cmd}): {res.stderr}")
    return res.stdout

# 1. Verify opt-in disabled by default and with NO_AT_BRIDGE=1
env_off = dict(os.environ)
env_off.pop("XM_ACCESSIBILITY", None)
r_off = subprocess.run([str(binary), "--verify-disabled"], env=env_off,
                       text=True, capture_output=True, timeout=8)
assert r_off.returncode == 0 and "disabled_verified" in r_off.stdout, r_off.stderr

env_no_bridge = dict(os.environ, XM_ACCESSIBILITY="1", NO_AT_BRIDGE="1")
r_nb = subprocess.run([str(binary), "--verify-disabled"], env=env_no_bridge,
                      text=True, capture_output=True, timeout=8)
assert r_nb.returncode == 0 and "disabled_verified" in r_nb.stdout, r_nb.stderr

# 2. Discover / launch AT-SPI bus on this isolated session bus
addr_out = gdbus_call(["--session"], "org.a11y.Bus", "/org/a11y/bus",
                      "org.a11y.Bus.GetAddress")
m = re.search(r"\('([^']+)'", addr_out)
assert m, f"Could not parse AT-SPI bus address from {addr_out}"
atspi_addr = m.group(1)
addr_flags = ["--address", atspi_addr]

# Start dbus-monitor for focus StateChanged events on the AT-SPI bus
mon = subprocess.Popen(
    ["dbus-monitor", "--address", atspi_addr, "interface='org.a11y.atspi.Event.Object'"],
    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
)

env_on = dict(os.environ, XM_ACCESSIBILITY="1")
env_on.pop("NO_AT_BRIDGE", None)
proc = subprocess.Popen([str(binary)], env=env_on,
                        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
try:
    rlist, _, _ = select.select([proc.stdout], [], [], 10.0)
    assert rlist, "Timed out waiting for check-auto-atspi-bridge connection JSON"
    line = proc.stdout.readline()
    assert line, f"Fixture exited early: {proc.stderr.read()}"
    meta = json.loads(line)
    pid = int(meta["pid"])
    bus_name = meta["bus"]
    assert bus_name, f"Empty AT-SPI bus name: {meta}"

    # Verify /proc/<pid>/maps links our exact Motif ABI 9 library
    maps_text = Path(f"/proc/{pid}/maps").read_text()
    expected_so = str(prefix_lib / "libXm.so.9")
    assert expected_so in maps_text, f"{expected_so} missing from /proc/{pid}/maps"

    # Verify registration in org.a11y.atspi.Registry
    reg_children = gdbus_call(addr_flags, "org.a11y.atspi.Registry",
                              "/org/a11y/atspi/accessible/root",
                              "org.a11y.atspi.Accessible.GetChildren")
    assert bus_name in reg_children, f"{bus_name} not registered in {reg_children}"

    # Walk tree from /org/a11y/atspi/accessible/root
    visited = {}
    def walk(obj_path):
        if obj_path in visited or obj_path == "/org/a11y/atspi/null":
            return
        role_raw = gdbus_call(addr_flags, bus_name, obj_path,
                              "org.a11y.atspi.Accessible.GetRole")
        role_m = re.search(r"uint32\s+(\d+)", role_raw)
        role = int(role_m.group(1)) if role_m else -1
        name_raw = gdbus_call(addr_flags, bus_name, obj_path,
                              "org.freedesktop.DBus.Properties.Get",
                              "org.a11y.atspi.Accessible", "Name")
        name_m = re.search(r"<'([^']*)'>", name_raw)
        name = name_m.group(1) if name_m else ""
        visited[obj_path] = {"role": role, "name": name}
        ch_raw = gdbus_call(addr_flags, bus_name, obj_path,
                            "org.a11y.atspi.Accessible.GetChildren")
        for _, child_path in re.findall(r"\('([^']*)',\s*(?:objectpath\s+)?'([^']+)'\)", ch_raw):
            walk(child_path)

    walk("/org/a11y/atspi/accessible/root")

    def find_node(role=None, name=None):
        for p, info in visited.items():
            if (role is None or info["role"] == role) and (name is None or info["name"] == name):
                return p
        raise AssertionError(f"Missing node role={role} name={name} in {visited}")

    lbl_path = find_node(role=29, name="Status: Ready")
    btn_path = find_node(role=43, name="Invoke Action")
    tog_path = find_node(role=7, name="Enable Option")
    sec_btn_path = find_node(role=43, name="Secondary Button")
    quit_path = find_node(role=43, name="Quit Fixture")
    list_path = find_node(role=31)
    find_node(role=32, name="Alpha")
    find_node(role=32, name="Beta")
    find_node(role=32, name="Gamma")

    text_paths = [p for p, info in visited.items() if info["role"] == 61]
    assert len(text_paths) >= 2, f"Expected XmTextField + XmText, got {text_paths}"
    texts = {}
    for tp in text_paths:
        txt_out = gdbus_call(addr_flags, bus_name, tp,
                             "org.a11y.atspi.Text.GetText", 0, -1)
        texts[tp] = txt_out
    field_path = next(tp for tp, t in texts.items() if "Hello Field" in t)
    assert any("Multi-line" in t and "Motif text" in t for t in texts.values()), texts

    # GrabFocus on XmTextField and verify StateChanged:focused + state bit 12
    focus_res = gdbus_call(addr_flags, bus_name, field_path,
                           "org.a11y.atspi.Component.GrabFocus")
    assert "true" in focus_res
    st_raw = gdbus_call(addr_flags, bus_name, field_path,
                        "org.a11y.atspi.Accessible.GetState")
    arr_nums = [int(x) for x in re.findall(r"(?<!uint)\b(\d+)\b", st_raw)]
    bits = (arr_nums[0] | (arr_nums[1] << 32)) if len(arr_nums) >= 2 else 0
    assert (bits & (1 << 12)) != 0, f"Expected FOCUSED bit 12 in {st_raw}"

    # Invoke PushButton via AT-SPI Action.DoAction(0)
    act_res = gdbus_call(addr_flags, bus_name, btn_path,
                         "org.a11y.atspi.Action.DoAction", 0)
    assert "true" in act_res
    lbl_after = gdbus_call(addr_flags, bus_name, lbl_path,
                           "org.freedesktop.DBus.Properties.Get",
                           "org.a11y.atspi.Accessible", "Name")
    assert "Status: Button Invoked" in lbl_after, lbl_after

    # Invoke ToggleButton via AT-SPI Action.DoAction(0)
    tog_res = gdbus_call(addr_flags, bus_name, tog_path,
                         "org.a11y.atspi.Action.DoAction", 0)
    assert "true" in tog_res
    lbl_tog = gdbus_call(addr_flags, bus_name, lbl_path,
                         "org.freedesktop.DBus.Properties.Get",
                         "org.a11y.atspi.Accessible", "Name")
    assert "Status: Toggle On" in lbl_tog, lbl_tog

    # Trigger clean shell teardown via Quit Fixture button
    gdbus_call(addr_flags, bus_name, quit_path,
               "org.a11y.atspi.Action.DoAction", 0)
    rc = proc.wait(timeout=8)
    assert rc == 0, f"Clean exit failed with rc={rc}: {proc.stderr.read()}"
    proc = None

    time.sleep(0.15)
    mon.terminate()
    mon_out, _ = mon.communicate(timeout=3)
    mon = None
    assert "StateChanged" in mon_out and "focused" in mon_out, f"Missing focus signal in monitor:\n{mon_out}"

    (results / "receipt.json").write_text(json.dumps({
        "passed": True,
        "bus_name": bus_name,
        "node_count": len(visited),
        "secondary_button": sec_btn_path,
        "list_node": list_path,
    }, indent=2) + "\n")
    print(f"PASS auto AT-SPI2 bridge ({len(visited)} nodes, focus signal + actions verified)")
finally:
    if proc is not None:
        proc.terminate()
        try:
            proc.wait(timeout=3)
        except subprocess.TimeoutExpired:
            proc.kill()
    if mon is not None:
        mon.terminate()
        try:
            mon.wait(timeout=2)
        except subprocess.TimeoutExpired:
            mon.kill()
"""


def main() -> int:
    if len(sys.argv) != 3:
        raise SystemExit("usage: run-auto-atspi-bridge.py PREFIX RESULTS")
    prefix = Path(sys.argv[1]).resolve()
    results = Path(sys.argv[2]).resolve()
    results.mkdir(parents=True, exist_ok=False)
    tmp_dir = results / "tmp"
    rt_dir = results / "rt"
    tmp_dir.mkdir()
    rt_dir.mkdir(mode=0o700)

    source = Path(__file__).with_name("check-auto-atspi-bridge.c")
    binary = results / "check-auto-atspi-bridge"
    cc = os.environ.get("CC", "cc")
    flags = [
        "-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2",
        f"-I{prefix / 'include'}",
        f"-L{prefix / 'lib'}",
        f"-Wl,-rpath,{prefix / 'lib'}",
    ]
    comp = subprocess.run(
        [cc, *flags, str(source), "-lXm", "-lXt", "-lX11", "-o", str(binary)],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, check=False
    )
    (results / "compile.log").write_text(comp.stdout)
    if comp.returncode != 0:
        sys.stderr.write(comp.stdout)
        return comp.returncode

    inner_py = results / "inner_observe.py"
    inner_py.write_text(INNER_RUNNER)

    env = dict(
        os.environ,
        TMPDIR=str(tmp_dir),
        XDG_RUNTIME_DIR=str(rt_dir),
        LC_ALL="C.UTF-8",
        LD_LIBRARY_PATH=str(prefix / "lib"),
    )
    env.pop("WAYLAND_DISPLAY", None)
    run_res = subprocess.run(
        [
            "dbus-run-session", "--",
            "xvfb-run", "-a",
            sys.executable, str(inner_py),
            str(binary), str(prefix / "lib"), str(results),
        ],
        env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, check=False
    )
    (results / "run.log").write_text(run_res.stdout)
    sys.stdout.write(run_res.stdout)
    return run_res.returncode


if __name__ == "__main__":
    raise SystemExit(main())
