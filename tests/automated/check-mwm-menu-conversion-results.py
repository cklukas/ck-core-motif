from pathlib import Path

source = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")
start = source.index("MenuItem *\nGetMwmMenuItems")
end = source.index("END OF FUNCTION GetMwmMenuItems", start)
region = source[start:end]

assert "textList && nItems > 0 && textList[0] != NULL" in region
assert "textProperty.nitems == 0" in region
print("PASS MWM menu conversion validates returned text entries")
