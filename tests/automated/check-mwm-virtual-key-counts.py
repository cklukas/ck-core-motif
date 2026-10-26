#!/usr/bin/env python3
"""Check MWM virtual-key scans use validated, type-compatible counts."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("static str_xref\nGetReplacementList")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "unsigned int key_count;" in region
assert "if (count < 0)" in region
assert "min_kc < 0 || max_kc > UCHAR_MAX" in region
assert "XGetKeyboardMapping( dsp, (KeyCode)min_kc" in region
assert "key_count = (unsigned int)kc_count;" in region
assert "i < (unsigned int)count" in region
assert "j < key_count" in region
assert "j == key_count" in region
assert "i < count" not in region
assert "j < kc_count" not in region
print("PASS MWM virtual-key counts are validated and type-compatible")
