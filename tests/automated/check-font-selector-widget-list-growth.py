#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/FontS.c").read_text()
for name in ("AddToXlfdOnlyList", "AddToXlfdSensitiveList"):
    start = source.index("static void\n" + name + "(")
    end = source.index("\n}\n", start) + 3
    region = source[start:end]
    assert "num < 0 || alloc < 0 || num >= alloc" in region
    assert "alloc > SCHAR_MAX - WIDGET_LIST_INC" in region
    assert "SIZE_MAX / sizeof(Widget)" in region
    assert "UINT_MAX / sizeof(Widget)" in region
    assert "WidgetList new_list" in region
    assert "if (new_list == NULL)" in region
    assert region.index("if (new_list == NULL)") < region.index("= new_list")
    assert "if (num >= SCHAR_MAX)" in region
print("FontSelector widget-list growth guards present")
