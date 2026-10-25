#!/usr/bin/env python3
"""Check that AT-SPI transport startup handles state allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c"
text = source.read_text(encoding="utf-8")
start = text.index("  t = (Transport *)XtCalloc(1, sizeof(*t));")
end = text.index("\n  t->owner = a;", start)
region = text[start:end]
assert "if (!t)" in region
assert "dbus_message_unref(reply);" in region
assert "dbus_connection_close(session);" in region
assert "return a->status = XmACCESSIBILITY_UNAVAILABLE;" in region
print("accessibility transport startup guards state allocation")
