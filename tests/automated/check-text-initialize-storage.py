#!/usr/bin/env python3
"""Ensure Text initialization publishes its fixed buffers transactionally."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("InitializeTextStorage(XmTextWidget tw)")
helper_end = source.index("\n/*\n * Create a text widget", helper_start)
helper = source[helper_start:helper_end]

assert "Line line;" in helper
assert "RangeRec *range;" in helper
assert "_XmHighlightRec *highlight;" in helper
assert "_XmHighlightRec *old_highlight;" in helper
assert "line = (Line) XtMalloc(sizeof(LineRec));" in helper
assert "range = (RangeRec *) XtMalloc(sizeof(RangeRec));" in helper
assert "highlight = (_XmHighlightRec *) XtMalloc(sizeof(_XmHighlightRec));" in helper
assert "old_highlight = (_XmHighlightRec *) XtMalloc(sizeof(_XmHighlightRec));" in helper
assert "line == NULL || range == NULL || highlight == NULL" in helper
assert "XtFree((char *) line);" in helper
assert "XtFree((char *) old_highlight);" in helper

publish = [
    "tw->text.line = line;",
    "tw->text.repaint.range = range;",
    "tw->text.highlight.list = highlight;",
    "tw->text.old_highlight.list = old_highlight;",
]
for statement in publish:
    assert statement in helper

hook_start = source.index("InitializeHook(Widget wid,")
hook_end = source.index("\n  /* Translation table overwrite */", hook_start)
hook = source[hook_start:hook_end]
assert "if (!InitializeTextStorage(tw))" in hook
assert 'XtError("Unable to allocate Text widget storage");' in hook
assert "tw->text.line->" not in hook
assert "tw->text.highlight.list[0]" not in hook

print("text initialization storage: transactional allocation and failure path present")
