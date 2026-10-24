#!/usr/bin/env python3
"""Build and run the installed-prefix BreadcrumbBar consumer fixture."""
from pathlib import Path
import os, subprocess, sys, tempfile

root = Path(__file__).resolve().parents[2]
prefix = Path(os.environ.get("MOTIF_TEST_PREFIX", root / "_install"))
source = Path(__file__).resolve().with_name("check-breadcrumb-bar.c")
with tempfile.TemporaryDirectory(prefix="breadcrumb-bar-", dir=os.environ.get("TMPDIR")) as tmp:
    binary = Path(tmp) / "check-breadcrumb-bar"
    cc = os.environ.get("CC", "cc")
    cmd = [cc, "-std=c99", "-Werror", "-I" + str(prefix / "include"), str(source), "-L" + str(prefix / "lib"), "-lXm", "-lXt", "-lX11", "-o", str(binary)]
    subprocess.run(cmd, check=True)
    env = os.environ.copy()
    env["LD_LIBRARY_PATH"] = str(prefix / "lib") + (":" + env["LD_LIBRARY_PATH"] if env.get("LD_LIBRARY_PATH") else "")
    display = env.get("DISPLAY")
    if not display:
        display = ":99"
        xvfb = subprocess.Popen(["Xvfb", display, "-screen", "0", "1024x768x24"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    else:
        xvfb = None
    try:
        env["DISPLAY"] = display
        subprocess.run([str(binary)], check=True, env=env)
    finally:
        if xvfb:
            xvfb.terminate(); xvfb.wait()
