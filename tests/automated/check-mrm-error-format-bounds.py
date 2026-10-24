#!/usr/bin/env python3
"""Check Mrm fixed-size error buffers use bounded formatting."""

from pathlib import Path
import re


error = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmerror.c").read_text(
    encoding="utf-8"
)
start = error.index("Urm__UT_Error (char")
end = error.index("\n}\n", start)
region = error[start:end]
if "snprintf (msg, sizeof(msg)" not in region:
    raise SystemExit("Mrm error formatter is not bounded")
if re.search(r"^\s*sprintf \(msg,", region, re.MULTILINE):
    raise SystemExit("unbounded Mrm error formatting remains")

hier = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c").read_text(
    encoding="utf-8"
)
start = hier.rindex("I18NOpenFile (Display")
end = len(hier)
region = hier[start:end]
if region.count("snprintf (err_stg, sizeof(err_stg)") != 3:
    raise SystemExit("hierarchy filename errors are not all bounded")
if re.search(r"^\s*sprintf \(err_stg,", region, re.MULTILINE):
    raise SystemExit("unbounded hierarchy error formatting remains")

print("Mrm fixed-size error messages use bounded formatting")
