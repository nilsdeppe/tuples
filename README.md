[![license](http://img.shields.io/badge/license-boost-blue.svg)](https://github.com/nilsdeppe/tuples/blob/master/LICENSE.md)
[![Standard](https://img.shields.io/badge/c%2B%2B-11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![Build Status](https://travis-ci.org/nilsdeppe/tuples.svg?branch=master)](https://travis-ci.org/nilsdeppe/tuples)

## Description

This library is designed to provide standard-library-quality
`tagged_tuple` and `tuple` implementations compliant with the C++17 standard,
but fully C++11 compliant. The implementations are efficient both in
compilation and runtime resources, outperforming compilation times of libstdc++
and are on par with libc++ compilation times. Tuples with thousands of elements,
and tagged tuples with millions of elements are supported.

## Requirements:
- C++11 compatible compiler
- GCC 4.8 or newer (GCC 4.7 does not properly support parameter pack expansion)
- Clang 3.5 or newer tested (3.1 and newer should work)
- Intel C++ compiler 14 or newer
- MSVC currently unknown
- XCode v6.4 and newer tested

## Doxygen Documentation

The generated Doxygen documentation is automatically deployed to gh-pages
[here](https://nilsdeppe.github.io/tuples/).

## Acknowledgements

- libc++ for design inspiration of the tuple
- [This](https://stackoverflow.com/questions/13065166/c11-tagged-tuple)
stackoverflow question, and Jozsef Bakosi for providing the original idea
- The Natural Sciences and Engineering Research Council of Canada (NSERC)
for funding throughout the initial development
