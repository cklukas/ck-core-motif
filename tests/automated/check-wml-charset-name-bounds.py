#!/usr/bin/env python3
"""Check WML charset-name generation uses an input-sized uppercase copy."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmlouth.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("static void\nwmlOutputUpperCaseCharsetName")
helper_end = source.index("void wmlOutputUilSymCSet", helper_start)
helper = source[helper_start:helper_end]
assert "uppercase_name = wmlAllocateString (name);" in helper
assert "wmlUpperCaseString (uppercase_name);" in helper
assert "free (uppercase_name);" in helper
assert "exit (EXIT_FAILURE);" in helper

body_start = source.index("void wmlOutputUilSymCSet ()")
body = source[body_start:]
assert "uname[200]" not in body
assert "strcpy (uname" not in body
assert body.count("wmlOutputUpperCaseCharsetName (outfil,") == 3

print("PASS WML charset names use input-sized uppercase copies")
