# Transfer context hash portability

Transfer conversion contexts now hash the display pointer and selection atom
with bounded `uintptr_t` arithmetic before converting to the toolkit hash type.
This avoids signed overflow and implementation-dependent pointer-to-`long`
truncation on 64-bit platforms.
