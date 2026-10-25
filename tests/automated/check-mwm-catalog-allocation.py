#!/usr/bin/env python3
"""Ensure MWM catalog strings use checked allocation widths."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
assert "CopyCatalogString(const char *text)" in text
assert "if (length >= UINT_MAX)" in text
assert "XtMalloc((Cardinal)(length + 1))" in text
assert "XtMalloc ((unsigned int) (strlen(tmpString) + 1))" not in text
print("MWM catalog-string allocations are width-checked")
