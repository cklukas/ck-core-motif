#!/usr/bin/env python3
"""Check UIL machine-listing string and offset width conversions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstMac.c").read_text()

assert "strlen(t_rec->index) >= USHRT_MAX" in source
assert "(int)strlen(t_rec->index)" in source
assert "offset += (int)sizeof(int)" in source
assert "(size_t)(off_info_cnt - i) * sizeof (off_info_type)" in source
assert "access_from_code ((MrmFlag)w_rec->access)" in source
assert "access_from_code ((MrmFlag)UrmRCAccess (az_context))" in source
print("UIL machine-listing string and offset widths are explicit")
