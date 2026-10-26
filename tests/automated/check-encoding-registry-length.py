#!/usr/bin/env python3
"""Ensure encoding-registry lengths are widened explicitly."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("} else if (atoms[XmA_MOTIF_ENCODING_REGISTRY]")
end = source.index("\n  _XmAppUnlock(app);", start)
body = source[start:end]

assert "int len;" in body
assert "cs -> length = (unsigned long)len;" in body
assert "cs -> length = len;" not in body

print("PASS encoding-registry length widening is explicit")
