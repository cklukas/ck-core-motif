# Container CELLS allocation bounds

The CELLS ideal-size path now checks both `size_t` and Xt `Cardinal` product
limits before allocating one geometry record per child. An unrepresentable
array is rejected before `XtCalloc`, avoiding truncation in the allocator.

The focused source contract is checked by
`tests/automated/check-container-cells-allocation.py`.
