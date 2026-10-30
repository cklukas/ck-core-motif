#!/usr/bin/env python3
"""Check resource-owning label helpers preserve compound XmString segments."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
helpers = {
    "FormSection.c": "static XmString copy_string",
    "SearchBox.c": "static XmString copy_label",
    "IconLabel.c": "static XmString copy_label",
    "Link.c": "static XmString copy_label",
    "EmptyState.c": "static XmString copy_string",
}
for name, marker in helpers.items():
    source = (root / name).read_text()
    start = source.index(marker)
    end = source.index("}", start) + 1
    body = source[start:end]
    assert "XmStringCopy(source)" in body, name
    assert "XmStringUnparse" not in body, name
    assert "XmStringCreateLocalized(text)" not in body, name

print("Compound-string copy helpers preserve label segments")
