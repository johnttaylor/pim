#!/usr/bin/python3
"""Output class"""

import os


class Printer:
    def __init__(self ):
        self.verbose_on = False
        self.debug_on = False

    def output(self,line):
        print(line)

    def verbose(self, line):
        if (self.verbose_on):
            self.output(line)

    def debug(self, line):
        if (self.debug_on):
            self.output(line)
    
    def enable_debug(self):
        self.debug_on = True
    
    def enable_verbose(self):
        self.verbose_on = True
    

