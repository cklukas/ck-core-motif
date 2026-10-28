#!/usr/bin/env python3
"""Check UIL machine-listing unload paths bound variable record lengths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstMac.c").read_text()

assert "text_len >= USHRT_MAX" in source
assert "(int)(text_len + 1)" in source
assert "next_offset < off_offset" in source
assert "rec_size < off_offset || rec_size - off_offset > USHRT_MAX" in source
print("UIL machine-listing unload lengths are bounded")
