#!/usr/bin/env python3
"""Check both Mrm reader arglist paths bound descriptor counts."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
needle = ("if (argdesc->count > RGMListSizeMax ||\n"
          "\t    argdesc->extra > RGMListSizeMax)\n"
          "\t  return MrmFAILURE;")
if text.count(needle) != 2:
    raise SystemExit("both reader arglist paths lack count limits")
print("PASS Mrm reader arglist counts are bounded")
