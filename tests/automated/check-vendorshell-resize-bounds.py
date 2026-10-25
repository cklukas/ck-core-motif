#!/usr/bin/env python3
"""Check VendorShell Resize handles missing extension data and small heights."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "VendorS.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("static void \nResize(")
end = text.index("/************************************************************************\n *\n *  ChangeManaged", start)
body = text[start:end]
if "if(extData == NULL)" not in body or "return;" not in body:
    raise SystemExit("VendorShell Resize does not return on missing extension data")
if "if (vendorExt == NULL)" not in body:
    raise SystemExit("VendorShell Resize does not validate extension object")
if "y = (int)sw->core.height - (int)vendorExt->vendor.im_height;" not in body:
    raise SystemExit("VendorShell Resize height arithmetic is not signed")
if "if (y < 0)" not in body:
    raise SystemExit("VendorShell Resize does not clamp negative child height")
print("VendorShell Resize guards extension data and height bounds")
