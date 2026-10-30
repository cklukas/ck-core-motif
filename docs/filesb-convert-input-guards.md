---
nav_exclude: true
---
# FileSelectionBox conversion callback guards

FileSelectionBox drag/selection conversion now rejects missing callback data,
missing client widgets, and client widgets of the wrong class before reading
search procedures or the display. Unsupported dispatch reports conversion
refusal without touching partial state.
