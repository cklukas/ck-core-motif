#!/usr/bin/env python3
"""Ensure Mrm callback-value copying rejects a null destination safely."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_SafeCopyValue (long")
body = text[start:text.index("\n}\n", start)]
assert "if (val == NULL)" in body
assert "if (vec_count < 0 || vec_size < 0" in body
print("Mrm safe-copy null destination is guarded")
