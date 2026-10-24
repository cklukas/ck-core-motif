#!/usr/bin/env python3
"""Check that DataField null values use type-correct zero constants."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
assert source.count("XmImRegister((Widget)tf, 0);") == 2
assert "(wchar_t)0" in source
assert "XmImRegister((Widget)tf, (unsigned int) NULL)" not in source
assert "(wchar_t)NULL" not in source
print("DataField null-value conversions are type-correct")
