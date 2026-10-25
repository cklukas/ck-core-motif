#!/usr/bin/env python3
"""Check that UIL locale charset names cannot overflow the stack buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemCSet.c").read_text()
start = source.index("int sem_charset_lang_name")
region = source[start:source.index("\n}\n", start) + 3]
assert "if (lang_charset == NULL)" in region
assert "name_length = strlen(lang_charset);" in region
assert "if (name_length >= sizeof(uname))" in region
assert "memcpy(uname, lang_charset, name_length + 1);" in region
assert "strcpy (uname, lang_charset);" not in region
print("PASS UIL charset names are length-checked before copying")
