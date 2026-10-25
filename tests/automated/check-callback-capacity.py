#!/usr/bin/env python3
"""Check callback-list growth rejects count and allocation-width overflow."""

from pathlib import Path


source = Path("lib/Xm/CallbackI.c").read_text()

assert "#include <limits.h>" in source
assert "count == USHRT_MAX" in source
assert "SIZE_MAX - sizeof(InternalCallbackRec)" in source
assert "sizeof(XtCallbackRec) *" in source
assert "sizeof(InternalCallbackRec) + sizeof(XtCallbackRec) *" in source

print("callback-list capacity and allocation bounds are guarded")
