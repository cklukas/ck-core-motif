#!/usr/bin/env python3
"""Check MWM preprocessor command construction is bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nPreprocessConfigFile")
body = source[start : source.index("\n}\n\n", start)]
assert "snprintf(NULL, 0, \"%s %s %s\"" in body
assert "commandLength >= (int)sizeof(pchCmd)" in body
assert "snprintf(pchCmd, sizeof(pchCmd), \"%s %s %s\"" in body
assert "strcpy (pchCmd" not in body
assert "strcat (pchCmd" not in body
assert "XtFree(pConfigStackTop->cppName);" in body

print("MWM preprocessor command construction is bounded")
