#!/usr/bin/env python3
"""Check render-table bodies honor Xt's Cardinal allocation limit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
assert source.count("sizeof(_XmRenderTableRec) +") >= 3
assert source.count("> UINT_MAX") >= 6
assert "_XmRTCount(oldtable) > UINT_MAX - (unsigned int)count" in source
assert "size_t)(capacity - RENDITIONS_IN_STRUCT) * sizeof(XmRendition) >" in source
print("PASS render-table Cardinal allocation bounds")
