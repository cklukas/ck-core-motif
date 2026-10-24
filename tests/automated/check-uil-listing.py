#!/usr/bin/env python3
"""Run the debug UIL listing path with output longer than its line buffer."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("uil", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
assert args.uil.is_absolute() and args.results.is_absolute()
args.results.mkdir(parents=True, exist_ok=False)
source = args.results / "long-comment.uil"
listing = args.results / "long-comment.lis"
comment = "".join("! long listing comment " * 5 + "\n" for _ in range(200))
source.write_text(
    "module listing_regression\n"
    "version = 'v1.0'\n"
    "names = case_sensitive\n"
    + comment
    + "object button : XmPushButton {\n"
    "  arguments { XmNwidth = 42; XmNlabelString = 'listing'; };\n"
    "};\n"
    "end module;\n"
)
env = dict(os.environ, LC_ALL="C.UTF-8", TMPDIR=str(args.results / "tmp"))
(args.results / "tmp").mkdir()
result = subprocess.run(
    [str(args.uil), "-v", str(listing), "-m", source],
    cwd=args.results,
    env=env,
    capture_output=True,
    timeout=60,
)
(args.results / "uil.log").write_bytes(result.stdout + result.stderr)
assert result.returncode == 0, result.stderr.decode(errors="replace")
assert listing.exists() and listing.stat().st_size > 0
assert b"AddressSanitizer" not in result.stdout + result.stderr
print("PASS debug UIL listing handles long formatted output")
