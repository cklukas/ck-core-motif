#!/usr/bin/env python3
"""Check UIL control-entry constructors handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()

start = source.index("void\t\tsar_save_control_item")
end = source.index("void\t\tsar_save_control_widget", start)
body = source[start:end]
allocation = body.index("control_entry =")
guard = body.index("if (control_entry == NULL)", allocation)
assert guard < body.index("control_entry->az_con_obj", guard)

start = source.index("void\t\tsar_save_control_widget")
end = source.index("\f", start)
body = source[start:end]
allocation = body.index("control_entry =")
guard = body.index("if (control_entry == NULL)", allocation)
assert guard < body.index("control_entry->az_con_obj", guard)
assert body.count("*item_frame = *control_frame;") >= 2
assert body.count("*control_frame = temp_frame;") >= 2
print("UIL control-entry allocations and frame rollback are guarded")
