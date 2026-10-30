#!/usr/bin/env python3
"""Check resource-owning label helpers preserve compound XmString segments."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
helpers = {
    "FormSection.c": ("static XmString copy_string", "XmStringCopy(source)"),
    "SearchBox.c": ("static XmString copy_label", "XmStringCopy(source)"),
    "IconLabel.c": ("static XmString copy_label", "XmStringCopy(source)"),
    "Link.c": ("static XmString copy_label", "XmStringCopy(source)"),
    "EmptyState.c": ("static XmString copy_string", "XmStringCopy(source)"),
    "Prompt.c": ("static XmString copy_prompt", "XmStringCopy(source)"),
    "RowActions.c": ("static XmString string_copy", "XmStringCopy(value)"),
    "AutoSuggestBox.c": ("static XmString string_copy", "XmStringCopy(value)"),
}
for name, (marker, copy_call) in helpers.items():
    source = (root / name).read_text()
    start = source.index(marker)
    end = source.index("}", start) + 1
    body = source[start:end]
    assert copy_call in body, name
    assert "XmStringUnparse" not in body, name
    assert "XmStringCreateLocalized(text)" not in body, name

print("Compound-string copy helpers preserve label segments")
