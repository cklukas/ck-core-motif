# TextField replacement growth capacity

TextField replacement and preedit paths now calculate the resulting character
count and allocation size with checked `size_t` arithmetic.  Negative callback
lengths, underflow in replacement ranges, multiplication overflow, and
capacities that cannot be represented by the toolkit's unsigned allocation
field are rejected before `XtRealloc` is called.  Byte and wide-character
storage use the same checked growth policy, including the terminator.

The source-contract regression is
`tests/automated/check-textfield-growth-capacity.py`.
