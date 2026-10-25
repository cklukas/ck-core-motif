#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/RCPopup.c").read_text()
assert "#include <stdint.h>" in source
assert "SIZE_MAX / sizeof(Widget)" in source
assert "UINT_MAX / sizeof(Widget)" in source
assert "(Cardinal)(sizeof(Widget) *" in source
assert "WidgetList new_popups" in source
assert "if (new_popups == NULL)" in source
assert "list->popups = new_popups;" in source
print("RCPopup list growth guards present")
