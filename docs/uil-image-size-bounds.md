# UIL icon payload sizing

UIL icon payload sizing previously shifted width and multiplied by height in
`int` arithmetic. Large or malformed dimensions could wrap the computed byte
count before the resource context was resized.

Icon row and payload sizes now use `size_t`, then pass through the checked
resource-size helper. Color-table descriptor additions use the same guard.
Valid icon layouts are unchanged; unrepresentable payloads stop emission via
the existing internal error path. The regression
`tests/automated/check-uil-image-size-bounds.py` covers the checked calculation.
