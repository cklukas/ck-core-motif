#!/usr/bin/env python3
"""Check MWM keyboard-state masking uses an unsigned XKB group mask."""

from pathlib import Path


source = Path("clients/mwm/WmGlobal.h").read_text(encoding="utf-8")
start = source.index("#define NOLOCKMOD")
definition = source[start : source.index("\n\n", start)]

assert "(unsigned int)(state)" in definition
assert "~wmGD.lockingModMask" in definition
assert "~((unsigned int)3U << 13)" in definition
assert "~(3 << 13)" not in definition

print("PASS MWM keyboard-state masking uses unsigned XKB group bits")
