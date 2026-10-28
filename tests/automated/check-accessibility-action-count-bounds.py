#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/AccessibleDBus.c").read_text()

assert "static dbus_int32_t bounded_action_count(unsigned int count)" in source
assert "count > (unsigned int)INT32_MAX ? INT32_MAX : (dbus_int32_t)count" in source
assert 'number = bounded_action_count(info->action_count);' in source
