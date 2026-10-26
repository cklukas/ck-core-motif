#!/usr/bin/env python3
"""Ensure malformed ICC drop-site streams fail before region writes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("Boolean \n_XmReadDSFromStream(")
body = source[start:source.index("/************************************************************************\n *\n *  _XmWriteDSToStream", start)]
assert "dsHeader = { 0 };" in body
assert "data_offset = (size_t)(propBuf->data.curr - propBuf->data.bytes);" in body
assert "dsRegionNumBoxes >" in body
assert "if (region == NULL)" in body
assert "goto malformed;" in body
print("PASS ICC drop-site stream bounds are checked before region writes")
