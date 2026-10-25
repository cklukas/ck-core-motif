#!/usr/bin/env python3
"""Ensure manager examples render their defining controls, not placeholders."""
from pathlib import Path

source = (Path(__file__).parents[2] / "demos" / "programs" /
          "ckmotif-gallery" / "gallery_examples.c").read_text(encoding="utf-8")

assert '#include <Xm/TabStack.h>' in source
tabstack = source[source.index('} else if (class == xmTabStackWidgetClass) {'):]
tabstack = tabstack[:tabstack.index('} else {', 1)]
for label in ("Overview", "Details", "Activity"):
    assert f'"{label}"' in tabstack
assert 'XmNtabLabelString' in tabstack
assert '"Live component content"' not in source
toolbar = source[source.index('} else if (class == xmToolbarWidgetClass) {'):]
toolbar = toolbar[:toolbar.index('} else {', 1)]
for label in ("Open", "Save", "Print", "Undo"):
    assert f'"{label}"' in toolbar
assert "XmToolbarSetActions(example, actions, 4)" in toolbar
print("PASS gallery manager examples render live controls")
