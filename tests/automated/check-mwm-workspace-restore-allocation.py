#!/usr/bin/env python3
"""Check that MWM workspace restore strings are assigned transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(encoding="utf-8")
assert source.count("char *workspaceNames = XtNewString(resourcePtr);") == 2
assert source.count("if (workspaceNames != NULL)") >= 2
assert "*workSpaceNamesP = XtNewString(resourcePtr);" not in source
print("PASS MWM workspace restore allocation is guarded")
