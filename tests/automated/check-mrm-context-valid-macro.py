#!/usr/bin/env python3
"""Check that the shared Mrm context-validation macro is null-safe."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrm.h"
text = source.read_text(encoding="utf-8")
start = text.index("#define\tUrmRCValid(context)")
definition = text[start:text.index("\n\n", start)]
if "NULL" not in definition or "validation" not in definition:
    raise SystemExit("UrmRCValid does not guard null contexts")
if "&&" not in definition:
    raise SystemExit("UrmRCValid does not short-circuit before dereferencing")
print("PASS Mrm context-validation macro is null-safe")
