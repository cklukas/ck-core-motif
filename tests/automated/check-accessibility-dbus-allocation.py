#!/usr/bin/env python3
"""Check that AT-SPI memo-record allocations fail closed."""

from pathlib import Path
import re

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()

assert "record = memo_get(t, id, 1);" in source
assert "if (!record)" in source
assert "TransportState *record = memo_get(t, id, 1);" in source
assert source.count("record = memo_get(t, id, 1);\n  if (!record)") >= 2
assert "record = memo_get(t, id, 1);\n    if (!record)" in source
assert "record = memo_get(t, ids[k], 1);\n    if (!record)" in source
assert len(re.findall(
    r"reply = dbus_message_new_method_return\(call\);\n\s+if \(!reply\)",
    source)) >= 3
for marker in (
    'message = dbus_message_new_signal(path,\n      "org.a11y.atspi.Event.Object", "StateChanged");\n  DBusMessageIter',
    'message = dbus_message_new_signal(path, "org.a11y.atspi.Event.Window",',
    'message = dbus_message_new_signal(CACHEPATH, CACHE, "RemoveAccessible");',
):
    assert marker in source
assert 'if (!message)\n    return;' in source
assert source.count(
    'call = dbus_message_new_method_call("org.a11y.Bus"') == 1
assert 'if (!call) {\n    dbus_connection_close(session);' in source
assert 'call = dbus_message_new_method_call("org.a11y.atspi.Registry", ROOT,' in source
assert 'if (!call) {\n    a->status = XmACCESSIBILITY_UNAVAILABLE;' in source
print("PASS accessibility D-Bus memo allocation failures are guarded")
