# XmString ASN.1 conversion failure cleanup

Non-optimized ASN.1 conversion now commits rendition counts only after their
arrays grow successfully. Separator conversion also checks its line and entry
allocations. Any failure releases rendition scratch storage and the partially
built string before returning `NULL`.
