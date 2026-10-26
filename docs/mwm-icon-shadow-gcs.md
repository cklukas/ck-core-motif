# MWM icon shadow graphics contexts

Icon pixmap construction allocates highlight GCs for the top and bottom
bevels. The constructor now validates both results and releases the temporary
pixmap on failure, preventing null-GC drawing requests and preserving the
existing null-pixmap failure contract.
