# GeoUtils box-width ordering

The geometry allocator now uses a proper three-way comparator when sorting
boxes by width. The previous boolean result could report only equality or
“greater,” violating `qsort`'s comparator contract and producing unstable
layout ordering.
