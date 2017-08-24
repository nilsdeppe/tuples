[![license](http://img.shields.io/badge/license-boost-blue.svg)](https://github.com/nilsdeppe/tuples/blob/master/LICENSE.md)
[![Standard](https://img.shields.io/badge/c%2B%2B-11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)

## Description

This library is designed to provide standard-library-quality
`tagged_tuple` and `tuple` implementations compliant with the C++17 standard,
but fully C++11 compliant. The implementations are efficient both in
compilation and runtime resources, outperforming compilation times of libstdc++
and are on par with libc++ compilation times. Tuples with thousands of elements,
and tagged tuples with millions of elements are supported.

## Requirements:
- C++11 compatible compiler
- GCC 4.7 or newer
- Clang 3.1 or newer
- Intel C++ compiler 14 or newer

## Acknowledgements

- libc++ for design inspiration of the tuple
- [This](https://stackoverflow.com/questions/13065166/c11-tagged-tuple)
stackoverflow question, and Jozsef Bakosi for providing the original idea
- The Natural Sciences and Engineering Research Council of Canada (NSERC)
for funding throughout the initial development
