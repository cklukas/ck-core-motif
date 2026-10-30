#!/usr/bin/env python3
"""Check SelectionBox synthetic getters reject invalid widgets and outputs."""

from pathlib import Path


source = Path("lib/Xm/SelectioB.c").read_text()
for function in (
    "_XmSelectionBoxGetSelectionLabelString",
    "_XmSelectionBoxGetListLabelString",
    "_XmSelectionBoxGetTextColumns",
    "_XmSelectionBoxGetTextString",
    "_XmSelectionBoxGetListItems",
    "_XmSelectionBoxGetListItemCount",
    "_XmSelectionBoxGetListVisibleItemCount",
    "_XmSelectionBoxGetOkLabelString",
    "_XmSelectionBoxGetApplyLabelString",
    "_XmSelectionBoxGetCancelLabelString",
    "_XmSelectionBoxGetHelpLabelString",
):
    start = source.index(function + "(")
    end = source.index("\n}", start)
    body = source[start:end]
    assert "value == NULL" in body, function
    assert "wid == NULL" in body, function
    assert "XtIsSubclass( wid, xmSelectionBoxWidgetClass)" in body, function

print("SelectionBox synthetic getters validate widget and output pointers")
