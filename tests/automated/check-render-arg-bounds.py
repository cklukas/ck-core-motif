#!/usr/bin/env python3
"""Check bounds before writing fixed render conversion arrays."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")

if text.count("count >= (int)(sizeof(args) / sizeof(args[0]))") < 8:
    raise SystemExit("render argument-array guards are incomplete")
if text.count("freecount >= (int)(sizeof(freelater) / sizeof(freelater[0]))") < 2:
    raise SystemExit("render saved-string guards are incomplete")

print("render conversion argument arrays are bounded")
