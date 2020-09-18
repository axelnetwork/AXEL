#!/usr/bin/env python3
# Copyright (c) 2017 The Bitcoin Core developers
# Copyright (c) 2020 The PIVX developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""Test debug logging."""

import os

from test_framework.test_framework import PivxTestFramework


class LoggingTest(PivxTestFramework):

    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True

    def run_test(self):
        # test default log file name
        assert os.path.isfile(os.path.join(self.nodes[0].datadir, "regtest", "debug.log"))
        self.log.info("Default filename ok")

        # DELETED for AXEL


if __name__ == '__main__':
    LoggingTest().main()
