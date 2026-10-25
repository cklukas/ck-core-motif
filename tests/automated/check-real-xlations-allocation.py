#!/usr/bin/env python3
"""Check that legacy virtual-key translation setup handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso2_0.c").read_text(encoding="utf-8")
start = source.index("_XmGetRealXlations(")
end = source.index("/************************************************************************\n *\n *  _XmSetEtchedSlider", start)
region = source[start:end]
assert "result = XtMalloc((Cardinal)capacity);" in region
assert "if (result == NULL)" in region
assert "return (NULL);" in region
print("PASS legacy real-translation allocation is guarded")
