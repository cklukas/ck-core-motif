# Outline top-node allocation

Outline initialization enlarges the hierarchy top-node record to its outline
constraint type.  It now uses a temporary realloc pointer and leaves the
existing hierarchy pointer published when allocation fails, avoiding an
immediate null dereference during initialization.
