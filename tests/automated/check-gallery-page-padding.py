#!/usr/bin/env python3
"""Check the shared page layout contract; runtime geometry is tested natively."""
from pathlib import Path
source = (Path(__file__).parents[2] / "demos/programs/ckmotif-gallery/gallery.c").read_text()
assert "static Widget create_page" in source
assert "XmNmarginWidth, 24, XmNmarginHeight, 24" in source
for page in ("welcomePage", "examplePage", "sourcePage", "iconsPage"):
    assert f'create_page(' in source and f'"{page}"' in source
assert "XmNpacking, XmPACK_NONE" in source
assert "XmVaCreateManagedNotebook" not in source
assert "static void select_page" in source
print("PASS gallery pages share consistent padding and purpose-sized previews")
