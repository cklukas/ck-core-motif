#!/usr/bin/env python3
"""Verify Exm Container copies detail headers transactionally."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "unsupported"
    / "Exm"
    / "Container.c"
).read_text(encoding="utf-8")

start = source.index("if (cw->container.detail_header && \n\tcw->container.detail_header_count) {")
end = source.index("\n    if (cw->container.detail_header && cw->container.detail_header_count) {", start)
copy = source[start:end]

assert "UINT_MAX / sizeof(*copied_header)" in copy
assert "XtCalloc(" in copy
assert "XmStringCopy(rcw->container.detail_header[i])" in copy
assert "if (!copied)" in copy
assert "XmStringFree(copied_header[i]);" in copy
assert "cw->container.detail_header = NULL;" in copy
assert "cw->container.detail_header_count = 0;" in copy
assert "cw->container.detail_header = copied_header;" in copy

print("Exm Container detail-header copying is bounded and transactional")
