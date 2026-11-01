#!/usr/bin/env python3
"""Check bounded resource-converter arithmetic keeps native widths."""

from pathlib import Path


source = Path("lib/Xm/ResConvert.c").read_text()

assert "strncpy(tmp, *s, (size_t)csize);" in source
assert "strncpy(table[i], top, (size_t)len);" in source
assert "((Cardinal)crd_array_size, sizeof(Cardinal))" in source
assert "(size_t)sizeof(SystemFontList) *\n\t\t\t\t (size_t)new_maxns_font_lists" in source
assert "(size_t)(new_maxns_font_lists - nsFontLists)" in source
assert "(size_t)(nsFontLists - i)" in source
assert "(sizeof(char *) * (size_t)atom_count)" in source

charset_start = source.rfind("CvtStringToCharSetTable(")
charset = source[charset_start:source.rfind("CvtStringToAtomList(")]
assert "numCharsets > (SIZE_MAX / sizeof(XmStringCharSet)) - 1" not in charset
assert "tableSize = sizeof(XmStringCharSet) * ((size_t)numCharsets + 1);" in charset

print("resource converter arithmetic uses checked native-width values")
