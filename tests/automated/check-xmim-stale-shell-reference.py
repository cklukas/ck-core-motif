#!/usr/bin/env python3
"""Check that XmIm reconstruction tolerates released shell data."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("recreate_xic_info(XIC")
end = source.index("create_xic_info(Widget", start)
body = source[start:end]

assert "if (tmp_info == NULL)" in body
assert "assert(tmp_info != NULL)" not in body
close_start = source.index("XmImCloseXIM(")
close = source[close_start:source.index("/*********************", close_start)]
assert "remove_ref(&xim_info->shell_refs, shell)" in close
assert "assert((xim_info->shell_refs.refs == NULL)" not in close
free_start = source.index("_XmImFreeShellData(")
free_shell = source[free_start:source.index("/* Delete the dummy widget.", free_start)]
assert "im_info->shell_xic = NULL;" in free_shell
assert "assert(im_info->shell_xic == NULL)" not in free_shell

print("XmIm stale shell-reference guard: ok")
