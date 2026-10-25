#!/usr/bin/env python3
"""Check that fixed Motif converter buffers use bounded Latin-1 lowering."""
from pathlib import Path
root = Path(__file__).parents[2] / "lib" / "Xm"
helper = (root / "XmExtUtil.c").read_text(encoding="utf-8")
assert "XmCopyISOLatin1LoweredN(dst, capacity, src)" in helper
assert "i + 1 < capacity" in helper
for name in ['ButtonBox.c', 'ColorS.c', 'FontS.c', 'Hierarchy.c', 'IconButton.c', 'Paned.c', 'Tree.c']:
    text = (root / name).read_text(encoding="utf-8")
    assert "XmCopyISOLatin1LoweredN" in text, name
    assert "XmCopyISOLatin1Lowered(" not in text, name
print("PASS fixed Latin-1 converter buffers use bounded lowering")
