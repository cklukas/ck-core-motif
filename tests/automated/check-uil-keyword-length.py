#!/usr/bin/env python3
"""Check UIL keyword lookup validates the signed configured length explicitly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilKeyTab.c").read_text()

assert "key_k_keyword_max_length < 0" in source
assert "symbol_length > (unsigned int)key_k_keyword_max_length" in source
print("UIL keyword lookup handles configured length width explicitly")
