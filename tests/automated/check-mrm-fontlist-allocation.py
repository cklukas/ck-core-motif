#!/usr/bin/env python3
"""Check Mrm legacy font-list conversion handles allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c"
text = source.read_text(encoding="utf-8")
start = text.index("case MrmRtypeFontList:")
end = text.index("case MrmRtypeInteger:", start)
region = text[start:end]
if "if (fontlist == NULL)" not in region:
    raise SystemExit("Mrm legacy font-list allocation guard is missing")
if "return MrmFAILURE;" not in region[region.index("if (fontlist == NULL)"):]:
    raise SystemExit("Mrm legacy font-list allocation failure is not returned")

reader = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c").read_text(
    encoding="utf-8"
)
start = reader.index("case MrmRtypeFontList:")
end = reader.index("case MrmRtypeInteger:", start)
region = reader[start:end]
assert "result = Urm__CW_FixupValue" in region
assert "if (result != MrmSUCCESS)" in region
assert "XtFree ((char *)fontlist);" in region
assert region.index("XtFree ((char *)fontlist);") < region.index("XtFree((char *)val);")

def assert_failed_fixup_releases_replacement(region: str) -> int:
    """Keep every old-font-list replacement from leaking on failed fixup."""
    failure = region.index("if (result != MrmSUCCESS)")
    cleanup = region[failure:]
    if "XtFree((char *)fontlist);" not in cleanup:
        raise SystemExit("Mrm failed legacy font-list fixup leaks its replacement")
    return failure


values = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c").read_text(
    encoding="utf-8"
)
values_start = values.index("case MrmRtypeFontList:")
values_end = values.index("\n\tdefault:", values_start)
values_region = values[values_start:values_end]
values_failure = assert_failed_fixup_releases_replacement(values_region)
assert "XtFree((char *)val);" in values_region[values_failure:]
assert "\n\t  break;\n\n\tdefault:" in values[values_start:values_end + 10]

wcrw = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
arglist_start = wcrw.index("Urm__CW_CreateArglist")
arglist_font_start = wcrw.index("case MrmRtypeFontList:", arglist_start)
arglist_font_end = wcrw.index("case MrmRtypeSingleFloat:", arglist_font_start)
arglist_region = wcrw[arglist_font_start:arglist_font_end]
assert "XtMalloc((Cardinal)(sizeof(RGMFontList) +" in arglist_region
assert "(sizeof(RGMFontItem) * (size_t)(count - 1))" in arglist_region
assert_failed_fixup_releases_replacement(arglist_region)

callback_start = wcrw.index("Cardinal \nUrm__CW_FixupCallback")
callback_font_start = wcrw.index("if ((reptype == MrmRtypeFontList)", callback_start)
callback_font_end = wcrw.index("else\n\t\tresult = Urm__CW_FixupValue", callback_font_start)
callback_region = wcrw[callback_font_start:callback_font_end]
assert "XtMalloc((Cardinal)(sizeof(RGMFontList) +" in callback_region
assert "(sizeof(RGMFontItem) * (size_t)(count - 1))" in callback_region
callback_failure = assert_failed_fixup_releases_replacement(callback_region)
assert callback_region.index("XtFree((char *)tag_val);") < callback_failure

print("Mrm legacy font-list replacement failures release temporary storage")
