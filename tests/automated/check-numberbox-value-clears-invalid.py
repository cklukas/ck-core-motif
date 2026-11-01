#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/NumberBox.c").read_text()
s = "".join(source.split())
body=s[s.index("staticBooleanSetValues(Widgetold_widget"):s.index("staticXtGeometryResultQueryGeometry(Widgetwidget")]
assert "old->number_box.value!=number->number_box.value" in body
assert body.index("number->number_box.invalid=False;") < body.index("format_value(number)")
body=s[s.index("voidXmNumberBoxSetRange"):s.index("voidXmNumberBoxSetValue")]
assert body.index("number->number_box.invalid=False;") < body.index("format_value(number)")
print("PASS numberbox-value-clears-invalid")
