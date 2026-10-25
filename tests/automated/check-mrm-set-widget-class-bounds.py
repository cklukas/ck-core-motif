#!/usr/bin/env python3
"""Check the Mrm auto-child path bounds its serialized class string."""
import os
from pathlib import Path
source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
start = text.index("\nUrmSetWidgetInstance (")
end = text.find("\n\n\f", start)
region = text[start:end]
needle = "!UrmWidgetStringValid(widgetrec, widgetrec->class_offs)"
if needle not in region:
    raise SystemExit("auto-child class string is not record-bounded")
print("Mrm auto-child class strings are record-bounded")
