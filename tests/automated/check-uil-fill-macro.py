#!/usr/bin/env python3
"""Check the UIL fill helper uses a private size-safe counter."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDefI.h").read_text()

assert "size_t __uil_fill_i = (size_t)(__len);" in source
assert "while (__uil_fill_i != 0)" in source
assert "(__string)[--__uil_fill_i] = __char;" in source
print("UIL fill macro avoids local shadowing and signed length tests")
