#!/usr/bin/env python3
"""Ensure Text secondary-resource setup handles table allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nGetSecResData(WidgetClass w_class,") + 1
end = text.index("\n/*********************************************************************/", start)
block = text[start:end]

assert "*secResDataRtn = NULL;" in block
assert "secResDataPtr =" in block
assert "if (secResDataPtr == NULL)" in block
assert "return 0;" in block
assert "_XmTextInputGetSecResData(&secResDataPtr[0]);" in block
assert "_XmTextOutputGetSecResData(&secResDataPtr[1]);" in block

print("text secondary-resource allocation guard: ok")
