## SSM Library

The problems of the unsafe C string functions such as the infamous
`strcpy()` and `strcat()` are well-known, causing buffer overflows
and leading to multiplesecurity vulnerabilities. The SSM library is
a safe and reliable alternative the old C string library.

| Subdirectory | Content |
| ------------ | ------- |
| doc          | Documentation (requires Texinfo 5.0 or higher). |
| include      | Public API of the library (one single header file). |
| src          | Source code of the library. |
| utests       | Unitary tests (requires CUnit 2.1-2 or higher). |
| demos        | Demo applications. |
| msvc2010     | Project files for Microsoft Visual C++ 2010. |
| msvc2013     | Project files for Microsoft Visual C++ 2013. |
| msvc2015     | Project files for Microsoft Visual C++ 2015. |

### License

The SSM library is distributed under the terms of the GNU Lesser General
Public License version 2.1. See file `COPYING.txt`.

### Build

The SSM library is built as a static library and a shared library.
See file `BUILDING.txt`.
