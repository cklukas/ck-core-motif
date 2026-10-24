#!/usr/bin/env python3
"""Check malformed RGB database lines are rejected before use."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c"
text = source.read_text(encoding="utf-8")

needle = "if (sscanf(buf, \"%hu %hu %hu\", &(color_info[count].red),"
if needle not in text:
    raise SystemExit("RGB scan validation is missing")
if ") != 3)" not in text:
    raise SystemExit("RGB scan does not reject malformed lines")

print("color database rejects malformed RGB channel lines")
