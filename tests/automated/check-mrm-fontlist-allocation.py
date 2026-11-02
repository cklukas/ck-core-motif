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

reader = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c").read_text(
    encoding="utf-8"
)
start = reader.index("case MrmRtypeFontList:")
end = reader.index("case MrmRtypeInteger:", start)
region = reader[start:end]
assert "result = Urm__CW_FixupValue" in region
assert "if (result != MrmSUCCESS)" in region
assert "XtFree ((char *)fontlist);" in region
assert region.index("XtFree ((char *)fontlist);") < region.index("XtFree((char *)val);")

print("Mrm legacy font-list allocation failure is handled")
