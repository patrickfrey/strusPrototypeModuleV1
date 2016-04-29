# Strus Prototype Module V1

This module implements a demo weighting formula which can be configured
in a simple language. The language contains:
- Constants: both integer and floating point constants like `1` or `2.44`
- Functions:
  - unary functions: `log10`, `log` (logarithm basis `e`), `-` (negative),
    `sgn` (signum function), `sqrt` (square root)
  - binary functions:
    `+` (addition), `-` (substraction), `*` (multiplication), `/` (division),
    `pow` (power), `min` (minimum), `max` (maximum), `minwinsize`, `minwinpos`
- Variable:
  - number variables:
    - predefined variables: `ff`, `df`, `weight`
    - metadata variables: `doclen`, `norm`, etc. must be defined in the storage
  - feature set variables: passed as feature expression to the system, e.g. `feat`
    representing the bag of words
- Iterator: `<feature set variable, binary function, constant>{expression}`
  meaning the set of features is iterated and the binary function applied to
  all elements in `expression`, the first argument to the `binary function`
  being an integer or floating point `constant` representing the null element
  of the function, e. g.

  `<feat,+,0>{ff}` adds up all values of `ff` of feature set `feat` starting with
  `0`.

Notes:
- Operators and functions have no priority, so normal brackets (`(` and `)`) must
  be used.

