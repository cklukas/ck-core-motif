#!/usr/bin/env python3
"""Check segment-encoding registry updates and exports are transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
register = source[source.index("XmRegisterSegmentEncoding"):source.index("_XmGetEncodingRegistryTarget")]
export = source[source.index("_XmGetEncodingRegistryTarget"):source.index("XmCvtCTToXmString")]
assert "String old_copy" in register
assert "String new_encoding" in register
assert "if (new_tag == NULL || new_encoding == NULL)" in register
assert "if (encodingPtr == NULL)" in register
assert "if (rval == NULL)" in export
assert "*length = 0" in export
print("PASS segment-encoding registry allocation is transactional")
