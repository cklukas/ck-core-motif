#!/usr/bin/env python3
"""Check Editres has typed C23 callback and protocol declarations."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2] / "demos" / "lib" / "Xmd" / "RegEdit.c"
).read_text(encoding="utf-8")

contracts = (
    "static void _XEditResCheckMessages(Widget, XtPointer, XEvent *, Boolean *);",
    "_XEditResCheckMessages(Widget w, XtPointer data, XEvent *event, Boolean *cont)",
    "GetCommand(Widget w, XtPointer data, Atom *selection, Atom *type,",
    "ExecuteCommand(Widget w, Atom sel, ResIdent ident, EditresEvent *event)",
    "DumpChildren(Widget w, ProtocolStream *stream, unsigned short *count)",
    "DoGetGeometry(Widget w, EditresEvent *event, ProtocolStream *stream)",
    "_XEditResPut32(ProtocolStream *stream, unsigned long value)",
    "_XEditResGetWidgetInfo(ProtocolStream *stream, WidgetInfo *info)",
    "CvtStringToBlock(Display *dpy, XrmValue *args, Cardinal *num_args,",
)

for contract in contracts:
    assert contract in source, f"missing typed Editres contract: {contract}"

legacy_definitions = (
    "DumpChildren(w, stream, count)\nWidget w;",
    "DoGetGeometry(w, event, stream)\nWidget w;",
    "_XEditResPut32(stream, value)\nProtocolStream * stream;",
    "CvtStringToBlock(dpy, args, num_args, from_val, to_val, converter_data)\n",
)
for legacy in legacy_definitions:
    assert legacy not in source, f"legacy K&R Editres definition remains: {legacy!r}"

print("Editres protocol callbacks and helpers use typed C23 prototypes")
