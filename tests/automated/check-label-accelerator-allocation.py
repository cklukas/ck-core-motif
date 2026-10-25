#!/usr/bin/env python3
"""Check that Label accelerator copies and queries fail safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text()

set_start = source.index("SetValues(Widget cw,")
copy_start = source.index("  if (Lab_IsMenupane(new_w)", set_start)
copy_end = source.index("  menuSTrait =", copy_start)
copy_region = source[copy_start:copy_end]

assert "String accelerator;" in copy_region
assert "accelerator = XtNewString(newlp->accelerator);" in copy_region
assert "if (accelerator == NULL)" in copy_region
assert "newlp->accelerator = curlp->accelerator;" in copy_region
assert copy_region.index("if (accelerator == NULL)") < copy_region.index("XtFree(curlp->accelerator)")

get_start = source.index("\nGetAccelerator(Widget wid,") + 1
get_end = source.index("GetAcceleratorText", get_start)
get_region = source[get_start:get_end]
assert "if (Lab_Accelerator(lw) == NULL)" in get_region
assert get_region.index("if (Lab_Accelerator(lw) == NULL)") < get_region.index("string = XtNewString")
assert "*value = (XtArgVal) NULL;" in get_region

print("PASS Label accelerator allocation and absent-value handling")
