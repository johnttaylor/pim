""" Global variables
"""

#
g_script_paths            = []
g_utils_import_dictionary = {}
g_utils_import_paths      = {}
g_uut                     = None
g_newline                 = "\n"
g_passed                  = 0
g_failed                  = 1
g_verbose_logs            = False;
g_debug_logs              = False;
g_entry_banner            = "{}STARTING:  {} ..."
g_exit_banner             = "{}COMPLETED: {}."
indent_prefix             = ".."
g_ratt_file_extension     = ".py"
g_ignore_pyfiles          = ['mytoolchain.py', 'nqbp.py', 'tca.py', 'a.py', 'aa.py', 'b.py', 'bb.py']