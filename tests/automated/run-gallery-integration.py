#!/usr/bin/env python3
"""Run the native gallery smoke test when a display is available.

The default checks navigation and responsive layout. --live exercises profile,
density and DPI transactions across every available component demo without
recreating its instance. Both modes require an explicit display.
"""
from __future__ import annotations

import argparse
import os
import signal
import subprocess
import sys
from pathlib import Path


def layout_failed(stderr: str) -> bool:
    if "Abandoned edge synchronization" in stderr:
        print("FAIL: gallery has contradictory Form constraints", file=sys.stderr)
        return True
    return False


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--live", action="store_true")
    parser.add_argument("--timeout", type=float, default=240,
                        help="live-test watchdog in seconds (default: 240)")
    args = parser.parse_args()
    if not 0 < args.timeout <= 1800:
        parser.error("--timeout must be greater than zero and at most 1800 seconds")
    root = Path(__file__).resolve().parents[2]
    binary = Path(os.environ.get("CKMOTIF_GALLERY", root / "demos/programs/ckmotif-gallery/.libs/ckmotif-gallery"))
    if not binary.exists():
        print(f"skip: gallery binary not found: {binary}")
        return 77
    if not os.environ.get("DISPLAY"):
        print("skip: DISPLAY is not set")
        return 77
    if args.live:
        # The initial full Xft run took 147 seconds for 376 transactions.
        # Keep a bounded watchdog with headroom for the core-font configuration.
        process = subprocess.Popen([str(binary), "--integration-live"],
                                   cwd=root, env=os.environ.copy(),
                                   stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                   text=True, start_new_session=True)
        try:
            stdout, stderr = process.communicate(timeout=args.timeout)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGTERM)
            try:
                stdout, stderr = process.communicate(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                stdout, stderr = process.communicate()
            sys.stdout.write(stdout)
            sys.stderr.write(stderr)
            print("FAIL: live gallery exceeded its watchdog", file=sys.stderr)
            return 124
        sys.stdout.write(stdout)
        sys.stderr.write(stderr)
        return process.returncode or (1 if layout_failed(stderr) else
                                     0 if "PASS gallery live appearance:" in stdout else 1)
    # The suite creates 94 component demos; measured native runs exceed ten
    # seconds on the reference machine. Keep a bounded watchdog and its evidence.
    try:
        result = subprocess.run([str(binary), "--integration-test"],
                                cwd=root, env=os.environ.copy(), timeout=60,
                                capture_output=True, text=True)
    except subprocess.TimeoutExpired as error:
        for stream, output in ((sys.stdout, error.stdout), (sys.stderr, error.stderr)):
            if output:
                stream.write(output.decode(errors="replace") if isinstance(output, bytes) else output)
        print("FAIL: gallery integration exceeded its 60-second watchdog", file=sys.stderr)
        return 124
    sys.stdout.write(result.stdout)
    sys.stderr.write(result.stderr)
    if layout_failed(result.stderr):
        return 1
    if (result.returncode == 0 and
            "navigation tabs interaction state PASS" in result.stdout and
            "PASS gallery sidebar label pixels:" in result.stdout and
            "keyboard traversal and focus restoration PASS" in result.stdout and
            "acceptance metadata" in result.stdout and
            "live catalogue transactions: use --integration-live" in result.stdout):
        checks = [
            (["--integration-responsive", "--appearance", profile,
              "--scale", scale], "PASS gallery responsive layout")
            for profile in ("classic", "modern-light", "modern-dark")
            for scale in ("1", "1.5", "2")
        ]
        checks.append((["--integration-controls"], "PASS gallery appearance controls:"))
        for options, marker in checks:
            try:
                check = subprocess.run([str(binary), *options],
                                       cwd=root, env=os.environ.copy(), timeout=60,
                                       capture_output=True, text=True)
            except subprocess.TimeoutExpired:
                print(f"FAIL: {' '.join(options)} exceeded its watchdog", file=sys.stderr)
                return 124
            sys.stdout.write(check.stdout)
            sys.stderr.write(check.stderr)
            if layout_failed(check.stderr):
                return 1
            if check.returncode or marker not in check.stdout:
                return check.returncode or 1
        return 0
    return result.returncode or 1


if __name__ == "__main__":
    raise SystemExit(main())
