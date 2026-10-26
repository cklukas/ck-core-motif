#!/usr/bin/env python3
"""Ensure the Text old-highlight snapshot grows transactionally."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmTextSetHighlight(Widget w,")
end = source.index("\nvoid \n_XmTextShowPosition", start)
region = source[start:end]

assert "Cardinal requested_maximum = tw->text.highlight.number;" in region
assert "_XmHighlightRec *new_list;" in region
assert "requested_maximum > UINT_MAX / sizeof(_XmHighlightRec)" in region
assert "if (new_list == NULL)" in region
assert "tw->text.old_highlight.maximum = requested_maximum;" in region
assert "tw->text.old_highlight.list = new_list;" in region
assert "tw->text.highlight_changed = TRUE;" in region

failure = region.index("if (new_list == NULL)")
published = region.index("tw->text.old_highlight.maximum = requested_maximum;")
assert failure < published

print("text old-highlight snapshot growth: transactional realloc")
