#!/usr/bin/env python3
"""Run C23 build/runtime regressions against one configured toolkit build."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shlex
import subprocess
import sys
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", required=True, type=Path)
    parser.add_argument("--results", required=True, type=Path)
    parser.add_argument("--cc", default="cc")
    parser.add_argument("--standard", default="c23")
    args = parser.parse_args()
    if not args.build.is_absolute() or not args.results.is_absolute():
        parser.error("use absolute build and fresh results paths")
    if sys.platform == "darwin" and not any(
            base.parent.is_mount() and args.results.is_relative_to(base)
            for base in [Path("/Volumes/S8T/tmp"), Path("/Volumes/PRO-BLADE/tmp")]):
        parser.error("macOS results must be on a mounted project scratch volume")
    work = args.results
    work.mkdir(parents=True, exist_ok=False)
    (work / "tmp").mkdir()
    root = Path(__file__).resolve().parents[2]
    build = args.build.resolve(strict=True)
    env = dict(os.environ, TMPDIR=str(work / "tmp"), PYTHONDONTWRITEBYTECODE="1")
    directories = [build / "lib/Xm/.libs", build / "lib/Mrm/.libs",
                   build / "clients/uil/.libs"]
    variable = "DYLD_LIBRARY_PATH" if sys.platform == "darwin" else "LD_LIBRARY_PATH"
    env[variable] = os.pathsep.join(map(str, directories))
    extension = ".dylib" if sys.platform == "darwin" else ".so"
    library = (directories[0] / ("libXm" + extension)).resolve(strict=True)
    receipt = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                   platform=platform.platform(), library=str(library),
                   library_sha256=hashlib.sha256(library.read_bytes()).hexdigest(),
                   commands=[], success=False)
    started = time.monotonic()

    def run(command, name, timeout=60):
        result = subprocess.run(command, cwd=work, env=env, capture_output=True,
                                text=True, timeout=timeout)
        receipt["commands"].append(dict(name=name, command=command,
                                       exit_code=result.returncode,
                                       stdout=result.stdout, stderr=result.stderr))
        if result.returncode:
            raise RuntimeError(name + ": " + result.stdout + result.stderr)
        if result.stdout:
            print(result.stdout.strip(), flush=True)

    try:
        for name, script, extra in [
                ("generator", "check-modern-c-generator.py", []),
                ("aliases", "check-symbol-aliases.py", ["--library", str(library)])]:
            run([sys.executable, str(root / "tests/automated" / script),
                 "--cc", args.cc, "--standard", args.standard,
                 "--results", str(work / name), *extra], name)
        packages = subprocess.check_output(
            ["pkg-config", "--cflags", "--libs", "xft", "icu-uc", "icu-i18n",
             "harfbuzz"], env=env, text=True).split()
        cc = shlex.split(args.cc) + ["-std=" + args.standard,
                                    "-Werror=implicit-function-declaration",
                                    "-Werror=return-type"]
        includes = ["-I" + str(p) for p in [build / "lib", build / "include",
                    root / "lib", root / "clients", build / "tools/wml",
                    root / "clients/mwm", work]]
        includes += ["-DHAVE_CONFIG_H", "-include", str(build / "include/config.h")]
        if sys.platform == "darwin":
            includes += ["-I/opt/X11/include", "-L/opt/X11/lib"]
        links = ["-L" + str(p) for p in directories] + ["-lUil", "-lMrm", "-lXm",
                                                       "-lXt", "-lX11", *packages]
        (work / "valid.uil").write_text("module ModernC\nend module;\n")
        source = (root / "clients/mwm/WmResParse.c").read_text()
        masked = re.sub(r'/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                        lambda match: " " * len(match[0]), source, flags=re.S)
        functions = []
        for name in ["StrToOct", "StrToHex", "StrToNum", "ParseWmFuncNbrArg",
                     "ParseWmFuncGrpArg", "ParseKeySym"]:
            match = re.search(r"(?m)^static Boolean " + name + r"\s*\([^;{}]*\)\s*\{", masked)
            if match is None:
                raise RuntimeError("missing Mwm parser implementation: " + name)
            depth, end = 1, match.end()
            while depth:
                depth += (masked[end] == "{") - (masked[end] == "}")
                end += 1
            functions.append(source[match.start():end])
        constants = [re.search(r"(?m)^#define " + name + r"\s+\d+", source)[0]
                     for name in ["MAX_GROUP_STRLEN", "MAX_KEYSYM_STRLEN"]]
        (work / "modern-mwm-functions.h").write_text("\n".join(constants + functions))
        cases = [
            ("abi", "tests/Toolkit/ABI/cohort.c", [], []),
            ("mrm", "tests/automated/check-mrm-modern-c.c", [], []),
            ("mwm", "tests/automated/check-mwm-modern-c.c", [], []),
            ("uil", "tests/automated/check-uil-modern-c.c", [], [str(work / "valid.uil")]),
            ("wml", "tests/automated/check-wml-typed-attributes.c",
             [str(build / "tools/wml/libwml.a")],
             [str(root / "tests/automated/modern-c-attributes.wml")]),
            ("string-mb", "tests/automated/check-xmstring-mb-conversion.c", [], []),
            ("empty-concat", "tests/XmString/empty_concat.c", [], []),
            ("unicode-map", "tests/Toolkit/Unicode/incremental_map.c", [], []),
        ]
        for name, source, objects, arguments in cases:
            binary = str(work / name)
            run(cc + includes + [str(root / source), *objects, *links, "-o", binary],
                name + "-compile")
            run([binary, *arguments], name)
        receipt["success"] = True
    finally:
        receipt.update(ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                       elapsed_seconds=time.monotonic()-started)
        (work / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("SKIP modern C portability: requires explicit build and results paths")
    else:
        main()
