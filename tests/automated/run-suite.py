#!/usr/bin/env python3
"""Run every run-*.py test of this tree against an installed toolkit prefix.

    run-suite.py PREFIX OUTDIR [--only NAME ...] [--timeout SECONDS] [--resume]

This is the driver of the continuous-integration job (docs/ci.md) and of the
hardening lane: it runs the headless regression tests of tests/automated one
at a time against a prefix that holds an installed build (make install of
bindings, bitmaps, config, localized, lib, include, tools/wml, clients/xmbind,
clients/uil and clients/mwm), so the tests exercise the installed headers,
libraries and tools and not a build tree.

A script is run when its command line is one of

    run-NAME.py PREFIX RESULTS
    run-NAME.py RESULTS
    run-NAME.py --prefix PREFIX --out RESULTS      (no other required option)

RESULTS is OUTDIR/NAME.  Scripts that need other required inputs (a build tree,
a binary, a display) are listed as skipped; they are not failures, but the
summary names how many there were.  The scripts start their own X server, so
the driver removes DISPLAY from their environment.  This file is not run
itself.

Writes OUTDIR/summary.tsv (name, status, seconds) and one log OUTDIR/NAME.log
per script.  The exit status is 1 when any script that ran did not pass
(status exitN or timeout), 2 for a usage error, else 0.
"""
import argparse
import os
from pathlib import Path
import re
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]


def command_line(script, prefix, out):
    """The argument list for SCRIPT, or None when it needs inputs this driver cannot supply."""
    text = script.read_text()
    adds = re.findall(r"add_argument\(\s*[\"']([^\"']+)[\"']([^)]*)\)", text)
    positional = [a for a, rest in adds if not a.startswith("-") and "nargs" not in rest]
    required = [a for a, rest in adds if a.startswith("-") and "required=True" in rest]
    flags = {a for a, _ in adds}
    usage = re.search(r"usage: run-[a-z0-9-]+\.py ([A-Z ]+)[\"']", text)
    name = script.stem[4:]
    if positional == ["prefix", "results"] and not required:
        return [str(prefix), str(out / name)]
    if positional == ["results"] and not required:
        return [str(out / name)]
    if not positional and {"--prefix", "--out"} <= flags and set(required) <= {"--prefix", "--out"}:
        return ["--prefix", str(prefix), "--out", str(out / name)]
    if not adds and usage and usage.group(1).split() == ["PREFIX", "RESULTS"]:
        return [str(prefix), str(out / name)]
    return None


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("prefix", type=Path, help="installed toolkit prefix")
    ap.add_argument("out", type=Path, help="absolute results directory (summary.tsv and the logs go here)")
    ap.add_argument("--only", nargs="*", metavar="NAME", help="run only these tests (the NAME of run-NAME.py)")
    ap.add_argument("--timeout", type=int, default=900, help="seconds per script (default 900)")
    ap.add_argument("--resume", action="store_true",
                    help="skip scripts that already have a result other than 'skipped' in summary.tsv")
    args = ap.parse_args()
    prefix, out = args.prefix.resolve(), args.out.resolve()
    if not (prefix / "lib").is_dir():
        ap.error(f"{prefix} is not an installed prefix (no lib directory)")
    out.mkdir(parents=True, exist_ok=True)
    summary_path = out / "summary.tsv"
    done = set()
    if args.resume and summary_path.exists():
        done = {line.split("\t")[0] for line in summary_path.read_text().splitlines()
                if line.split("\t")[1] != "skipped"}
    counts = {"pass": 0, "skipped": 0, "failed": 0}
    failed = []
    with open(summary_path, "a") as summary:
        for script in sorted((ROOT / "tests/automated").glob("run-*.py")):
            name = script.stem[4:]
            if script.name == Path(__file__).name or name in done or (args.only and name not in args.only):
                continue
            argv = command_line(script, prefix, out)
            if argv is None:
                summary.write(f"{name}\tskipped\t0\n")
                summary.flush()
                counts["skipped"] += 1
                continue
            env = dict(os.environ, PYTHONDONTWRITEBYTECODE="1")
            env.pop("DISPLAY", None)
            started = time.time()
            try:
                result = subprocess.run([sys.executable, str(script), *argv], cwd=ROOT, env=env,
                                        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
                                        timeout=args.timeout)
                status = "pass" if result.returncode == 0 else f"exit{result.returncode}"
                log = result.stdout
            except subprocess.TimeoutExpired as expired:
                status, log = "timeout", expired.stdout or ""
                if isinstance(log, bytes):
                    log = log.decode(errors="replace")
            (out / f"{name}.log").write_text(log)
            summary.write(f"{name}\t{status}\t{time.time() - started:.1f}\n")
            summary.flush()
            print(f"{status:8} {name}", flush=True)
            if status == "pass":
                counts["pass"] += 1
            else:
                counts["failed"] += 1
                failed.append(name)
    print(f"{counts['pass']} passed, {counts['failed']} failed, {counts['skipped']} skipped "
          f"(they need inputs this driver does not supply)")
    if failed:
        print("failed: " + " ".join(failed))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
