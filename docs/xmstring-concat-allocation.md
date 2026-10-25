# Optimized XmString concatenation allocation

When two compatible strings can remain optimized, `XmStringConcatAndFree`
allocates one combined representation. The function now checks that allocation
before writing its header or text. Failure releases both owned inputs,
unlocks the process state, and returns `NULL` without dereferencing a failed
allocation.
