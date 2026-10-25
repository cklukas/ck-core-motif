#!/usr/bin/env python3
"""Check that XmIm avoids stale-XIM assertion failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("if ((ret != NULL) && unrecognized)")
end = source.index("if (flags & GEO_CHG)", start)
body = source[start:end]

assert "if (XIMOfIC(icp->xic) != xim_info->xim)" in body
assert "assert(XIMOfIC(icp->xic) == xim_info->xim)" not in body

print("XmIm stale-XIM fallback guard: ok")
