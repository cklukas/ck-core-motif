#!/usr/bin/env python3
"""Check panel transient remapping preserves a valid fallback leader."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmManage.c").read_text(
    encoding="utf-8"
)
start = source.index("Handle case of transients that derive from embedded clients")
end = source.index("if (IsEmbeddedClient", start)
body = source[start:end]

assert "if (!XFindContext" in body
assert "pCDfp != NULL" in body
assert body.index("pCDfp != NULL") < body.index("pCD->transientLeader = pCDfp;")
assert "pCD->transientLeader = pCDfp;" not in body[: body.index("if (!XFindContext")]

print("PASS MWM preserves transient leader when panel lookup is unavailable")
