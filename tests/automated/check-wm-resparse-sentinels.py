#!/usr/bin/env python3
"""Check Mwm parser table sentinels use type-correct zero values."""

from pathlib import Path


source = Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c"
text = source.read_text(encoding="utf-8")
assert "{NULL,      0}," in text
assert text.count("{ NULL, 0, (Boolean(*)())0, 0, (Boolean)0}") == 2
assert "(unsigned int)NULL" not in text
assert "(Boolean(*)())NULL" not in text
print("WmResParse sentinel values are type-correct")
