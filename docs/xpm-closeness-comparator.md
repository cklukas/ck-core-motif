# Xpm color closeness ordering

Color candidate sorting now compares `long` closeness values relationally
instead of subtracting and narrowing the result to `int`. Extreme color
distances therefore retain a valid `qsort` ordering without overflow.
