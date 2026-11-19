#!/usr/bin/env python3
"""Run installed RGB and RGBA PNG image-cache regression under Xvfb."""
import argparse
import os
from pathlib import Path
import platform
import struct
import subprocess
import sys
import zlib


def write_png(path: Path, width: int, height: int, color_type: int, rows: list[bytes]) -> None:
    def chunk(tag: bytes, payload: bytes) -> bytes:
        return (
            struct.pack(">I", len(payload))
            + tag
            + payload
            + struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF)
        )

    raw = b"".join(b"\x00" + row for row in rows)
    ihdr = struct.pack(">IIBBBBB", width, height, 8, color_type, 0, 0, 0)
    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b"")
    path.write_bytes(data)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("run in an isolated Linux task workspace")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if not prefix.is_dir() or not output.is_absolute() or output.exists():
        parser.error("--out must be a fresh absolute directory")

    source_root = Path(__file__).resolve().parents[2]
    fixture = source_root / "tests/automated/check-png-image.c"
    output.mkdir(parents=True)

    rgb_png = output / "sample_rgb.png"
    rgba_png = output / "sample_rgba.png"
    # 4x1 RGB: red, green, blue, white
    write_png(rgb_png, 4, 1, 2, [bytes([255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255])])
    # 4x1 RGBA: cyan, transparent, yellow, black
    write_png(
        rgba_png,
        4,
        1,
        6,
        [bytes([0, 255, 255, 255, 0, 0, 0, 0, 255, 255, 0, 255, 0, 0, 0, 255])],
    )

    env = dict(os.environ, LD_LIBRARY_PATH=str(prefix / "lib"), LC_ALL="C.UTF-8", TMPDIR=str(output))
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(name, None)

    binary = output / "check-png-image"
    cmd = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"),
        "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib"),
        str(fixture), "-lXm", "-lXt", "-lX11", "-o", str(binary),
    ]
    compile_log = output / "compile.log"
    with compile_log.open("w") as stream:
        if subprocess.run(cmd, env=env, stdout=stream, stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1

    run_log = output / "run.log"
    with run_log.open("w") as stream:
        res = subprocess.run(
            ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary), str(rgb_png), str(rgba_png)],
            env=env, stdout=stream, stderr=subprocess.STDOUT,
        )
    text = run_log.read_text(errors="replace")
    if res.returncode or "PASS PNG RGB and RGBA image loading" not in text:
        print(text, file=sys.stderr)
        return 1
    print(text, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
