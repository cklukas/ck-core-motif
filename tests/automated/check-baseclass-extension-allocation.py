#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/BaseClass.c").read_text()
assert source.count("if (!*wcePtr)") >= 2
assert source.count("if (!secResData)") >= 1
assert source.count("if (!sd)") >= 1
assert source.count("if (!table)") >= 1
assert source.count("if (!tmp_resources)") >= 1
assert source.count("if (!newData)") >= 1
print("BaseClass extension allocation guards present")
