# UIL color-table count bounds

UIL color-table construction now bounds the number of list items before
incrementing its signed counter or multiplying by `sizeof(sym_color_element)`.
An unrepresentable table is diagnosed as too large and converted to the normal
error value instead of wrapping into an undersized allocation.
