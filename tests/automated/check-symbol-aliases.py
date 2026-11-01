#!/usr/bin/env python3
"""Verify function/data aliases in shared libraries and static archives."""
import argparse
import ctypes
import json
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--results", required=True, type=Path)
    parser.add_argument("--library", required=True, type=Path)
    parser.add_argument("--cc", default="cc")
    parser.add_argument("--standard", default="c23")
    args = parser.parse_args()
    if not args.results.is_absolute() or not args.library.is_absolute():
        parser.error("use absolute results and library paths")
    work = args.results
    work.mkdir(parents=True, exist_ok=False)
    (work / "tmp").mkdir()
    env = dict(os.environ, TMPDIR=str(work / "tmp"))
    root = Path(__file__).resolve().parents[2]
    cc = shlex.split(args.cc) + ["-std=" + args.standard]
    records = []

    def run(command):
        result = subprocess.run(command, cwd=work, env=env, capture_output=True,
                                text=True, timeout=30)
        records.append(dict(command=command, exit_code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work / "commands.json").write_text(json.dumps(records, indent=2) + "\n")
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)

    (work / "aliases.c").write_text(
        '#include "SymbolAliasI.h"\n'
        'struct Record { int value; };\n'
        'struct Record original_record = {23};\n'
        'int original_function(int n) { return n + 23; }\n'
        '_XmDefineAlias(alias_record, original_record);\n'
        '_XmDefineAlias(alias_function, original_function);\n')
    (work / "consumer.c").write_text(
        'struct Record { int value; };\n'
        'extern struct Record alias_record, original_record;\n'
        'extern int alias_function(int), original_function(int);\n'
        'int main(void) {\n'
        ' struct Record *volatile a = &alias_record, *volatile b = &original_record;\n'
        ' int (*volatile f)(int) = alias_function;\n'
        ' int (*volatile g)(int) = original_function;\n'
        ' if (a != b || f != g || f(1) != 24) return 1;\n'
        ' a->value = 42; return b->value != 42;\n}\n')
    run(cc + ["-I" + str(root / "lib/Xm"), "-c", "aliases.c", "-o", "aliases.o"])
    run(["ar", "crs", "aliases.a", "aliases.o"])
    run(cc + ["consumer.c", "aliases.a", "-o", "consumer"])
    run([str(work / "consumer")])
    extension = ".dylib" if sys.platform == "darwin" else ".so"
    run(cc + ["-I" + str(root / "lib/Xm"), "-fPIC", "-shared", "aliases.c",
              "-o", "aliases" + extension])
    probe = ctypes.CDLL(str(work / ("aliases" + extension)))
    library = ctypes.CDLL(str(args.library))
    pairs = [("alias_record", "original_record"),
             ("alias_function", "original_function")]
    receipt = []
    for handle, names in [(probe, pairs), (library, [
            pair for name in ["DropDown.c", "MultiList.c"]
            for pair in re.findall(r"_XmDefineAlias\((\w+), (\w+)\)",
                                   (root / "lib/Xm" / name).read_text())])]:
        for alias, target in names:
            a = ctypes.cast(getattr(handle, alias), ctypes.c_void_p).value
            b = ctypes.cast(getattr(handle, target), ctypes.c_void_p).value
            if a != b:
                raise AssertionError(f"{alias} does not share {target}'s address")
            receipt.append(dict(alias=alias, target=target, same_address=True))
    if len(receipt) != 30:
        raise AssertionError("expected two fixture aliases and 28 toolkit aliases")
    (work / "aliases.json").write_text(json.dumps(receipt, indent=2) + "\n")
    print("PASS static archive, shared function/data aliases and all 28 toolkit aliases")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("SKIP native portability check: requires explicit results and build inputs")
    else:
        main()
