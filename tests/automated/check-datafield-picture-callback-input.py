#!/usr/bin/env python3
"""Ensure DataField picture verification rejects malformed callback input."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text()
start = source.index("\nPictureVerifyCallback(w, client_d, call_d)")
end = source.index("\n/* USE ITERATIONS OF mblen", start)
body = source[start:end]

assert "if (w == NULL || call_d == NULL)" in body
assert "if (cbs->text == NULL)" in body
assert body.index("if (cbs->text == NULL)") < body.index("cbs->startPos")
print("DataField picture verification rejects malformed callback input")
