#!/usr/bin/env python3
"""Check modal-tree helpers validate recursive inputs."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
checks = {
    "MarkModalTransient": "pcdLeader == NULL || pCD == NULL",
    "FixupFullAppModalCounts": "pcdLeader == NULL || pcdDelete == NULL",
    "UnMarkModalTransient": "pcdModee == NULL || pcdModal == NULL || modalCount <= 0",
}
for name, needle in checks.items():
    start = text.index({
        "MarkModalSubtree": "void MarkModalSubtree",
        "MarkModalTransient": "void MarkModalTransient",
        "DeleteFullAppModalChildren": "void DeleteFullAppModalChildren",
        "FixupFullAppModalCounts": "FixupFullAppModalCounts (ClientData",
        "UnMarkModalTransient": "void UnMarkModalTransient",
    }[name])
    assert needle in text[start:start + 500], name
mark = text[text.index("void MarkModalSubtree"):text.index("void MarkModalTransient")]
assert "VisitTransientSubtreePostorder(pcdTree" in mark
delete = text[text.index("void DeleteFullAppModalChildren"):text.index("} /* END OF FUNCTION DeleteFullAppModalChildren")]
assert "pcdLeader == NULL" in delete
assert "VisitTransientSubtreePostorder(pCD" in delete
print("modal-tree helpers validate iterative state")
