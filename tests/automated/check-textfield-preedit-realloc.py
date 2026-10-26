#!/usr/bin/env python3
"""Check that TextField preedit reallocations are transactional."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

if "char *resized_mb;" not in region or "wchar_t *resized_wc;" not in region:
    raise SystemExit("transactional TextField reallocation temporaries missing")
if region.count("if (resized_mb == NULL)") != 2:
    raise SystemExit("byte preedit reallocations are not both checked")
if region.count("if (resized_wc == NULL)") != 2:
    raise SystemExit("wide preedit reallocations are not both checked")
if "\n            mb = XtRealloc(mb," in region or "\n           wc = (wchar_t *)XtRealloc" in region:
    raise SystemExit("preedit reallocations still overwrite live pointers")

print("TextField preedit reallocations preserve live buffers on failure")
