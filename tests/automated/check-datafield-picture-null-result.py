#!/usr/bin/env python3
"""Ensure DataField picture generation checks a NULL result."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text()
start = source.index("\nPictureVerifyCallback(w, client_d, call_d)")
end = source.index("\n/* USE ITERATIONS OF mblen", start)
body = source[start:end]
assert "if (changed == NULL) {" in body
assert body.index("if (changed == NULL) {") < body.index(
    "changed_length = strlen(changed);"
)
print("DataField picture generation checks for a NULL result")
