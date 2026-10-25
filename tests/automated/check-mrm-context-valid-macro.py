#!/usr/bin/env python3
"""Check that the shared Mrm context-validation macro is null-safe."""

import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_MRM_HEADER", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrm.h"))
text = source.read_text(encoding="utf-8")
start = text.index("#define\tUrmRCValid(context)")
line = text[start:text.index("\n\n", start)]
if "NULL" not in line or "validation" not in line:
    raise SystemExit("UrmRCValid does not guard null contexts")
if "&&" not in line:
    raise SystemExit("UrmRCValid does not short-circuit before dereferencing")
print("Mrm context-validation macro is null-safe")
