#!/usr/bin/env python3
"""Check focus and transient stacking helpers handle detached clients."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
focus = text.index("ClientData * FindTransientFocus")
focus_end = text.index("} /* END OF FUNCTION FindTransientFocus */", focus)
focus_body = text[focus:focus_end]
assert "if (pcd == NULL)" in focus_body[:focus_body.index("    /*")]
assert "while (pcdSibling->transientSiblings && IS_APP_MODALIZED(pcdSibling))" in focus_body
assert "while (pcdSibling->transientSiblings && IS_APP_MODALIZED(pcdFocus))" not in focus_body
bump = text.index("Boolean BumpPrimaryToBottom")
assert "if (pcdLeader == NULL)" in text[bump:text.index("    if (pcdLeader->primaryStackPosition", bump)]
normalize = text.index("NormalizeTransientTreeStacking (")
assert "if (pcdLeader == NULL)" in text[normalize:text.index("    pcdNext = pcdLeader->transientChildren", normalize)]
print("PASS transient focus and stacking helpers validate clients and sibling modality")
