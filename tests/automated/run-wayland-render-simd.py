#!/usr/bin/env python3
"""Compile and run the WaylandRender A8 glyph-atlas and SIMD parity regression."""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import time


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()

    if platform.system() == "Darwin":
        print("SKIP: native Wayland runtime backend targets Wayland hosts", file=sys.stderr)
        return 0

    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
        parser.error("--out must be a fresh directory separate from --prefix")

    labwc = shutil.which("labwc")
    if not labwc:
        parser.error("labwc is required for headless Wayland qualification")

    output.mkdir(parents=True)
    (output / "tmp").mkdir()
    rt_dir = output / "rt-wl"
    cfg_dir = output / "labwc-cfg"
    rt_dir.mkdir(mode=0o700)
    cfg_dir.mkdir()
    (cfg_dir / "rc.xml").write_text(
        '<?xml version="1.0"?>\n'
        "<labwc_config><core><xwayland>no</xwayland></core></labwc_config>\n"
    )

    root = Path(__file__).resolve().parents[2]
    source = root / "tests/automated/check-wayland-render-simd.c"
    lib_dir = (prefix / "lib").resolve(strict=True)
    binary = output / "check-wayland-render-simd"

    base_env = dict(
        os.environ,
        TMPDIR=str(output / "tmp"),
        XDG_CACHE_HOME=str(output / "tmp"),
        LC_ALL="C.UTF-8",
        LD_LIBRARY_PATH=str(lib_dir),
    )
    for var in ("DISPLAY", "WAYLAND_DISPLAY", "CK_MOTIF_BACKEND", "CK_MOTIF_WAYLAND_ACTIVE"):
        base_env.pop(var, None)

    cc = os.environ.get("CC", "cc")
    xr_cf = subprocess.check_output(["pkg-config", "--cflags", "xrender"], text=True).split()
    xr_lf = subprocess.check_output(["pkg-config", "--libs", "xrender"], text=True).split()
    compile_cmd = [
        cc, "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"), *xr_cf,
        "-L" + str(lib_dir), "-Wl,-rpath," + str(lib_dir),
        str(source), "-lXm", "-lXt", "-lX11", *xr_lf, "-lm", "-o", str(binary),
    ]
    cres = subprocess.run(compile_cmd, env=base_env, capture_output=True, text=True, timeout=60)
    (output / "compile.log").write_text(cres.stdout + cres.stderr)
    if cres.returncode != 0:
        print(cres.stdout + cres.stderr, file=sys.stderr)
        return 1

    comp_env = dict(
        base_env,
        XDG_RUNTIME_DIR=str(rt_dir),
        WLR_BACKENDS="headless",
        WLR_RENDERER="pixman",
        WLR_HEADLESS_OUTPUTS="1",
    )
    comp_log = (output / "labwc.log").open("w")
    comp_proc = subprocess.Popen([labwc, "-C", str(cfg_dir)], env=comp_env, stdout=comp_log, stderr=subprocess.STDOUT)
    try:
        sock_name = None
        deadline = time.monotonic() + 10.0
        while time.monotonic() < deadline:
            socks = sorted(p.name for p in rt_dir.glob("wayland-*") if not p.name.endswith(".lock"))
            if socks:
                sock_name = socks[0]
                break
            time.sleep(0.04)
        if not sock_name:
            raise RuntimeError("labwc did not publish a wayland-* socket")

        app_env = dict(base_env, XDG_RUNTIME_DIR=str(rt_dir), WAYLAND_DISPLAY=sock_name)
        rres = subprocess.run([str(binary)], env=app_env, capture_output=True, text=True, timeout=30)
        (output / "run.log").write_text(rres.stdout + rres.stderr)
        if rres.returncode != 0 or "PASS WaylandRender A8 glyph-atlas + SIMD" not in rres.stdout:
            print(rres.stdout + rres.stderr, file=sys.stderr)
            return 1
        print(rres.stdout.strip())
        return 0
    finally:
        if comp_proc.poll() is None:
            comp_proc.terminate()
            try:
                comp_proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                comp_proc.kill()
        comp_log.close()


if __name__ == "__main__":
    raise SystemExit(main())
