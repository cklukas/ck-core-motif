#!/usr/bin/env python3
"""Check that UIL debug fragments are flushed at listing cleanup."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text()

assert "static void lst_debug_flush(void);" in source
assert "lst_debug_cur_pos = 0;" in source
cleanup = source[source.index("void\tUil_lst_cleanup_listing") :]
assert "lst_debug_flush();" in cleanup[:cleanup.index("static void\nlst_debug_append")]
debug = source[source.index("lst_debug_append") :]
assert "lst_debug_buffer[lst_debug_cur_pos] = '\\0';" in debug
assert "lst_output_line(lst_debug_buffer, FALSE);" in debug
print("PASS UIL debug fragments flush at listing cleanup")
