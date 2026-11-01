#!/usr/bin/env python3
"""Compile makestrs and exercise every output ABI as modern C."""
import argparse
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--results", required=True, type=Path)
    parser.add_argument("--cc", default="cc")
    parser.add_argument("--standard", default="c23")
    args = parser.parse_args()
    work = args.results
    if not work.is_absolute():
        parser.error("--results must be an absolute, fresh scratch path")
    work.mkdir(parents=True, exist_ok=False)
    (work / "tmp").mkdir()
    env = dict(os.environ, TMPDIR=str(work / "tmp"))
    root = Path(__file__).resolve().parents[2]
    cc = shlex.split(args.cc) + ["-std=" + args.standard,
                                "-Werror=strict-prototypes",
                                "-Werror=implicit-function-declaration"]
    records = []

    def run(command, directory):
        result = subprocess.run(command, cwd=directory, env=env,
                                capture_output=True, text=True, timeout=30)
        records.append(dict(command=command, exit_code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work / "receipt.json").write_text(json.dumps(records, indent=2) + "\n")
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)
        return result.stdout

    binary = str(work / "makestrs")
    run(cc + [str(root / "config/util/makestrs.c"), "-o", binary], work)
    modes = {"default": [], "array": ["-arrayperabi"],
             "intel": ["-intelabi"], "intel-bc": ["-intelabi", "-earlyR6bc"],
             "sparc": ["-sparcabi"], "function": ["-functionabi"]}
    for name, flags in modes.items():
        directory = work / name
        directory.mkdir()
        template = "#define TestConst const\n<<<STRING_TABLE_GOES_HERE>>>\n"
        (directory / "header.tmpl").write_text(template)
        (directory / "source.tmpl").write_text(template)
        (directory / "strings.list").write_text(
            "#prefix Test\n#const Test\n#feature TEST_STRING_DEFINES\n"
            "#externref extern\n#externdef \n#ctmpl source.tmpl\n"
            "#file strings.h\n#table TestStrings\n#htmpl header.tmpl\n"
            "Nfirst first\nNsecond second\n")
        output = run([binary, *flags, "-f", "strings.list"], directory)
        (directory / "strings.c").write_text(output)
        (directory / "consumer.c").write_text(
            '#include "strings.h"\n#include <string.h>\n'
            'int main(void) { return strcmp(TestNfirst, "first") || '
            'strcmp(TestNsecond, "second"); }\n')
        run(cc + ["strings.c", "consumer.c", "-o", "consumer"], directory)
        run([str(directory / "consumer")], directory)
    print("PASS modern C makestrs and all six generated ABIs")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("SKIP native portability check: requires explicit results and build inputs")
    else:
        main()
