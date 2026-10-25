# Cached XmString constructor safety

`XmStringDirectionCreate` and `XmStringSeparatorCreate` now check cached
representation allocations before initialization. Their rare reference-count
rollover paths also release the process lock before calling `XmStringFree` and
recursing, avoiding recursive-lock deadlocks.
