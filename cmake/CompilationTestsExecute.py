#!/usr/bin/env python

# copyright Nils Deppe 2017
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

import re
import argparse as ap
import subprocess

def parse_args():
    parser = ap.ArgumentParser(
        description='Run command expecting some regular expression. If the '
        'regex is found, returns 0, else 1',
        formatter_class=ap.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        '--command', type=str, required=True,
        help='The build command')
    parser.add_argument(
        '--regex', type=str, required=True,
        help='Expected regular expression to match')
    return vars(parser.parse_args())


def run_command_and_check_output(args):
    try:
        command = args['command'] + " 2>&1"
        print(command)
        subprocess.check_output(command, shell=True)
    except subprocess.CalledProcessError as e:
        if re.match(args['regex'], e.output.decode('utf-8'), re.DOTALL):
            return 0
        
        print("Failed to find regular expression:\n%s" % args['regex'])
        print("\nwith command:\n%s" % args['command'])
        print("\nThe output of the command was:\n%s" % e.output.decode('utf-8'))
        return 1
    
    print("Compilation test failed because compilation worked. Command:\n%s"
          % args['command'])
    return 1


if __name__ == '__main__':
    exit(run_command_and_check_output(parse_args()))

