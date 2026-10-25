#!/usr/bin/env python3
"""Check focus and transient stacking helpers handle detached clients."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
focus = text.index("ClientData * FindTransientFocus")
assert "if (pcd == NULL)" in text[focus:text.index("    /*", focus)]
bump = text.index("Boolean BumpPrimaryToBottom")
assert "if (pcdLeader == NULL)" in text[bump:text.index("    if (pcdLeader->primaryStackPosition", bump)]
normalize = text.index("NormalizeTransientTreeStacking (")
assert "if (pcdLeader == NULL)" in text[normalize:text.index("    pcdNext = pcdLeader->transientChildren", normalize)]
print("transient focus and stacking helpers validate clients")
