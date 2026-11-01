#!/usr/bin/env python3
"""Check that Editres string and widget-ID payload allocations are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(encoding="utf-8")
string_start = source.index("\n_XEditResGetString8(")
widget_start = source.index("\n_XEditResGetWidgetInfo(")
string_region = source[string_start:widget_start]
event_region = source[source.index("case SetValues:"):widget_start]
widget_region = source[widget_start:source.index("#endif", widget_start)]

assert "*str = XtMalloc" in string_region
assert "if ((size_t)len >= (size_t)UINT_MAX)" in string_region
assert "if ((size_t)sv_event->value_len >= (size_t)UINT_MAX)" in event_region
assert "if (*str == NULL)" in string_region
assert "return(FALSE);" in string_region
assert "if (info->ids == NULL && info->num_widgets != 0)" in widget_region
print("PASS Editres payload allocations are guarded")
