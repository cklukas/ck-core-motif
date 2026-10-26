# Font Selector encoding-list count

Font Selector initialization duplicates the configured encoding list. Its
counting loop now advances once per encoding instead of incrementing twice,
which avoids needless over-allocation and signed counter growth. The resulting
array size is checked against both native and Xt allocation limits before it is
created.

The focused source-contract check is
`tests/automated/check-font-selector-encoding-count.py`.
