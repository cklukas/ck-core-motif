#!/usr/bin/env python3
"""Keep gallery presentation on shared toolkit defaults and semantic roles."""
from pathlib import Path
source=(Path(__file__).parents[2]/"demos/programs/ckmotif-gallery/gallery.c").read_text()
for private_style in ("XAllocNamedColor", "style_gallery_tab", "fontName:", "fontSize:",
                      "topShadowColor:", "bottomShadowColor:", "renderTable:"):
    assert private_style not in source, private_style
assert "XmVaCreateSegmentedControl(pane" in source
assert "XmNSegmentedSelectionCallback, select_tab" in source
for role in ("page_title", "section", "secondary", "code"):
    assert "appearanceRole: " + role in source
print("PASS gallery uses toolkit styling and semantic roles")
