#!/usr/bin/env python3
"""Check Mrm index collapse bounds the temporary heap before copying."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
for function, limit in (("Idb__INX_CollapseLeafRecord", "IDBIndexLeafFreeMax"),
                        ("Idb__INX_CollapseNodeRecord", "IDBIndexNodeFreeMax")):
    start = source.index("\n" + function + " (")
    end = source.index("\n\n\f", start)
    region = source[start:end]
    check = region.index("stgsiz > " + limit + " - heap_size")
    copy = region.index("UrmBCopy (ndxstg, cur_heap, stglen + 1)")
    assert check < copy, function
    assert "stglen = strnlen" in region
print("Mrm index collapse checks rounded string capacity before copying")
