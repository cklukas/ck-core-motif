#!/usr/bin/env python3
"""Check widget class and callback offsets stay inside Mrm records."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
assert text.count("!UrmWidgetStringValid(widgetrec, widgetrec->class_offs)") >= 3
assert text.count("!UrmCallbackRecordValid(widgetrec, widgetrec->creation_offs,") == 2
assert "if (offset < RGMWidgetRecordHdrSize || offset >= widgetrec->size)" in text
print("Mrm widget class and callback offsets are bounded")
