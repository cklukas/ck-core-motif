#!/usr/bin/env python3
"""Check Mwm icon-box creation reports failure with the correct ABI types."""

from pathlib import Path


source = Path(__file__).parents[2] / "clients" / "mwm" / "WmIconBox.c"
text = source.read_text(encoding="utf-8")
region = text[text.index("if (iconWidget == NULL)"):text.index("if (dummyString == NULL)")]
assert 'MWarning("%s", (char *)GETMESSAGE' in region
assert "return (False);" in region
assert "return (NULL);" not in region
print("WmIconBox creation failure uses type-correct warning and return")
