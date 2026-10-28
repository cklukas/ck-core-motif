#!/usr/bin/env python3
"""Check the 64-bit UIL name hash keeps masks and words unsigned."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymNam.c").read_text()

section = source[source.index("#elif defined (LONG64)") : source.index("#else", source.index("#elif defined (LONG64)"))]
assert "static unsigned long XmConst    mask[ 8 ]" in section
assert "unsigned long       l_hash_code;" in source
assert "unsigned long       al_value[20];" in source
assert "sizeof(unsigned long) * 20" in source
print("UIL 64-bit name hashing uses unsigned long masks and words")
