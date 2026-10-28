#!/usr/bin/env python3
"""Keep empty wide callback results NULL-safe before conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text()
needle = """     { /* caller expects char */
\t\t if (wcs_vcb.text->wcsptr != NULL)
\t\t     wcs_vcb.text->wcsptr[wcs_vcb.text->length] = 0L;
\t\t if (*insert_length > 0) 
"""
assert needle in source
assert source.index("wcs_vcb.text->wcsptr != NULL") < source.index(
    "count = wcstombs(*insert, wcs_vcb.text->wcsptr"
)
print("DataField does not dereference NULL for empty wide callback results")
