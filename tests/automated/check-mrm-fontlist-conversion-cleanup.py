#!/usr/bin/env python3
"""Check MRM font-list conversion retains every entry and cleans failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = source.index("Cardinal \nUrm__CW_ConvertValue")
end = source.index("\n\f", start)
region = source[start:end]
fontlist_start = region.index("case MrmRtypeFontList:")
fontlist_end = region.index("case MrmRtypeColor:", fontlist_start)
fontlist = region[fontlist_start:fontlist_end]

# A font-set list must accumulate from the prior list.  NULL creates a fresh
# one-entry list and silently discarded every earlier entry.
assert "XmFontListAppendEntry(dfontlist, fontset_entry)" in fontlist
assert "XmFontListAppendEntry(NULL, fontset_entry)" not in fontlist
assert "XmFontListEntryFree(&fontset_entry);" in fontlist
assert "if (fontset_entry == NULL)" in fontlist

# Any failure after a display/font lookup string or a partial font list exists
# releases the owned temporary data before returning an error.
assert "XtFree (dpyandfontstr);" in fontlist
assert fontlist.count("XtFree(dpyandfontstr);") >= 2
assert fontlist.count("XmFontListFree(dfontlist);") >= 4

# The single-font-set conversion follows the same entry ownership contract.
single_start = region.index("case MrmRtypeFont:\n    case MrmRtypeFontSet:")
single_end = region.index("case MrmRtypeFontList:", single_start)
single = region[single_start:single_end]
assert "XmFontListAppendEntry(NULL, fontset_entry)" in single
assert "XmFontListEntryFree(&fontset_entry);" in single
assert "if (fontset_entry == NULL)" in single
assert "if (dfontlist == NULL)" in single

print("MRM font-list conversion preserves entries and cleans temporary failures")
