#!/usr/bin/env python3
"""Check that the legacy DataField paths publish allocations safely."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "memcpy(XtMalloc" not in source
assert "memcpy(\n\t\t\t\t\t\t   XtRealloc" not in source
assert "char *validated_value;" in source
assert "wchar_t *validated_wc_value = (wchar_t *)XtMalloc(" in source
assert source.count("if (restored != NULL)") >= 2

print("DataField allocation checks passed")
