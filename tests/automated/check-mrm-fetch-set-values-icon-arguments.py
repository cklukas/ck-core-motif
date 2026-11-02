#!/usr/bin/env python3
"""Check Mrm deferred icon arguments use checked dynamic storage."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c").read_text(
    encoding="utf-8"
)
start = source.index("UrmFetchSetValues (MrmHierarchy")
region = source[start:]

assert "pixargs[10]" not in region
assert "_SavePixmapArgPtr\tpixargs" in region
assert "(size_t)num_args > SIZE_MAX / sizeof(*pixargs)" in region
assert "num_args > UINT_MAX / sizeof(*pixargs)" in region
assert "pixargs = (_SavePixmapArgPtr) XtMalloc" in region
assert "&pixargs[pixargs_cnt]" in region
assert "XtFree ((char*)pixargs)" in region

print("PASS Mrm FetchSetValues deferred icon arguments are dynamically bounded")
