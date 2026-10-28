#!/usr/bin/env python3
"""Check TextField selection insertion conversion results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("static void\nInsertSelection(")
end = source.index("/* ARGSUSED */\nstatic void\nHandleInsertTargets", start)
body = source[start:end]

assert "size_t converted;" in body
assert body.count("converted = mbstowcs") == 2
assert body.count("converted == (size_t)-1") == 2
assert body.count("replace_res = False") >= 2
assert "(int)mbstowcs" not in body
assert "num_chars < 0" not in body

print("PASS TextField selection insertion conversion guards")
