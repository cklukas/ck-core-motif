# Container selection callback allocation guards

Container selection callbacks now tolerate allocation failure. Single-item
callbacks report zero selected items when their temporary array cannot be
allocated, and multi-item collection rejects oversized requests, checks the
allocation, and frees an incomplete result before returning.
