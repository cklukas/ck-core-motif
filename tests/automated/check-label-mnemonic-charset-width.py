#!/usr/bin/env python3
"""Check that Label mnemonic charset export does not narrow its length."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text()
start = source.index("static void \nGetMnemonicCharSet")
end = source.index("static void \nSetValuesAlmost", start)
region = source[start:end]

assert "int   size" not in region
assert "lw->label.mnemonicCharset[0] != '\\0'" in region
assert "_XmStringCharSetCreate(lw->label.mnemonicCharset)" in region
print("Label mnemonic charset export avoids narrow length conversion")
