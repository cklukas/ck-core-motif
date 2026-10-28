#!/usr/bin/env python3
"""Check drop-transfer callbacks stop after destroying their object."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DropTrans.c").read_text()
start = source.index("(*(dtp->transfer_callback))")
end = source.index("/* The transfer list needs", start)
body = source[start:end]

assert "dt->object.being_destroyed" in body
assert "return;" in body

print("PASS drop-transfer callbacks stop after destruction")
