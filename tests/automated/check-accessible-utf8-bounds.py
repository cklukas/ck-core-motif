#!/usr/bin/env python3
"""Check wide-buffer and UTF-8 sizing guards in accessibility conversion."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c"
text = source.read_text(encoding="utf-8")
start = text.index("static char *locale_utf8(")
end = text.index("\nstatic", start + 1)
region = text[start:end]

for needle in (
    "count > SIZE_MAX / sizeof(*wide) - 1",
    "count > ((size_t)((Cardinal)~0) / sizeof(*wide)) - 1",
    "count > (SIZE_MAX - 1) / 4",
    "count > ((size_t)((Cardinal)~0) - 1) / 4",
    "wide = (wchar_t *)XtMalloc((count + 1) * sizeof(*wide));",
    "if (!wide)",
    "result = (char *)XtMalloc(count * 4 + 1);",
    "if (!result)",
    "XtFree((char *)wide);",
):
    if needle not in region:
        raise SystemExit(f"accessibility UTF-8 capacity guard missing: {needle}")

print("accessibility UTF-8 conversion sizing is bounded")
