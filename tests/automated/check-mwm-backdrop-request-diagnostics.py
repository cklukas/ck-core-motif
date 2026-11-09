#!/usr/bin/env python3
"""Check missing backdrop paths are not reported as allocation failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmEvent.c").read_text(
    encoding="utf-8"
)
start = source.index("DTWM_REQ_CHANGE_BACKDROP")
end = source.index("if (sRequest)", start)
body = source[start:end]

missing = '"Missing path name for backdrop change request."'
oom = '"Insufficient memory to handle backdrop change."'
assert missing in body and oom in body
assert "else if (pch != NULL)" in body
assert body.index("else if (pch != NULL)") < body.index(oom)
assert "pchReq[strlen(DTWM_REQ_CHANGE_BACKDROP)] == '\\0'" in body
assert "pchReq[strlen(DTWM_REQ_CHANGE_BACKDROP)] == ' '" in body

print("PASS MWM backdrop requests validate the exact command and distinguish missing paths from OOM")
