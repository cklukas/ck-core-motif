# Container integer comparison

Container reorder sorting now compares position indices relationally instead
of subtracting them. This preserves the same ordering while avoiding signed
integer overflow when malformed or extreme indices reach the comparator.
