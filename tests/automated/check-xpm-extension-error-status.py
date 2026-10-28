#!/usr/bin/env python3
"""Check XPM extension metadata failures use the invalid-input status."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
data_source = (root / "XpmCrDatFrI.c").read_text()
data_call = data_source[data_source.index("if (extensions)\n\tif (CountExtensions") :
                       data_source.index("/*\n     * alloc", data_source.index("if (extensions)\n\tif (CountExtensions"))]
assert "return(XpmFileInvalid);" in data_call

buffer_source = (root / "XpmCrBufFrI.c").read_text()
buffer_call = buffer_source[buffer_source.index("if (extensions)\n\text_size") :
                             buffer_source.index("if (cmts)", buffer_source.index("if (extensions)\n\text_size"))]
assert "RETURN(XpmFileInvalid);" in buffer_call
print("PASS XPM extension error status")
