# DataField wide insertion allocation

The DataField keyboard insertion path converts multibyte input to wide text
before replacement.  Its temporary wide buffer now checks signed length,
wide-character multiplication, Xt's allocation width, and the allocation
result before conversion.  Failure restores the insertion point and returns
without dereferencing null storage.

The focused source contract is checked by
`tests/automated/check-datafield-wide-insertion-allocation.py`.
