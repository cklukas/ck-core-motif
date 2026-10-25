#!/usr/bin/env python3
"""Check that Editres string and widget-ID payload allocations are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(encoding="utf-8")
string_start = source.index("_XEditResGetString8(stream, str)")
widget_start = source.index("_XEditResGetWidgetInfo(stream, info)")
string_region = source[string_start:widget_start]
widget_region = source[widget_start:source.index("#endif", widget_start)]

assert "*str = XtMalloc" in string_region
assert "if (*str == NULL)" in string_region
assert "return(FALSE);" in string_region
assert "if (info->ids == NULL && info->num_widgets != 0)" in widget_region
print("PASS Editres payload allocations are guarded")
