# CascadeButton resource-list merge guards

CascadeButton class initialization merges the LabelG and CascadeButton
resource arrays into one Xt resource list. The existing native `size_t` check
now also enforces Xt's `Cardinal` limit before the merged list is allocated,
keeping the class initialization safe on platforms where those widths differ.

The focused source-contract check is
`tests/automated/check-cascadebg-resource-merge.py`.
