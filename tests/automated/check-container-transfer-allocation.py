#!/usr/bin/env python3
"""Check that Container transfer setup handles allocation failure safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerStartTransfer(")
end = source.index("/************************************************************************\n * ContainerPrimaryCopy", start)
region = source[start:end]
assert region.count("transfer_action == NULL") >= 1
assert "transfer_action == NULL)\n\t\t    return;" in region
assert "transfer_action->event == NULL" in region
primary = source[source.index("ContainerPrimaryCopy("):source.index("/************************************************************************\n * ContainerPrimaryLink", source.index("ContainerPrimaryCopy("))]
link = source[source.index("ContainerPrimaryLink("):source.index("/************************************************************************\n * ContainerPrimaryMove", source.index("ContainerPrimaryLink("))]
move = source[source.index("ContainerPrimaryMove("):source.index("/************************************************************************\n * Actions to dispatch", source.index("ContainerPrimaryMove("))]
for function in (primary, link, move):
    assert "loc_data == NULL" in function
drop = source[source.index("ContainerDestPrehookProc("):source.index("/************************************************\n * Free data allocated", source.index("ContainerDestPrehookProc("))]
assert "loc_data == NULL" in drop
print("PASS Container transfer allocations are guarded")
