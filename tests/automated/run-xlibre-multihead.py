#!/usr/bin/env python3
"""Run multi-head XLibre RandR output-DPI qualification under a 2-CRTC X server."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--display", help="existing >=2-CRTC X display")
    args = parser.parse_args()

    prefix = args.prefix.resolve(strict=True)
    out = args.out.resolve()
    if out.exists():
        parser.error("--out must be a fresh directory")
    out.mkdir(parents=True)

    src_root = Path(__file__).resolve().parents[2]
    fixture = src_root / "tests/automated/check-xlibre-multihead.c"
    binary = out / "check-xlibre-multihead"

    cmd = [
        "cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
        f"-I{prefix / 'include'}",
        f"-I{src_root / 'lib/Xm'}",
        str(fixture),
        f"-L{prefix / 'lib'}",
        f"-Wl,-rpath,{prefix / 'lib'}",
        "-lXm", "-lXt", "-lXrandr", "-lX11", "-lm",
        "-o", str(binary),
    ]
    subprocess.run(cmd, check=True)

    env = dict(
        os.environ,
        LC_ALL="C.UTF-8",
        TMPDIR=str(out),
        LD_LIBRARY_PATH=str(prefix / "lib"),
    )
    for k in ("DISPLAY", "WAYLAND_DISPLAY", "CK_MOTIF_BACKEND", "LD_PRELOAD"):
        env.pop(k, None)

    if args.display:
        env["DISPLAY"] = args.display
        res = subprocess.run([str(binary)], env=env, capture_output=True, text=True, timeout=30)
    else:
        labwc = shutil.which("labwc")
        xwayland = shutil.which("Xwayland")
        if not labwc or not xwayland:
            print("SKIP multi-head RandR qualification requires labwc + Xwayland or --display")
            return 77
        rt = out / "rt"
        cfg = out / "cfg"
        rt.mkdir(mode=0o700)
        cfg.mkdir()
        (cfg / "rc.xml").write_text(
            '<?xml version="1.0"?>\n<openbox_config><core><xwayland>yes</xwayland></core></openbox_config>\n'
        )
        env.update(
            XDG_RUNTIME_DIR=str(rt),
            WLR_BACKENDS="headless",
            WLR_RENDERER="pixman",
            WLR_HEADLESS_OUTPUTS="2",
        )
        startup = f"{binary}; rc=$?; kill -TERM $LABWC_PID 2>/dev/null || true; exit $rc"
        res = subprocess.run(
            [labwc, "-C", str(cfg), "-s", f"sh -c '{startup}'"],
            env=env,
            capture_output=True,
            text=True,
            timeout=30,
        )

    (out / "run.log").write_text(res.stdout + res.stderr)
    if "PASS XLibre multi-head RandR DPI per-CRTC resolution and cross-monitor reflow" not in res.stdout:
        print(res.stdout + res.stderr, file=sys.stderr)
        return res.returncode or 1
    print("PASS XLibre multi-head RandR DPI per-CRTC resolution and cross-monitor reflow")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
