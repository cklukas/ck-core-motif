#!/usr/bin/env python3
"""Check remaining label/font SetValues replacements are transactional."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
contracts = {
    "BreadcrumbBar.c": ("bar->breadcrumb.font", "old->breadcrumb.font"),
    "Divider.c": ("divider->divider.font", "old->divider.font"),
    "DropHighlight.c": ("highlight->drop_highlight.font", "old->drop_highlight.font"),
}

for filename, (current, old) in contracts.items():
    source = (root / filename).read_text(encoding="utf-8")
    start = source.index("static Boolean SetValues(")
    start = source.index("static Boolean SetValues(", start + 1)
    end = source.find("\nstatic ", start + 1)
    region = source[start:] if end < 0 else source[start:end]
    if "if (replacement)" not in region:
        raise SystemExit(f"{filename} does not validate replacement allocation")
    if f"{current} = {old};" not in region:
        raise SystemExit(f"{filename} does not preserve the old resource")

print("PASS remaining SetValues font/label replacements are transactional")
