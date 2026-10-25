#!/usr/bin/env python3
"""Check that MWM CCI label prefixing is bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = source.index("static Boolean PrefixMenuItemLabel")
end = source.index("\n#endif /* !defined(WSM)", start)
region = source[start:end]
assert "prefixLength > SIZE_MAX - labelLength - 1" in region
assert "prefixLength + labelLength + 1 > UINT_MAX" in region
assert "XtMalloc((Cardinal)(prefixLength + labelLength + 1))" in region
assert "XtMalloc(prefixLength + labelLength + 1)" not in region
assert "if (replacement == NULL)" in region
assert "memcpy(replacement, prefix, prefixLength);" in region
assert "XtFree(menuItem->label);" in region
assert "PrefixMenuItemLabel(menuItem, \"=>\")" in region
assert "sprintf(tmp" not in region
print("PASS MWM CCI label prefixing is bounded")
