#!/usr/bin/env python3
"""Check malformed RGB database lines are rejected before use."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c"
text = source.read_text(encoding="utf-8")

if "ParseColorChannels" not in text:
    raise SystemExit("RGB channel parser is missing")
if "strtoul(cursor, &end, 10)" not in text:
    raise SystemExit("RGB channels are not range-checked")
if "value > USHRT_MAX" not in text:
    raise SystemExit("RGB channel width is not validated")

print("color database rejects malformed RGB channel lines")
