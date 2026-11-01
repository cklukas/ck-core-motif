#!/usr/bin/env python3
"""Check MWM client-command payloads reject non-byte selection formats."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProtocol.c").read_text(
    encoding="utf-8"
)
assert "#define MWM_CCI_FORMAT 8" in source
assert "wmGD.xa_MWM_OFFSET > (Atom)LONG_MAX" in source
assert "clientMsgEvent.data.l[0] = (long)wmGD.xa_MWM_OFFSET;" in source
offset_start = source.index("void SendClientOffsetMessage")
offset_end = source.index("void SendClientMsg", offset_start)
offset_region = source[offset_start:offset_end]
assert offset_region.index("pCD == NULL") < offset_region.index("pCD->xBorderWidth")
assert "if (type < 0)" in source
assert "clientMsgEvent.message_type = (Atom)type;" in source

for target, handler in (
    ("_MOTIF_WM_DEFINE_COMMAND", "DefineCommand"),
    ("_MOTIF_WM_INCLUDE_COMMAND", "IncludeCommand"),
    ("_MOTIF_WM_REMOVE_COMMAND", "RemoveCommand"),
    ("_MOTIF_WM_ENABLE_COMMAND", "EnableCommand"),
    ("_MOTIF_WM_DISABLE_COMMAND", "DisableCommand"),
    ("_MOTIF_WM_RENAME_COMMAND", "RenameCommand"),
):
    start = source.index(f"target == wmGD.{target}")
    end = source.find("    else if (target ==", start + 1)
    region = source[start:end if end != -1 else len(source)]
    guard = "input == NULL || inputFmt != MWM_CCI_FORMAT"
    assert guard in region
    assert f"{handler}(w, target, (MessageData)input, inputLen, inputFmt);" in region
    assert region.index(guard) < region.index(handler)

print("PASS MWM client-command payloads require the byte wire format")
