# DataField wide callback bounds

Wide modify-verify callbacks control the character count copied back into a
DataField.  The callback result is now checked before computing the terminating
element and allocation size, preventing `INT_MAX + 1` signed overflow and
rejecting buffers wider than Xt's `Cardinal` allocator can represent.

Normal callback results and their conversion behavior are unchanged.
