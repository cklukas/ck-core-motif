#!/usr/bin/env python3
"""Compile and run the installed DropDown geometry regression on Linux."""
from pathlib import Path
import argparse, hashlib, json, os, platform, signal, subprocess, time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("DropDown negotiation requires Linux/Xvfb")
prefix = args.prefix.resolve(strict=True); output = args.out.resolve()
if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
    parser.error("--out must be fresh and separate from --prefix")
output.mkdir(parents=True); (output / "tmp").mkdir()
source = Path(__file__).with_name("check-dropdown-negotiation.c").resolve(); binary = output / "check-dropdown-negotiation"
env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8", LD_LIBRARY_PATH=str(prefix / "lib"), MALLOC_PERTURB_="165")
env.pop("DISPLAY", None)
library = (prefix / "lib/libXm.so").resolve(strict=True)
receipt = {"library": str(library), "library_sha256": hashlib.sha256(library.read_bytes()).hexdigest(), "source": str(source), "prefix": str(prefix), "success": False}
started = time.monotonic()
def run(name, command):
    log = output / (name + ".log")
    with log.open("w") as stream:
        process = subprocess.Popen(command, cwd=output, env=env, stdout=stream, stderr=subprocess.STDOUT, start_new_session=True)
        try: process.wait(timeout=180)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGTERM); process.wait(timeout=5)
    if process.returncode: print(log.read_text(errors="replace"), end=""); raise SystemExit(process.returncode)
    return log
try:
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g", "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib")]
    run("compile", ["cc", *flags, source, "-lXm", "-lXt", "-lX11", "-o", binary]); run("link", ["ldd", binary]); run("run", ["xvfb-run", "-a", "-s", "-screen 0 1600x1200x24", binary])
    if "PASS DropDown negotiation" not in (output / "run.log").read_text(): raise RuntimeError("regression did not report its pass marker")
    receipt["success"] = True
finally:
    receipt["seconds"] = round(time.monotonic() - started, 3); (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
print("PASS DropDown negotiation")
