#!/usr/bin/env python3
"""Check MultiList selection helpers reject a missing internal list child."""

from pathlib import Path


source = Path("lib/Xm/MultiList.c").read_text()
start = source.index("XmMultiListGetSelectedRows(Widget w)")
end = source.index("/* aliases of deprecated", start)
body = source[start:end]

helpers = (
    "XmMultiListGetSelectedRows",
    "XmMultiListUnselectAllItems",
    "XmMultiListUnselectItem",
    "XmMultiListToggleRow",
    "XmMultiListSelectItems",
    "XmMultiListDeselectItems",
    "XmMultiListSelectAllItems",
    "XmMultiListSelectRow",
    "XmMultiListDeselectRow",
    "XmMultiListGetSelectedRowArray",
    "XmMultiListMakeRowVisible",
)
for helper in helpers:
    function_start = body.index(helper + "(")
    function_end = body.find("\n}\n", function_start)
    region = body[function_start:function_end]
    assert "XmMultiList_ilist(elist) == NULL" in region, helper

array_start = body.index("XmMultiListGetSelectedRowArray")
array_region = body[array_start:body.index("\n}\n", array_start)]
assert "*num_rows = 0" in array_region

print("MultiList selection helpers reject missing internal list children")
