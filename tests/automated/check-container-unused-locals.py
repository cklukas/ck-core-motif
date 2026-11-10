#!/usr/bin/env python3
"""Check Container does not retain discarded local state."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)

change_managed = source[source.rindex("ChangeManaged(\n") : source.index(
    "ChangeManagedOutlineDetail(\n", source.rindex("ChangeManaged(\n")
)]
assert "Boolean             outline_ready" not in change_managed

test_fit = source[source.rindex("TestFitItem(\n") : source.index(
    "PlaceItem(\n", source.rindex("TestFitItem(\n")
)]
assert "XtGeometryResult result" not in test_fit
assert "(void)_XmMakeGeometryRequest(cwid, &child_req);" in test_fit

previous = source[source.rindex("GetPrevTraversalWidget(\n") : source.index(
    "_XmContainerDrawDisclosure", source.rindex("GetPrevTraversalWidget(\n")
)]
assert "CwidNode\tlast_node;" not in previous

print("PASS Container does not retain discarded local state")
