#!/usr/bin/env python3
"""Check GeoUtils matrix sizing and allocation failure handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmGeoMatrixAlloc(")
end = source.index("void\n_XmGeoMatrixFree", start)
body = source[start:end]
assert "size_t          matrixRecSize" in body
assert "SIZE_MAX / layoutRecSize" in body
assert "SIZE_MAX / kidGeoRecSize" in body
assert "totalSize > (size_t)UINT_MAX" in body
assert "if (geoSpecPtr == NULL)" in body
free_start = source.index("void\n_XmGeoMatrixFree", start)
free_body = source[free_start:source.index("/****************************************************************", free_start)]
assert "if (geo_spec == NULL)" in free_body
for source_name in ("SelectioB.c", "FileSB.c", "MessageB.c"):
    widget_source = (Path(__file__).parents[2] / "lib" / "Xm" / source_name).read_text(
        encoding="utf-8"
    )
    assert widget_source.count("if (geoSpec == NULL)") >= 1
assert source.count("if (geoSpec == NULL)") >= 3
for signature in (
    "_XmGeoMatrixGet(\n        XmGeoMatrix",
    "_XmGeoMatrixSet(\n        XmGeoMatrix",
    "_XmGeoAdjustBoxes(\n        XmGeoMatrix",
):
    operation_start = source.index(signature)
    operation_end = source.index("/****************/", operation_start) + 120
    assert "if (geoSpec == NULL)" in source[operation_start:operation_end + 20]
arrange_start = source.index("_XmGeoArrangeBoxes(\n        XmGeoMatrix")
arrange_end = source.index("/****************/", arrange_start)
assert "geoSpec == NULL || pW == NULL || pH == NULL" in source[
    arrange_start:arrange_end + 200
]

print("GeoUtils matrix allocation guards passed")
