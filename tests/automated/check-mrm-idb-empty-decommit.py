#!/usr/bin/env python3
"""Check empty Mrm IDB buffer pools decommit successfully."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c").read_text(encoding="utf-8")
start = source.index("Idb__BM_DecommitAll (")
end = source.index("\n}\n", start)
region = source[start:end]
empty = region.index("idb__buffer_pool_vec == NULL")
assert "return MrmSUCCESS" in region[empty:]
print("Mrm empty IDB buffer pools decommit successfully")
