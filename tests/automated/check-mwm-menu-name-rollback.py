#!/usr/bin/env python3
"""Check duplicated MWM menu names are replaced transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWinInfo.c").read_text()
icon_start = source.index('XtNewString("IconBoxMenu")')
icon_region = source[icon_start - 220:icon_start + 260]
proto_start = source.index('XtNewString("ProtocolsMenu")')
proto_region = source[proto_start - 180:proto_start + 240]

assert "if (menu_name != NULL)" in icon_region
assert icon_region.index('XtNewString("IconBoxMenu")') < icon_region.index("XtFree(pCD->systemMenuSpec->name)")
assert "if (menu_name != NULL)" in proto_region
assert proto_region.index('XtNewString("ProtocolsMenu")') < proto_region.index("XtFree(pCD->systemMenuSpec->name)")
print("MWM menu names preserve old values on allocation failure")
