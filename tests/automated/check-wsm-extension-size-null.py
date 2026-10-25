#!/usr/bin/env python3
"""Ensure WSM extension sizing matches null-safe string packing."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "WSMStringLength(String string)" in text
assert "WSMStringLength(request->extensions.extension_suggestions[i])" in text
assert "WSMStringLength(reply->extensions.extensions[i])" in text
assert "strlen(request->extensions.extension_suggestions[i])" not in text
assert "strlen(reply->extensions.extensions[i])" not in text
print("WSM extension sizing handles null strings")
