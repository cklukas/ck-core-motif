# TreeView geometry regression contracts

TreeView row positions are calculated through a `long long` intermediate and
clamped before they become X11 coordinates. Disclosure indicators use a
nine-pixel raster box centered within the configured row height, keeping the
crosshair symmetric for both compact and tall rows.

The source contracts are checked with:

```sh
python3 tests/automated/check-tree-view-coordinate-bounds.py
python3 tests/automated/check-tree-view-disclosure-geometry.py
```
