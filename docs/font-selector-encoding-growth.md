# FontSelector family-encoding growth

Each discovered font family keeps a dynamically grown table of encoding
quarks. The table grows in `NUM_BUTTONS` entries when a family is updated.

Growth now checks signed and byte-size limits, allocates through a temporary
pointer, and commits the new pointer and capacity only after success. A failed
family update leaves all previously recorded encodings intact.
