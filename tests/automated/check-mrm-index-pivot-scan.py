#!/usr/bin/env python3
"""Check Mrm split pivot scans stay inside the serialized record."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
for function, capacity in (("Idb__INX_SplitLeafRecord", "IDBIndexLeafFreeMax"),
                           ("Idb__INX_SplitNodeRecord", "IDBIndexNodeFreeMax")):
    start = source.index("\n" + function + " (")
    end = source.index("\n\n\f", start)
    region = source[start:end]
    scan = region.index("strnlen (p_index_stgadr,")
    assert "MIN(IDBMaxIndexLength1," in region[scan:]
    assert f"{capacity} - old_itmvec[p_index].index_stg" in region[scan:]
print("Mrm split pivot scans are bounded by record capacity and name limit")
