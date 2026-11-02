#!/usr/bin/env python3
"""Check Mrm widget override counts retain their public-width value."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)

for name, successor in (
    ("UrmCreateWidgetInstance", "UrmSetWidgetInstance"),
    ("UrmSetWidgetInstance", None),
):
    start = source.index("\n" + name + " (")
    end = source.index("\n" + successor + " (", start + 1) if successor else len(source)
    region = source[start:end]

    assert "size_t\t\tnum_listent = (size_t)ov_num_args" in region
    assert "MrmCount\t\tnum_listent = ov_num_args" not in region
    assert "Cardinal\t\tov_ndx" in region
    assert "num_listent > UINT_MAX / sizeof(Arg)" in region
    assert "arg_bytes = (Cardinal)(num_listent * sizeof(Arg))" in region
    assert "XtMalloc (arg_bytes)" in region
    assert "for ( ov_ndx=0 ; ov_ndx<ov_num_args ; ov_ndx++ )" in region
    assert "num_listent += (size_t)argdesc->count + (size_t)argdesc->extra" in region

print("PASS Mrm widget override counts use checked public-width arithmetic")
