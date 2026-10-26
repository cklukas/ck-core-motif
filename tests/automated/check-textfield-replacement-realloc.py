#!/usr/bin/env python3
"""Check TextField replacement growth preserves live buffers on failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
for marker in ("_XmTextFieldReplaceText(", "_XmTextFieldReplaceTextForPreedit("):
    start = text.index(marker)
    end = text.index("\n}\n", start)
    region = text[start:end]
    if "new_value = (char *)XtRealloc" not in region:
        raise SystemExit(f"missing checked byte replacement growth: {marker}")
    if "new_wc_value = (wchar_t *)XtRealloc" not in region:
        raise SystemExit(f"missing checked wide replacement growth: {marker}")
    if "if (new_value == NULL)" not in region or "if (new_wc_value == NULL)" not in region:
        raise SystemExit(f"replacement growth lacks failure checks: {marker}")

print("TextField replacement reallocations preserve live buffers")
