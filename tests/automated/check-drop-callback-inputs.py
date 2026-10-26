#!/usr/bin/env python3
"""Check that DnD callback wrappers validate teardown inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
delete_start = source.index("DeleteDropCBStruct(Widget w")
drop_start = source.index("DropDestinationHandler(Widget w")
assert "cs == NULL || cs -> transfer_id == NULL" in source[delete_start:drop_start]
region = source[drop_start:source.index("/*************************************************************/\n/* XmePrimarySink", drop_start)]
assert "if (w == NULL || ds == NULL)" in region
assert "MOTIF_DROP = XInternAtom" in region
print("DnD callback wrappers validate teardown inputs before dereferencing")
