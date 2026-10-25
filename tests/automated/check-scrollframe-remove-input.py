#!/usr/bin/env python3
"""Check ScrollFrame navigator removal validates its pointers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c").read_text()
start = source.index("_XmSFRemoveNavigator(")
guard = source.index("if ((scroll_frame_data == NULL) || (nav == NULL))", start)
trait = source.index("XmeTraitGet((XtPointer) XtClass(nav)", guard)
assert guard < trait
assert "return;" in source[guard:trait]
print("PASS ScrollFrame navigator removal validates inputs")
