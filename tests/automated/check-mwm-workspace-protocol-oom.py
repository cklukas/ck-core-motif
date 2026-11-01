#!/usr/bin/env python3
"""Check MWM workspace-protocol allocation failures remain non-fatal."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWorkspace.c").read_text(
    encoding="utf-8"
)

track_start = source.index("static void track_owner")
track_end = source.index("\n}\n\nstatic void formats", track_start)
track_owner = source[track_start:track_end]
assert "if (!owners)" in track_owner
assert "if (!owners)\n            return;" in track_owner
assert track_owner.index("if (!owners)\n            return;") < track_owner.index(
    "WorkspaceOwner *owner"
)

snapshot_start = source.index("static void snapshot")
snapshot_end = source.index("\n}\n\nstatic int tree_count", snapshot_start)
snapshot = source[snapshot_start:snapshot_end]
assert "if (!info->data_list)" in snapshot
assert "info->num_data_list = 0;" in snapshot
assert snapshot.index("if (!info->data_list)") < snapshot.index("for (int i = 0")

request_start = source.index("void HandleWsmConvertRequest")
request_end = source.index("\n}\n\nvoid WsmConnectExisting", request_start)
request = source[request_start:request_end]
assert "if (count != 0 && !reply->get_state.win_info_list)" in request
assert request.index("if (count != 0 && !reply->get_state.win_info_list)") < request.index(
    "reply->any.allocated = True;"
)

print("PASS MWM workspace protocol allocation failures are guarded")
