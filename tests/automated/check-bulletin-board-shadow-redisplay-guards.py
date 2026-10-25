#!/usr/bin/env python3
"""Check XmBulletinBoard validates its expose-time shadow draw state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BulletinB.c").read_text()
start = source.index("static void \nRedisplay(")
end = source.index("    return ;\n    }", start)
redisplay = source[start:end]

assert "bb->manager.shadow_thickness != 0" in redisplay
assert "XtIsRealized((Widget)bb)" in redisplay
assert "bb->manager.top_shadow_GC != NULL" in redisplay
assert "bb->manager.bottom_shadow_GC != NULL" in redisplay
assert "bb->core.width != 0" in redisplay
assert "bb->core.height != 0" in redisplay
print("PASS BulletinBoard shadow redisplay guards")
