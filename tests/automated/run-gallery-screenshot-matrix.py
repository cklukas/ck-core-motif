#!/usr/bin/env python3
"""Run the gallery screenshot qualification for every requested profile/scale."""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import math
import os
from pathlib import Path
import platform
import subprocess
import sys
import time


APPEARANCES = ("classic", "modern-light", "high-contrast", "modern-dark")
RUNNER = Path(__file__).with_name("run-gallery-screenshots.py")


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def parse_appearances(value: str) -> list[str]:
    values = [item.strip() for item in value.split(",") if item.strip()]
    if not values or any(item not in APPEARANCES for item in values):
        raise argparse.ArgumentTypeError(
            "appearances must be a comma-separated subset of "
            + ", ".join(APPEARANCES))
    if len(set(values)) != len(values):
        raise argparse.ArgumentTypeError("appearances must not contain duplicates")
    return values


def parse_scales(value: str) -> list[float]:
    values = []
    try:
        values = [float(item.strip()) for item in value.split(",")
                  if item.strip()]
    except ValueError as error:
        raise argparse.ArgumentTypeError("scales must be numeric") from error
    if not values or any(not math.isfinite(item) or not 0.0 < item <= 64.0
                         for item in values):
        raise argparse.ArgumentTypeError(
            "scales must be finite values in the range (0, 64]")
    if len(set(values)) != len(values):
        raise argparse.ArgumentTypeError("scales must not contain duplicates")
    return values


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--display", default=os.environ.get("DISPLAY"))
    parser.add_argument("--appearances", type=parse_appearances,
                        default=list(APPEARANCES))
    parser.add_argument("--scales", type=parse_scales,
                        default=[1.0, 1.5, 2.0, 3.0])
    parser.add_argument("--discard-captures", action="store_true",
                        help="remove per-entry XWD/PNG files after hashing")
    args = parser.parse_args()
    if platform.system() == "Darwin" and not (
            str(args.out).startswith("/Volumes/S8T/tmp/") or
            str(args.out).startswith("/Volumes/PRO-BLADE/tmp/")):
        parser.error("Mac matrix output must be under an approved temporary volume")
    if not args.binary.is_absolute() or not args.out.is_absolute():
        parser.error("--binary and --out must be absolute")
    if args.out.exists():
        parser.error(f"output already exists: {args.out}")
    args.out.mkdir(parents=True)
    (args.out / "tmp").mkdir()
    env = dict(os.environ, TMPDIR=str(args.out / "tmp"),
               PYTHONDONTWRITEBYTECODE="1", LC_ALL="C")
    receipt = {
        "schema": 1,
        "runner": str(RUNNER),
        "runner_sha256": sha256(RUNNER),
        "binary": str(args.binary),
        "binary_sha256": sha256(args.binary) if args.binary.is_file() else None,
        "display": args.display,
        "appearances": args.appearances,
        "scales": args.scales,
        "retain_captures": not args.discard_captures,
        "scenarios": [],
        "success": False,
        "result": "fail",
        "reason": None,
    }
    started = time.monotonic()
    try:
        for appearance in args.appearances:
            for scale in args.scales:
                scale_text = format(scale, "g")
                name = f"{appearance}-{scale_text.replace('.', '_')}"
                scenario_out = args.out / name
                command = [sys.executable, str(RUNNER), "--binary",
                           str(args.binary), "--out", str(scenario_out),
                           "--appearance", appearance, "--scale", scale_text]
                if args.display:
                    command.extend(("--display", args.display))
                log = args.out / f"{name}.log"
                began = time.monotonic()
                with log.open("w") as stream:
                    process = subprocess.run(command, cwd=RUNNER.parents[2],
                                             env=env, stdout=stream,
                                             stderr=subprocess.STDOUT,
                                             timeout=900)
                child_receipt_path = scenario_out / "receipt.json"
                child_receipt = None
                if child_receipt_path.is_file():
                    child_receipt = json.loads(child_receipt_path.read_text())
                scenario = {
                    "appearance": appearance,
                    "scale": scale,
                    "output": str(scenario_out),
                    "command": command,
                    "log": str(log),
                    "exit": process.returncode,
                    "seconds": round(time.monotonic() - began, 3),
                    "result": child_receipt.get("result")
                    if child_receipt else "fail",
                    "receipt": str(child_receipt_path)
                    if child_receipt else None,
                }
                receipt["scenarios"].append(scenario)
                if process.returncode not in (0, 77):
                    receipt["reason"] = (
                        f"{name}: screenshot runner exited with "
                        f"status {process.returncode}")
                    return 1
                if child_receipt is None:
                    receipt["reason"] = f"{name}: child receipt missing"
                    return 1
                if child_receipt.get("result") == "fail":
                    receipt["reason"] = child_receipt.get("reason") or name
                    return 1
                if child_receipt.get("result") == "skip":
                    receipt["result"] = "skip"
                    receipt["reason"] = child_receipt.get("reason") or name
                    return 77
                if args.discard_captures:
                    for capture in scenario_out.iterdir():
                        if capture.suffix in (".xwd", ".png"):
                            capture.unlink()
        receipt["success"] = True
        receipt["result"] = "pass"
        print(f"gallery screenshot matrix: {len(receipt['scenarios'])} scenarios passed")
        return 0
    except (OSError, subprocess.TimeoutExpired, json.JSONDecodeError) as error:
        receipt["reason"] = str(error)
        print(f"gallery screenshot matrix: {error}", file=sys.stderr)
        return 1
    finally:
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["seconds"] = round(time.monotonic() - started, 3)
        (args.out / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    raise SystemExit(main())
