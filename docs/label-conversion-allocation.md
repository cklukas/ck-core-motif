# Label conversion allocation guards

Label and LabelGadget drag conversion now checks target-list, pixmap,
background, and foreground result allocations. Allocation failure reports
`XmCONVERT_REFUSE` before any result is dereferenced or published.

The focused regression is
`tests/automated/check-label-conversion-allocation.py`.
