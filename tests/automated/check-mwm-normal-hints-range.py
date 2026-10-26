from pathlib import Path

source = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")
helper_start = source.index("WmSizeHintsValuesFit")
helper_end = source.index("\n}\n", helper_start) + 2
helper = source[helper_start:helper_end]
assert "INT_MIN" in helper and "INT_MAX" in helper
normal_start = source.index("GetNormalHints")
normal_end = source.index("END OF FUNCTION GetNormalHints", normal_start)
normal = source[normal_start:normal_end]
assert "WmCurrentSizeHintsValuesFit(property)" in normal
assert "WmSizeHintsValuesFit(property)" in normal
assert "PROP_SIZE_HINTS_ELEMENTS - 3" in normal
assert "sizeHints.x = (int)property->x;" in normal
print("PASS MWM normal hints reject out-of-range property values")
