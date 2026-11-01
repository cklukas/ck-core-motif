#!/usr/bin/env python3
"""Check FontSelector font-list counts and allocation sizes stay representable."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text(encoding="utf-8")

process_start = source.index("ProcessXlfdFontData(XmFontSelectorWidget fsw")
process_end = source.index("SortOtherFontData(FontInfo", process_start)
process = source[process_start:process_end]
assert "if (num >= SHRT_MAX)" in process
assert "UINT_MAX / sizeof(*fam)" in process
assert "Cardinal allocation_size" in process
assert "font_info->num_families = (short)num;" in process

other_start = source.index("SortOtherFontData(FontInfo *font_info")
other_end = source.index("CmpStrings(const void *", other_start)
other = source[other_start:other_end]
assert "qsort((void *) list, (size_t)num" in other
assert "font_info->num_others = (short)num;" in other

for signature, element_type in (
    ("AddToXlfdList(XmFontSelectorWidget fsw", "FontData"),
    ("AddToOtherList(char *name", "String"),
):
    start = source.index(signature)
    end = source.index("/*", start + len(signature))
    body = source[start:end]
    assert "*num < 0 || *alloc < 0 || *num >= SHRT_MAX" in body
    assert "UINT_MAX / sizeof(*new_list)" in body
    assert "(Cardinal)(sizeof(*new_list)" in body

fill_start = source.index("FillNewFamily(FamilyInfo *fam")
fill_end = source.index("SizeMapping(short size)", fill_start)
fill = source[fill_start:fill_end]
assert "XtCalloc((Cardinal)fam->encoding_alloc" in fill

print("FontSelector font counts and allocation sizes are bounded")
