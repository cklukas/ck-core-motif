#!/usr/bin/env python3
"""Check MWM's workspace protocol walks transient trees without C recursion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWorkspace.c").read_text(
    encoding="utf-8"
)

walk_start = source.index("static Boolean visit_tree")
walk_end = source.index("\n}\n\nstatic WmScreenData *screen_for_widget", walk_start)
walk = source[walk_start:walk_end]
assert "WsmTreeFrame" in walk
assert "XtCalloc" in walk
assert "XtRealloc" in walk
assert "XtFree" in walk
assert "capacity > SIZE_MAX / 2" in walk
assert "new_capacity > UINT_MAX / sizeof(*frames)" in walk
assert "return complete;" in walk

find_start = source.index("static ClientData *find_tree")
find_end = source.index("\n}\n\nstatic ClientData *find_client", find_start)
find_tree = source[find_start:find_end]
assert "visit_tree(client, find_tree_visit" in find_tree
assert "find_tree(child" not in find_tree

visibility_start = source.index("void WsmApplyVisibility")
visibility_end = source.index("\n}\n\ntypedef struct { WmScreenData", visibility_start)
visibility = source[visibility_start:visibility_end]
assert "visit_tree(client, apply_visibility" in visibility
assert "WsmApplyVisibility(child" not in visibility

count_start = source.index("static int tree_count")
count_end = source.index("\n}\n\ntypedef struct {\n    WSMWinInfo *items;", count_start)
count = source[count_start:count_end]
assert "visit_tree(client, count_tree" in count
assert "tree_count(child" not in count

snapshot_start = source.index("static Boolean tree_snapshot")
snapshot_end = source.index("\n}\n\nstatic void discard_snapshots", snapshot_start)
snapshot = source[snapshot_start:snapshot_end]
assert "visit_tree(client, snapshot_tree" in snapshot
assert "tree_snapshot(child" not in snapshot

request_start = source.index("void HandleWsmConvertRequest")
request_end = source.index("\n}\n\nvoid WsmConnectExisting", request_start)
request = source[request_start:request_end]
assert "if (tree > 65535 - count)" in request
assert "discard_snapshots(reply->get_state.win_info_list, used);" in request
assert request.index("discard_snapshots(reply->get_state.win_info_list, used);") < request.index(
    "reply->any.allocated = True;"
)

print("PASS MWM workspace transient-tree traversal is iterative and bounded")
