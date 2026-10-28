#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/AccessibleDBus.c").read_text()

assert "dbus_uint32_t type, symbol_bits, stamp_bits;" in source
assert "symbol_bits = (dbus_uint32_t)symbol;" in source
assert "memcpy(&identity, &symbol_bits, sizeof(identity));" in source
assert "identity = (dbus_int32_t)symbol;" not in source
