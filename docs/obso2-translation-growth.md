# Legacy translation-buffer growth

The compatibility `_XmGetRealXlations` helper grows a shared translation
buffer.  When doubling would cross Xt's `Cardinal` allocation limit, it now
uses the exact required size instead of rejecting an otherwise representable
translation.  The existing allocation-failure path still preserves the old
buffer and returns failure cleanly.
