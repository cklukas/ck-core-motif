#!/usr/bin/env python3
from pathlib import Path
import os, subprocess, json, hashlib, time, select, importlib.util, re
import argparse

parser = argparse.ArgumentParser(
    description="Check actual gallery startup geometry on Xvfb."
)
parser.add_argument("--binary", type=Path, required=True)
parser.add_argument("--prefix", type=Path, required=True)
parser.add_argument("--out", type=Path, required=True)
args = parser.parse_args()
exe = args.binary.resolve(strict=True)
prefix = args.prefix.resolve(strict=True)
out = args.out.resolve()
out.mkdir(exist_ok=False, parents=True)
(out / "tmp").mkdir()
lib = prefix / "lib/libXm.so.9.0.0"
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
for p in Path("/proc").iterdir():
    if p.name.isdigit():
        try:
            name = (p / "comm").read_text().strip()
        except OSError:
            continue
        if name in ("periodic", "ckmotif-gallery"):
            raise RuntimeError("Existing project app " + p.name)
spec = importlib.util.spec_from_file_location(
    "capture", Path(__file__).with_name("run-gallery-screenshots.py")
)
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)
env = dict(
    os.environ,
    TMPDIR=str(out / "tmp"),
    LD_LIBRARY_PATH=str(prefix / "lib"),
    XFILESEARCHPATH=str(out / "%N"),
    XUSERFILESEARCHPATH=str(out / "%N"),
    PYTHONDONTWRITEBYTECODE="1",
)
for k in ("LD_PRELOAD", "LD_AUDIT", "XENVIRONMENT", "XAPPLRESDIR"):
    env.pop(k, None)
a, c = os.pipe()
server = subprocess.Popen(
    [
        "Xvfb",
        "-displayfd",
        str(c),
        "-screen",
        "0",
        "2000x1600x24",
        "-nolisten",
        "tcp",
        "-ac",
        "-noreset",
    ],
    env=env,
    pass_fds=(c,),
    stdout=(out / "xserver.log").open("w"),
    stderr=subprocess.STDOUT,
)
os.close(c)
ready = b""
deadline = time.monotonic() + 15
while not ready.endswith(b"\n"):
    remaining = deadline - time.monotonic()
    assert remaining > 0 and select.select([a], [], [], remaining)[0]
    chunk = os.read(a, 64)
    assert chunk
    ready += chunk
env["DISPLAY"] = ":" + ready.decode().strip()
os.close(a)
app = None
captures = []
try:
    scenarios = [
        ("scale1", ["--scale", "1"], None, (1100, 740)),
        ("scale15", ["--scale", "1.5"], None, (1650, 1110)),
        ("scale2", ["--scale", "2"], None, (1900, 1480)),
        ("scale3", ["--scale", "3"], None, (1900, 1520)),
        ("geometry", ["--scale", "2", "-geometry", "900x650+20+30"], None, (900, 650)),
        ("position", ["--scale", "1.5", "-geometry", "+20+30"], None, (1650, 1110)),
        (
            "resources",
            [
                "--scale",
                "2",
                "-xrm",
                "CkMotifGallery.width: 930",
                "-xrm",
                "CkMotifGallery.height: 670",
            ],
            None,
            (930, 670),
        ),
        ("dpi", ["-xrm", "Xft.dpi: 144"], None, (1650, 1110)),
        ("workarea", ["--scale", "2"], [100, 80, 1000, 800], (950, 760)),
        ("invalid-workarea", ["--scale", "2"], [100, 80, 99999, 800], (1900, 1480)),
    ]
    for name, args, area, expected in scenarios:
        component = "xm.selectionboxwidgetclass"
        subprocess.run(
            ["xprop", "-root", "-remove", "_NET_WORKAREA"],
            env=env,
            check=True,
            capture_output=True,
        )
        if area:
            subprocess.run(
                [
                    "xprop",
                    "-root",
                    "-f",
                    "_NET_WORKAREA",
                    "32c",
                    "-set",
                    "_NET_WORKAREA",
                    ", ".join(map(str, area)),
                ],
                env=env,
                check=True,
                capture_output=True,
            )
        log = (out / (name + ".log")).open("wb")
        app = subprocess.Popen(
            [
                str(exe),
                "--component",
                component,
                "--screenshot-mode",
                "--appearance",
                "modern-light",
                *args,
            ],
            cwd=out,
            env=env,
            stdout=log,
            stderr=log,
        )
        window = m.find_window_id("xwininfo", env, process=app, entry_id=component)
        try:
            m.wait_ready(window, app, env, component)
        except RuntimeError:
            for tool, command in [
                ("tree", ["xwininfo", "-id", window, "-tree"]),
                (
                    "stack",
                    ["gdb", "-batch", "-p", str(app.pid), "-ex", "thread apply all bt"],
                ),
            ]:
                result = subprocess.run(
                    command, env=env, capture_output=True, text=True, timeout=10
                )
                (out / (name + "-" + tool + ".log")).write_text(
                    result.stdout + result.stderr
                )
            raise
        assert Path("/proc/" + str(app.pid) + "/exe").resolve() == exe
        loaded = m.loaded_library(app)
        assert loaded and loaded[1] == sha(lib)
        info = subprocess.run(
            ["xwininfo", "-id", window],
            env=env,
            check=True,
            capture_output=True,
            text=True,
        ).stdout
        actual = tuple(
            int(re.search(r"\b" + axis + r":\s+(\d+)", info).group(1))
            for axis in ("Width", "Height")
        )
        assert actual == expected, (name, actual, expected)
        if name in ("geometry", "position"):
            position = tuple(
                int(
                    re.search(
                        r"Absolute upper-left " + axis + r":\s+(-?\d+)", info
                    ).group(1)
                )
                for axis in ("X", "Y")
            )
            assert position == (20, 30), (name, position)
        print("PASS geometry", name, actual, flush=True)
        xwd = subprocess.run(
            ["xwd", "-silent", "-id", window], env=env, check=True, capture_output=True
        ).stdout
        pnm = subprocess.run(
            ["xwdtopnm"], input=xwd, check=True, capture_output=True
        ).stdout
        png = subprocess.run(
            ["pnmtopng"], input=pnm, check=True, capture_output=True
        ).stdout
        (out / (name + ".png")).write_bytes(png)
        captures.append(
            {
                "name": name,
                "dimensions": actual,
                "pid": app.pid,
                "window": window,
                "executable": str(exe),
                "executable_sha256": sha(exe),
                "library": loaded[0],
                "library_sha256": loaded[1],
                "screenshot_sha256": hashlib.sha256(png).hexdigest(),
            }
        )
        app.terminate()
        app.wait(timeout=5)
        app = None
        log.close()
finally:
    if app and app.poll() is None:
        app.terminate()
        app.wait(timeout=5)
    server.terminate()
    server.wait(timeout=5)
    (out / "receipt.json").write_text(json.dumps(captures, indent=2))
print("PASS gallery startup geometry:", len(captures), "verified native windows")
