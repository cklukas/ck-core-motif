#!/usr/bin/env python3
"""Exercise the production Periodic menus on an isolated Xvfb display.

Uses a fake lp: no job is ever sent to a printer. The build must include demos.
"""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import select
import subprocess
import time
os.sys.dont_write_bytecode = True
from native_macos_fonts import font_libraries

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--build", type=Path, required=True)
parser.add_argument("--out", type=Path, required=True)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
build = args.build.resolve(strict=True)
out = args.out.resolve()
mac = platform.system() == "Darwin"
if out.exists():
    parser.error("--out must be fresh")
if mac and not any(base.parent.is_mount() and out.is_relative_to(base)
                   for base in (Path("/Volumes/S8T/tmp"), Path("/Volumes/PRO-BLADE/tmp"))):
    parser.error("select a mounted Mac project temporary volume")
out.mkdir(parents=True)
(out / "tmp").mkdir()
(out / "bin").mkdir()
(out / "app-defaults").mkdir()
(out / "app-defaults/XmdPeriodic").write_bytes((root / "demos/programs/periodic/XmdPeriodic.ad").read_bytes())
(out / "dictionary").write_text("hello\n")
fake_lp = out / "bin/lp"
fake_lp.write_text("#!" + os.sys.executable + "\n" + '''import json, os, pathlib, sys, time
p = pathlib.Path(os.environ["TMPDIR"])
(p / "print-argv.json").write_text(json.dumps(sys.argv))
(p / "printed.txt").write_bytes(pathlib.Path(sys.argv[-1]).read_bytes())
if os.getenv("PERIODIC_TEST_LP_WAIT"): time.sleep(60)
if os.getenv("PERIODIC_TEST_LP_FAIL"):
    print("fake print failure"); sys.exit(1)
print("fake print accepted")
''')
fake_lp.chmod(0o755)
libdirs = [build / "lib/Mrm/.libs", build / "lib/Xm/.libs"]
env = dict(os.environ, TMPDIR=str(out / "tmp"), XDG_CACHE_HOME=str(out / "cache"),
    PATH=str(out / "bin") + ":/opt/X11/bin:" + os.environ["PATH"],
    LC_ALL="en_US.UTF-8" if mac else "C.UTF-8",
    PERIODIC_DICTIONARY=str(out / "dictionary"),
    XFILESEARCHPATH=str(out / "app-defaults/%N"),
    XUSERFILESEARCHPATH=str(out / "missing/%N"))
env["DYLD_LIBRARY_PATH" if mac else "LD_LIBRARY_PATH"] = ":".join(map(str, libdirs))
for key in ("LD_PRELOAD", "LD_AUDIT", "XENVIRONMENT", "XAPPLRESDIR", "UIDPATH"):
    env.pop(key, None)
sources = [root / "tests/automated/check-periodic-menu-actions.c",
    *sorted((root / "demos/programs/periodic").glob("periodic*.[chu]*")),
    *(root / ("lib/Xm/" + name) for name in
      ("AppearanceDraw.c", "Traversal.c", "CutPaste.c", "RCMenu.c", "TextSel.c", "TextFSel.c"))]
digest = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
suffix = "dylib" if mac else "so"
libraries = [libdirs[0] / ("libMrm." + suffix), libdirs[1] / ("libXm." + suffix)]
receipt = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "scope": "production Mrm menus; native XTest on isolated Xvfb; fake printing",
    "sources": {str(p): digest(p) for p in sources},
    "libraries": {str(p.resolve()): digest(p) for p in libraries},
    "uid_sha256": digest(build / "demos/programs/periodic/periodic.uid"),
    "cases": [], "success": False}
server = None
try:
    binary = out / "check-periodic-menu-actions"
    config = (build / "include/config.h").read_text()
    font_flags = subprocess.check_output(["pkg-config", "--cflags", "--libs", "xft", "fontconfig"], env=env, text=True).split() if "#define USE_XFT 1" in config else []
    if mac and font_flags:
        fonts = font_libraries(libraries[1])
        font_flags = [flag for flag in font_flags if flag not in ("-lXft", "-lfontconfig", "-lfreetype")] + fonts
        receipt["font_libraries"] = {path: digest(Path(path)) for path in fonts}
    command = ["cc", "-std=c11", "-DHAVE_CONFIG_H", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter", "-O1",
        "-I" + str(build / "include"), "-I" + str(build / "lib"), "-I" + str(root / "lib"),
        *(["-I/opt/X11/include", "-L/opt/X11/lib"] if mac else []),
        str(sources[0]), str(root / "demos/programs/periodic/periodic_layout.c"),
        str(root / "demos/programs/periodic/periodic_font.c"),
        *("-L" + str(p) for p in libdirs), "-lMrm", "-lXm", "-lXt", "-lX11", "-lXtst", *font_flags, "-o", str(binary)]
    with (out / "compile.log").open("w") as log:
        subprocess.run(command, env=env, stdout=log, stderr=subprocess.STDOUT, check=True)
    read_fd, write_fd = os.pipe()
    server_args = ["Xvfb", "-displayfd", str(write_fd), "-nolock", "-noreset", "-screen", "0", "3840x2160x24"]
    server_args += ["-listen", "tcp", "-nolisten", "unix", "-nolisten", "local"] if mac else ["-nolisten", "tcp"]
    with (out / "xvfb.log").open("w") as log:
        server = subprocess.Popen(server_args, env=env, pass_fds=(write_fd,), stdout=log, stderr=subprocess.STDOUT)
    os.close(write_fd)
    with os.fdopen(read_fd) as ready:
        if not select.select([ready], [], [], 10)[0]:
            raise RuntimeError("Xvfb readiness timeout")
        number = ready.readline().strip()
        if not number.isdigit(): raise RuntimeError("invalid display number")
    env["DISPLAY"] = ("127.0.0.1:" if mac else ":") + number
    for name, options in (("light", []), ("dark", ["--appearance", "modern-dark"]),
                          ("scaled", ["-xrm", "Xft.dpi: 144"]),
                          ("classic", ["--appearance", "classic"])):
        # Each case has independent files and print receipts.
        case_tmp = out / "tmp" / name
        case_tmp.mkdir()
        env["TMPDIR"] = str(case_tmp)
        start = time.monotonic()
        with (out / (name + ".log")).open("w") as log:
            result = subprocess.run([str(binary), *options], env=env,
                cwd=build / "demos/programs/periodic", stdout=log,
                stderr=subprocess.STDOUT, timeout=90)
        receipt["cases"].append({"name": name, "exit": result.returncode,
                                  "seconds": time.monotonic() - start})
        if result.returncode: raise RuntimeError((out / (name + ".log")).read_text()[-4000:])
        argv = json.loads((case_tmp / "print-argv.json").read_text())
        assert argv[1:5] == ["-d", "Test printer ; no shell", "-t", "Motif demo document"]
        assert not list(case_tmp.glob("periodic-print-*"))
        print("PASS", name, flush=True)
    receipt["success"] = True
finally:
    if server:
        server.terminate()
        server.wait(timeout=5)
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
