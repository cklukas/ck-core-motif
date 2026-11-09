#!/usr/bin/env python3
"""Check Mrm FetchSetValues releases its pointer list on index-size failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c").read_text(
    encoding="utf-8"
)
start = source.index("UrmFetchSetValues (MrmHierarchy")
region = source[start:]
guard = "if (sizeof(RGMResourceDesc) > MrmMaxResourceSize ||"
guard_start = region.index(guard)
guard_end = region.index("resptr =", guard_start)
failure_path = region[guard_start:guard_end]

assert "XtFree((char *)locargs);" in failure_path
assert "UrmPlistFree (ptrlist);" in failure_path
assert failure_path.index("XtFree((char *)locargs);") < failure_path.index(
    "UrmPlistFree (ptrlist);"
)
assert "return MrmFAILURE;" in failure_path

print("PASS Mrm FetchSetValues releases the pointer list on index-size failure")
