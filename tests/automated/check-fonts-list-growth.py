#!/usr/bin/env python3
"""Check FontS raw font-list growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
for marker in ("AddToXlfdList(XmFontSelectorWidget fsw", "AddToOtherList(char *name"):
    start = source.index(marker)
    end = source.index("/*", start + 20)
    body = source[start:end]
    assert "new_list" in body
    assert "if (new_list == NULL)" in body
assert "new_name = XtNewString(name);" in source
assert "if (new_name != NULL)" in source
print("PASS Font selector raw-list growth is transactional")
