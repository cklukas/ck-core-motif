#!/usr/bin/env python3
"""Check resource converters reject missing source strings safely."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")

expected = (
    "if (in_str == NULL)\n        return(FALSE);",
    "if (in_str == NULL)\n    return(FALSE);",
    "if (in_str == NULL)\n    return(False);",
)
for guard in expected:
    if guard not in text:
        raise SystemExit(f"converter null-input guard missing: {guard!r}")

print("resource converters reject null source strings")
