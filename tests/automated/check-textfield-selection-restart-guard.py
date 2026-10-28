#!/usr/bin/env python3
"""Check clipboard selection restart remains conditional on a nonempty range."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text(
    encoding="utf-8"
)
start = source.index("if (ds->selection == atoms[XmACLIPBOARD])", source.index("if (replace_res)"))
end = source.index("\n\t} else {", start)
body = source[start:end]
assert "if (left != right && (!dest_disjoint || !tf->text.add_mode)) {" in body
assert body.count("_XmProcessLock();") == 1
assert body.count("_XmProcessUnlock();") == 1
print("PASS TextField clipboard selection restart is range-guarded")
