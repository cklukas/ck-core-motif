#!/usr/bin/env python3
"""Guard Label wide-string conversion failure paths."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text(
    encoding="utf-8"
)
start = text.index("LabelSetValue(", 10000)
end = text.index("\n}\n", start) + 2
body = text[start:end]
for needle in (
    "if (str == NULL)",
    "wcstombs(str, str2, byte_length) == (size_t)-1",
    "XtFree(str);",
):
    if needle not in body:
        raise SystemExit(f"Label wide conversion guard missing: {needle}")

print("Label wide-string conversion checks passed")
