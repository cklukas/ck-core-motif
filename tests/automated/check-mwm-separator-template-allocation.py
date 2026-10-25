#!/usr/bin/env python3
"""Check that MWM optional CCI separator templates are allocation-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = source.rindex("MenuItem *MakeSeparatorTemplate")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "XtCalloc(1, sizeof(MenuItem))" in region
assert "if (item == NULL)" in region
assert "if (item->label == NULL)" in region
assert source.count("if (separator != NULL)") >= 2
print("PASS MWM separator-template allocation is guarded")
