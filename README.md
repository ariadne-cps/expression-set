# Ariadne Expression Set

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Unix Status](https://github.com/ariadne-cps/expression-set/actions/workflows/unix.yml/badge.svg)](https://github.com/ariadne-cps/expression-set/actions/workflows/unix.yml)
[![Windows Status](https://github.com/ariadne-cps/expression-set/actions/workflows/win.yml/badge.svg)](https://github.com/ariadne-cps/expression-set/actions/workflows/win.yml)
[![Sanitizers Status](https://github.com/ariadne-cps/expression-set/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/ariadne-cps/expression-set/actions/workflows/sanitizers.yml)
[![Coverage Status](https://github.com/ariadne-cps/expression-set/actions/workflows/coverage.yml/badge.svg)](https://github.com/ariadne-cps/expression-set/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/gh/ariadne-cps/expression-set/branch/main/graph/badge.svg)](https://codecov.io/gh/ariadne-cps/expression-set)

Ariadne Expression Set is the standalone C++20 layer for sets defined by symbolic real-variable bounds and predicates. It provides named-variable intervals, symbolic boxes, expression-defined constraint sets, and conversions to Euclidean and validated set representations.

## Layout

Public headers live under `include/expression-set`, implementations under `src`, C++ tests under `tests`, Python bindings and tests under `python`, and a Python tutorial under `python/tutorials`.

The umbrella header is `include/ariadne-expression-set.hpp`.

## Dependencies

Expression Set depends directly on:

- [ariadne-cps/symbolic](https://github.com/ariadne-cps/symbolic), included as Git submodule `ss/sym`.
- [ariadne-cps/set](https://github.com/ariadne-cps/set), included as Git submodule `ss/set`.
- [ariadne-cps/configuration](https://github.com/ariadne-cps/configuration), included as Git submodule `ss/cfg` for shared build configuration.

Symbolic provides named variables, expressions, predicates and spaces. Set provides the computational set representations and their solving dependencies. Function and lower-level Ariadne libraries are supplied transitively. Symbolic does **not** depend on Expression Set or Set.

## Build

```bash
git clone --recurse-submodules https://github.com/ariadne-cps/expression-set.git
cd expression-set
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel --target tests
ctest --test-dir build --output-on-failure
```

For a release build and installation:

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel --target ariadne-expression-set bindings
cmake --install build-release --prefix /path/to/install
```

## Python bindings

With Python 3.11 or newer and a compatible development environment, the standalone build produces the `pyariadne` extension by combining Expression Set bindings with the transitive Set and Symbolic bindings and their lower-level dependencies.

The extension installs under the chosen prefix (`lib/pythonX.Y/site-packages` on Unix, `Lib/site-packages` on Windows), not into a global Python installation.

## Tutorials

- C++ examples: `tests/test_expression_set.cpp`.
- Python: `python/tutorials/tutorial_expression_set.py`.

## Contribution guidelines

If you would like to contribute to Ariadne Expression Set, please contact the developer:

- Luca Geretti <luca.geretti@univr.it>

## License

Ariadne Expression Set is released under the GNU General Public License v3.0 (or later where individual source files specify it).
