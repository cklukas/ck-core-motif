#!/usr/bin/env python3
"""Check that deferred TabStack selection cannot draw through a null GC."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c"
text = source.read_text(encoding="utf-8")

realize_start = text.index("static void\n#ifndef _NO_PROTO\nRealize")
realize_end = text.index("\n/*\n * Function:\n *\tResize", realize_start)
realize = text[realize_start:realize_end]
assert realize.index("XmTabStack__gc(tab) = XCreateGC") < realize.index(
    "XmTabStackSelectTab(XmTabStack__selected_tab(tab),"
), "TabStack restores the deferred selection before its drawing GC exists"

callback_start = text.index("TabSelectedCallback(Widget widget")
callback_end = text.index("\nWidget\nXmVaCreateManagedTabStack", callback_start)
callback = text[callback_start:callback_end]
assert "XtIsRealized((Widget)tab) &&" in callback
assert "XiSelectSpecified(tab) && XmTabStack__gc(tab) != NULL" in callback
assert "XiBackgroundSpecified(active) && XmTabStack__gc(tab) != NULL" in callback
assert "XmTabStack_stacked_effect(tab)) &&\n\tXmTabStack__gc(tab) != NULL" in callback

print("TabStack deferred selection creates and guards its drawing GC")
