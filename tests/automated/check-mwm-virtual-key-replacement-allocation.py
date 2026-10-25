#!/usr/bin/env python3
"""Check MWM virtual-key replacement growth fails closed."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(encoding="utf-8")
start = source.index("GetReplacementList(")
end = source.index("static Boolean\nGetBindingsProperty", start)
region = source[start:end]
assert "str_xref resized" in region
assert "UINT_MAX / sizeof(str_xref_rec) - 2" in region
assert "XtRealloc((char *)xref_rtn" in region
assert "if (resized == NULL)" in region
assert "XtFree((char *)xref_rtn);" in region
assert "XFree((char *)key_map);" in region
assert "!XDisplayKeycodes(dsp, &min_kc, &max_kc)" in region
assert "(long)max_kc - (long)min_kc + 1 > INT_MAX" in region
assert "ks_per_kc <= 0 || kc_count > INT_MAX / ks_per_kc" in region
assert "xref_rtn[num_xref++]" in region
print("PASS MWM virtual-key replacement growth fails closed")
