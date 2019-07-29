#!/usr/bin/env python

# Copyright (C) 2019, Jian Lin
# License: GPL v3

import sys

if len(sys.argv) <= 2:
    print("Usage: python RuleWarn.py [from addr] [to addr 1] ...")
    exit(1)

from_addr = sys.argv[1]
to_addr = sys.argv[2:]

print('RuleWarn: From address: %s' % from_addr)
print('RuleWarn: To addresses: %s' % to_addr)

if from_addr.lower() == 'me@example.com':
    for addr in to_addr:
        if not addr.lower().endswith('@example.com'):
            print('RuleWarn: Sent to non-company address: %s' % addr)
            exit(2)

exit(0)
