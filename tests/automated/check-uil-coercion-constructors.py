#!/usr/bin/env python3
"""Check UIL semantic coercion constructors handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()

for constructor, variable in (("sem_create_cstr()", "cstr_value"),
                              ("sem_create_wchar_str()", "wcstr_value")):
    assert source.count(constructor) >= 2
    assert source.count(f"if ({variable} == NULL)") >= 2
print("UIL semantic coercion constructors check allocation")
