# Outline top-node allocation

Outline initialization enlarges the hierarchy top-node record to its outline
constraint type. It now uses a temporary realloc pointer and raises an Xt error
when allocation fails, rather than leaving a widget with a hierarchy node of an
incompatible size. Successful initialization remains unchanged.
