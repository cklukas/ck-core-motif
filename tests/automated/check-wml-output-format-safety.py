#!/usr/bin/env python3
"""Check WML fixed output templates are never runtime printf formats."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmloutkey.c").read_text(
    encoding="utf-8"
)

assert "fprintf (outfil, canned" not in source
assert source.count("fputs (canned_warn, outfil);") == 2
assert source.count("fputs (canned1, outfil);") == 2
assert "fputs (canned3, outfil);" in source
assert "static const char\tcanned_warn[]" in source

print("PASS WML fixed output templates avoid runtime printf formats")
