### To run the tests on Windows host, do the following:

1. In separate DOSBox/console, launch the `mock_uut.py` script (located in the `ratt\bin` directory).
2. In the `tests/` directory (this directory) run the following:
`..\bin\ratt.py --input ts_mytests.py --path1 patha --path2 pathb --win "c:\Program Files\PuTTY\plink.exe" -telnet localhost -P 5002`


### Notes for when in interactive mode
* `help('rattlib')`   # -->list of 'standard' ratt commands/modules
* `help(man)`			# -->help on the rattlib.man command
