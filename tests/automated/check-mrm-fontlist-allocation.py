#!/usr/bin/env python3
"""Check Mrm legacy font-list conversion handles allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c"
text = source.read_text(encoding="utf-8")
start = text.index("case MrmRtypeFontList:")
end = text.index("case MrmRtypeInteger:", start)
region = text[start:end]
if "if (fontlist == NULL)" not in region:
    raise SystemExit("Mrm legacy font-list allocation guard is missing")
if "return MrmFAILURE;" not in region[region.index("if (fontlist == NULL)"):]:
    raise SystemExit("Mrm legacy font-list allocation failure is not returned")

print("Mrm legacy font-list allocation failure is handled")
