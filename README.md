# Ariadne Expression Set

Ariadne Expression Set provides named-variable intervals, symbolic boxes, predicate-defined constraint sets, and conversions to Euclidean and validated set representations.

## Dependencies

- [symbolic](https://github.com/ariadne-cps/symbolic) for expressions, predicates, and spaces.
- [set](https://github.com/ariadne-cps/set) for geometric set implementations.
- [configuration](https://github.com/ariadne-cps/configuration) for CMake infrastructure.

Function is supplied transitively. Symbolic does not depend on expression-set or the solver stack.

## Build and test

```sh
git clone --recurse-submodules https://github.com/ariadne-cps/expression-set.git
cmake -S expression-set -B expression-set/build -DCMAKE_BUILD_TYPE=Debug
cmake --build expression-set/build --target tests
ctest --test-dir expression-set/build --output-on-failure
```

GNU GPL v3.
