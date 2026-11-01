#!/usr/bin/env python3
"""NumberBox bounds tentative native text and committed decimal formatting."""
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/NumberBox.c").read_text()
assert "XmNmaxLength,63" in source
assert 'snprintf(text,sizeof(text),"%d",number->number_box.value)' in source
assert "XmTextFieldGetString" in source and "XtFree(text)" in source
print("PASS NumberBox native text length and ownership bounds")
