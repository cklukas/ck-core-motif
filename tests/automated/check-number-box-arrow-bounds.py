#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/NumberBox.c").read_text()
s = "".join(source.split())
assert "width=MIN(number->core.width,SHRT_MAX)" in s
assert "intarrow_width=MIN((int)aw,MAX(0,width/3));" in s
assert "Booleanarrows=arrow_width>0&&field_height>=2;" in s
assert "XtSetMappedWhenManaged(number->number_box.increment,arrows);" in s
print("PASS number-box-arrow-bounds")
