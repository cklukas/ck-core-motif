#!/usr/bin/env python3
"""Check that the UIL listing's compiler/date title is bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text()
assert "snprintf(lst_c_title1, sizeof(lst_c_title1)" in source
assert "sprintf(lst_c_title1" not in source
print("UIL listing title-one formatting is bounded")
