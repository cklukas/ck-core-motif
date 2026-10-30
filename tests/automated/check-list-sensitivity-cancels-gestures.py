#!/usr/bin/env python3
"""Check List sensitivity changes cancel delayed gesture work."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text()
helper_start = source.rindex("CancelPendingInteraction(XmListWidget lw)")
helper_end = source.index("\n}\n", helper_start) + 2
helper = source[helper_start:helper_end]

assert "XtRemoveTimeOut(lw->list.drag_start_timer)" in helper
assert "XtRemoveTimeOut(lw->list.DragID)" in helper
assert "lw->list.Event = 0;" in helper
assert "lw->list.AppendInProgress = FALSE;" in helper
assert "lw->list.AutoSelectionType = XmAUTO_UNSET;" in helper

sensitivity = source[source.index("if (XtIsSensitive(new_w)"):source.index("/* Select color", source.index("if (XtIsSensitive(new_w)"))]
assert "CancelPendingInteraction(newlw);" in sensitivity

browse_start = source.rindex("BrowseScroll(XtPointer closure,")
browse = source[browse_start:browse_start + 700]
assert "if (!XtIsSensitive((Widget) lw))" in browse

drag_start = source.rindex("DragStart(XtPointer closure,")
drag = source[drag_start:drag_start + 500]
assert "if (!XtIsSensitive((Widget) lw))" in drag

print("List sensitivity changes cancel pending gesture callbacks")
