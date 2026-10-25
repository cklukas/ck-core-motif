#!/usr/bin/env python3
"""Check that MWM CCI argument parsing is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = source.rindex("static Boolean ParseWmFuncCCIArgs")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (cciArg == NULL)" in region
assert "cciArg->cciEntry = NULL;" in region
assert "if (cciArg->cciEntry == NULL)" in region
assert "XtFree((char *)cciArg);" in region
print("PASS MWM CCI argument parsing is transactional")
