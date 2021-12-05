#!/usr/bin/python3
"""Output class"""

import os
import multiprocessing
from . import utils


class Printer:
    def __init__(self, mutex=None, log_file_name='make.log', start_new_file=False):
        self.fname = log_file_name
        self.mutex = mutex
        self.verbose_on = False
        self.debug_on = False

        # Create an empty file when asked
        if (start_new_file):
            open(self.fname, 'w').close()

    def remove_log_file(self):
        utils.delete_file(self.fname)

    def output(self,line):
        self.mutex.acquire()
        print(line)
        with open(self.fname, 'a') as f:
            f.write(line)
            f.write(os.linesep)
        f.close()
        self.mutex.release()

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
    

