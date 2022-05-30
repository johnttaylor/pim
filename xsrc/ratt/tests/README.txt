To run the tests, do the following:

1) In separate DOSBox/console, launch the mock_uut.py script
2) in the tests/ directory (this directory) run the following:
..\bin\ratt.py --input test_suite.ratt --path1 patha --path2 pathb --win "E:\Program Files (x86)\PuTTY\plink.exe" -telnet localhost -P 5002


Notes for when in interactive mode
- help('rattlib')   # -->list of 'standard' ratt commands/modules
- help(man)			# -->help on the rattlib.man command
