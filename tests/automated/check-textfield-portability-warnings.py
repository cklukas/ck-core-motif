#!/usr/bin/env python3
"""Check TextField input-method null values use type-correct constants."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
assert source.count("XmImRegister((Widget)tf, 0);") == 2
assert "*TextF_WcValue(new_tf) = (wchar_t)0;" in source
assert "XmImRegister((Widget)tf, (unsigned int) NULL)" not in source
assert "(wchar_t)NULL" not in source
print("TextField null-value conversions are type-correct")
