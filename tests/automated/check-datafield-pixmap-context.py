#!/usr/bin/env python3
"""Check pixmap-context setup handles context-record allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("context, (char **) &tf_cache_pixmap")
start = source.index("ctx_data =", start)
end = source.index("ctx_data->screen", start)
context = source[start:end]
if "if (ctx_data == NULL)" not in context or "return;" not in context:
    raise SystemExit("pixmap-context allocation is not guarded")

print("DataField pixmap-context checks passed")
