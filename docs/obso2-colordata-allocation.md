# Legacy color-data table allocation

The compatibility color-data cache grows a table of `XmColorData` pointers.
Its capacity check now covers both `size_t` and Xt's `Cardinal` allocation
range before multiplying the entry count by the pointer size.  A failed resize
still leaves the existing cache unchanged.
