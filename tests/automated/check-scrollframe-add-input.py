#!/usr/bin/env python3
"""Check ScrollFrame navigator addition validates its pointers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c").read_text()
start = source.index("_XmSFAddNavigator(")
guard = source.index("if ((sf == NULL) || (nav == NULL) || (scroll_frame_data == NULL))", start)
trait = source.index("XmeTraitGet((XtPointer) XtClass(nav)", guard)
assert guard < trait
assert "return;" in source[guard:trait]
print("PASS ScrollFrame navigator addition validates inputs")
