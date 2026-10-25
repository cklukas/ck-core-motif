# Tabbed-stack list allocation bounds

The legacy tabbed-stack list now checks both native `size_t` arithmetic and
Xt's `Cardinal` allocation width when copying or growing its attribute array.
Unrepresentable requests retain the existing `NULL`/`XmTAB_NOT_FOUND` failure
results.
