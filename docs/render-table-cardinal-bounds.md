# Render-table allocation bounds

Render-table creation, merge, append, and copy paths now check Xt's
`Cardinal` allocation limit in addition to host `size_t` overflow.  Merge
count addition is checked before subtraction from the inline rendition
capacity, preventing truncation or wrapped table sizes on 64-bit hosts.

The focused source-contract check is:

    python3 tests/automated/check-render-table-cardinal-bounds.py
