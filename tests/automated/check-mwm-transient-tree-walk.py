#!/usr/bin/env python3
"""Check MWM transient-tree operations do not recurse on dialog depth."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWinList.c").read_text(
    encoding="utf-8"
)


def body(signature: str, end_marker: str) -> str:
    start = source.index(signature)
    return source[start : source.index(end_marker, start)]


walker = body("VisitTransientSubtreePostorder", "static void\nMarkModalSubtreeVisitor")
assert "node->transientChildren" in walker
assert "node->transientSiblings" in walker
assert "node->transientLeader" in walker
assert "boundary = tree->transientLeader;" in walker

mark = body("void MarkModalSubtree", "void MarkModalTransient")
assert "VisitTransientSubtreePostorder(pcdTree" in mark
assert "MarkModalSubtree (pcdTree->" not in mark

delete = body("void DeleteFullAppModalChildren", "} /* END OF FUNCTION DeleteFullAppModalChildren")
assert "VisitTransientSubtreePostorder(pCD" in delete
assert "DeleteFullAppModalChildren (pcdLeader," not in delete

fixup = body("FixupFullAppModalCounts (ClientData *pcdLeader", "} /* END OF FUNCTION FixupFullAppModalCounts")
assert "VisitTransientSubtreePostorder(pcdLeader" in fixup
assert "FixupFullAppModalCounts (pcdLeader->" not in fixup

count = body("CountTransientChildren (ClientData *pcd)", "} /* END OF FUNCTION CountTransientChildren")
assert "boundary = pcd->transientLeader;" in count
assert "if (count == INT_MAX)" in count
assert "CountTransientChildren (pcdNext)" not in count

print("PASS MWM transient-tree operations use bounded iterative traversal")
