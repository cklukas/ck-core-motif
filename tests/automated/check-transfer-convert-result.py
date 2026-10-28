#!/usr/bin/env python3
"""Check conversion results reject typeless non-empty payloads."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Transfer.c").read_text()
start = source.index("if (cbstruct.status == XmCONVERT_DONE")
end = source.index("/* Copy out the data */", start)
body = source[start:end]

assert "cbstruct.length != 0 && cbstruct.type == None" in body
assert "cbstruct.length != 0 && cbstruct.value == NULL" in body
assert "*type = None;" in body

print("PASS conversion rejects non-empty typeless results")
