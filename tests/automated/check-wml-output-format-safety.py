#!/usr/bin/env python3
"""Check WML fixed output templates are never runtime printf formats."""

from pathlib import Path


root = Path(__file__).parents[2] / "tools" / "wml"
key_source = (root / "wmloutkey.c").read_text(
    encoding="utf-8"
)
manual_source = (root / "wmloutmm.c").read_text(encoding="utf-8")

assert "fprintf (outfil, canned" not in key_source
assert key_source.count("fputs (canned_warn, outfil);") == 2
assert key_source.count("fputs (canned1, outfil);") == 2
assert "fputs (canned3, outfil);" in key_source
assert "static const char\tcanned_warn[]" in key_source

assert "fprintf (outfil, canned" not in manual_source
for template in ("canned1", "canned2", "canned3", "canned4"):
    assert f"static const char\t{template}[]" in manual_source
    assert f"fputs ({template}, outfil);" in manual_source

print("PASS WML fixed output templates avoid runtime printf formats")
