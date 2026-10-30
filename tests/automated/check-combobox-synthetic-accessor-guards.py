#!/usr/bin/env python3
"""Check ComboBox synthetic import/export callbacks validate inputs."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()
for function in (
    "CBSetSelectedItem",
    "CBGetSelectedItem",
    "CBGetColumns",
    "CBGetItems",
    "CBGetItemCount",
    "CBGetVisibleItemCount",
):
    start = source.index(function + "(")
    end = source.find("\f", start)
    body = source[start:end]
    assert "widget == NULL || value == NULL" in body, function
assert "!cb->composite.num_children || CB_List(cb) == NULL" in source

print("ComboBox synthetic accessors validate widget and value inputs")
