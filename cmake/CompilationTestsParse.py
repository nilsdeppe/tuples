#!/usr/bin/env python

# copyright Nils Deppe 2017
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

import re

def parse_source_file(file_name):
    file_string = open(file_name, "r").read()

    # The FILE_IS_COMPILATION_TEST macro expands to a static_assert(false) so
    # that the translation unit cannot be compiled unintentionally
    if file_string.find("\nFILE_IS_COMPILATION_TEST") == -1:
        print("All compilation test source files must end in "
              "'FILE_IS_COMPILATION_TEST'")
        exit(1)

    test_regex = re.compile("\n#ifdef COMPILATION_TEST_([A-Za-z_0-9]+)\n"
                            "(.*?)#endif", re.DOTALL)
    for (test_name, test_contents) in re.findall(test_regex, file_string):
        # parse tags
        test_tags = re.search("\/\/ \[\[TAGS:(.*?)\]\]", test_contents,
                              re.DOTALL)
        if not test_tags:
            print("ERROR: Could not find any TAGS for test: %s\n"
                  "The tags should be specified as:\n\n"
                  "#ifdef COMPILATION_TEST_%s\n// [[TAGS: tag1, tag2]]\n"
                  % (test_name, test_name))
            exit(1)
        test_tags = test_tags.group(1).strip().lower().split(',')
        test_tags_string = ''.join(test_tags)
        
        # Parse regex for each compiler
        compiler_and_message_regex = re.compile("\/\/ \[\[(COMPILER:.*?)\]\]",
                                                re.DOTALL)
        for compiler_and_message in re.findall(compiler_and_message_regex,
                                               test_contents):
            compiler_and_regex = re.search("COMPILER: (.*?)REGEX: (.*)",
                                           compiler_and_message, re.DOTALL)

            compilers = compiler_and_regex.group(1).strip().split(',')
            regex_to_find = compiler_and_regex.group(2).replace("\n//", '')
            if compilers[0] != "all":
                for i in range(len(compilers)):
                    compiler_and_version = compilers[i].split(':')
                    open("COMPILATION_TEST_%s.%s"  %
                         (test_name, compiler_and_version[0]), "a").\
                         write((" TAGS: %s VERSION: %s REGEX: %s") %
                               (test_tags_string,
                                compiler_and_version[1] if
                                len(compiler_and_version) > 1 else "0.0.0",
                                regex_to_find))
            else:
                open("COMPILATION_TEST_%s.all" % test_name, "w").\
                     write("TAGS: %s REGEX: %s" %
                           (test_tags_string, regex_to_find))

if __name__ == '__main__':
    import sys
    source_files = sys.argv[1:]
    for file in source_files:
        parse_source_file(file)
