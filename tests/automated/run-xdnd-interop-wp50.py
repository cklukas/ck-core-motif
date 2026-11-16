#!/usr/bin/env python3
"""Run WP-50 XDND v5, UTF8_STRING clipboard, text/uri-list, and image/png interop tests on Xvfb."""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import select
import shutil
import subprocess
import sys
import time


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()

    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
        parser.error("--out must be a fresh directory separate from --prefix")
    if platform.system() == "Darwin" and not any(
        base.parent.is_mount() and output.is_relative_to(base)
        for base in (Path("/Volumes/S8T/tmp"), Path("/Volumes/PRO-BLADE/tmp"))
    ):
        parser.error("Mac results must be beneath a mounted project scratch volume")

    output.mkdir(parents=True)
    (output / "tmp").mkdir()

    root = Path(__file__).resolve().parents[2]
    source = root / "tests/automated/check-xdnd-interop-wp50.c"
    ext = ".dylib" if platform.system() == "Darwin" else ".so"

    if (prefix / "lib" / f"libXm{ext}").is_file():
        lib_dir = (prefix / "lib").resolve(strict=True)
        library = (lib_dir / f"libXm{ext}").resolve(strict=True)
        inc_flags = [
            "-I" + str(prefix / "include"),
            "-I" + str(root / "tests/automated"),
        ]
    else:
        lib_dir = (prefix / "lib/Xm/.libs").resolve(strict=True)
        library = (lib_dir / f"libXm{ext}").resolve(strict=True)
        inc_flags = [
            "-I" + str(prefix / "lib"),
            "-I" + str(root / "lib"),
            "-I" + str(root / "tests/automated"),
        ]

    env = dict(
        os.environ,
        TMPDIR=str(output / "tmp"),
        XDG_CACHE_HOME=str(output / "tmp"),
        PYTHONDONTWRITEBYTECODE="1",
        LC_ALL="en_US.UTF-8" if platform.system() == "Darwin" else "C.UTF-8",
    )
    for var in ("DISPLAY", "XENVIRONMENT", "XAPPLRESDIR", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(var, None)
    lib_var = "DYLD_LIBRARY_PATH" if platform.system() == "Darwin" else "LD_LIBRARY_PATH"
    env[lib_var] = str(lib_dir)

    receipt = {
        "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "library": str(library),
        "library_sha256": sha256(library),
        "source": str(source),
        "source_sha256": sha256(source),
        "steps": [],
        "success": False,
    }
    started = time.monotonic()

    binary = output / "check-xdnd-interop-wp50"
    cc = os.environ.get("CC", "cc")
    cflags = [
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-O2",
        *inc_flags,
        "-L" + str(lib_dir),
    ]
    if platform.system() == "Darwin":
        cflags += ["-I/opt/X11/include", "-L/opt/X11/lib"]
    else:
        cflags += ["-Wl,-rpath," + str(lib_dir)]

    compile_cmd = [cc, *cflags, str(source), "-lXm", "-lXt", "-lX11", "-o", str(binary)]
    compile_res = subprocess.run(
        compile_cmd, env=env, capture_output=True, text=True, timeout=60
    )
    (output / "compile.log").write_text(compile_res.stdout + compile_res.stderr)
    receipt["steps"].append(
        {"name": "compile", "command": compile_cmd, "exit": compile_res.returncode}
    )
    if compile_res.returncode != 0:
        print(compile_res.stdout + compile_res.stderr, file=sys.stderr)
        return 1

    xvfb = shutil.which("Xvfb") or "/opt/X11/bin/Xvfb"
    read_fd, write_fd = os.pipe()
    server = None
    try:
        with (output / "xvfb.log").open("w") as xvfb_log:
            server = subprocess.Popen(
                [
                    xvfb,
                    "-displayfd",
                    str(write_fd),
                    "-screen",
                    "0",
                    "1600x1200x24",
                    "-noreset",
                    "-nolisten",
                    "tcp",
                ],
                env=env,
                pass_fds=(write_fd,),
                stdout=xvfb_log,
                stderr=subprocess.STDOUT,
            )
        os.close(write_fd)
        write_fd = -1
        with os.fdopen(read_fd) as channel:
            read_fd = -1
            if not select.select([channel], [], [], 10)[0]:
                raise RuntimeError("Xvfb did not publish its isolated display")
            disp_num = channel.readline().strip()
            if not disp_num.isdecimal():
                raise RuntimeError(f"Xvfb returned invalid display number: {disp_num!r}")
            env["DISPLAY"] = ":" + disp_num

        run_res = subprocess.run(
            [str(binary)], env=env, capture_output=True, text=True, timeout=60
        )
        (output / "run.log").write_text(run_res.stdout + run_res.stderr)
        receipt["steps"].append(
            {"name": "run", "command": [str(binary)], "exit": run_res.returncode}
        )
        if run_res.returncode != 0:
            print(run_res.stdout + run_res.stderr, file=sys.stderr)
            return 1
        if "PASS WP-50 XDND v5, UTF8_STRING clipboard, text/uri-list, and image/png interop" not in run_res.stdout:
            raise RuntimeError(f"missing PASS marker in output:\n{run_res.stdout}")
        if sha256(library) != receipt["library_sha256"]:
            raise RuntimeError("libXm changed during qualification")

        receipt["success"] = True
        print(run_res.stdout.strip())
        return 0
    finally:
        if write_fd >= 0:
            os.close(write_fd)
        if read_fd >= 0:
            os.close(read_fd)
        if server is not None and server.poll() is None:
            server.terminate()
            try:
                server.wait(timeout=5)
            except subprocess.TimeoutExpired:
                server.kill()
                server.wait()
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["seconds"] = round(time.monotonic() - started, 3)
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    raise SystemExit(main())
