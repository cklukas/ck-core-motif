#!/usr/bin/env python3
"""Check byte-swapping bounds IDB index vectors before walking them."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIswap.c").read_text(
    encoding="utf-8"
)
for record, header, entry in (
    ("leaf_rec", "IDBIndexLeafHdr", "IDBIndexLeafEntry"),
    ("node_rec", "IDBIndexNodeHdr", "IDBIndexNodeEntry"),
):
    count = f"{record}->{record.removesuffix('_rec')}_header.index_count"
    swap = f"swapbytes( {count} );"
    guard = f"if ({count} < 0 ||"
    loop = f"for( ndx=0 ; ndx < {count} ; ndx++ )"
    assert swap in source
    assert guard in source
    assert "IDBRecordSize - sizeof(" + header + ")" in source
    assert "sizeof(" + entry + ")" in source
    assert source.index(swap) < source.index(guard) < source.index(loop)

print("Mrm byte-swapping bounds leaf and node index vectors")
