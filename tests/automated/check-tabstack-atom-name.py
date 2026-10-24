#!/usr/bin/env python3
"""Check that TabStack atom names preserve wide process IDs safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text()
start = source.index("TabAtom(Widget widget)")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert 'snprintf(pid_buf, sizeof(pid_buf), "ICS_TAB_PID_%ld\\n"' in region
assert "(long)getpid()" in region
assert 'sprintf(pid_buf, "ICS_TAB_PID_%d' not in region
print("TabStack atom names preserve process IDs safely")
