#!/usr/bin/python3
"""
Tool for parsing the GCC MAP file
===============================================================================
usage: map [options] <map-file>
       

Arguments:
    <map-file>          The GCC MAP file that is output by the Linker  

Options:
    -r                  Sort by RAM size (the default is sort by FLASH size)
    -o OBJ              Filter by a specific object file OBJ
    -a LIB              Filter by a specific library file LIB
    --ram ADDR          Starting address (in Hex) of RAM [Default: 20000000]
    -v                  Be verbose
    -h, --help          Display help
    

Notes:
    - The script is NOT 100% accurate with the numbers/totals it data mines
      from the MAP file.  However, it is close enough for the purpose of 
      understanding relative 'footprints' of the modules with respect to each 
      other.
"""


from docopt.docopt import docopt
import sys, os
import re

MAP_VERSION = '0.1'

be_verbose  = False


#-------------------------------------------------------------------------------
def fix_split_lines( fname ):
    lines = []
    issplit = False;
    label   = ''
    with open( fname ) as inf:
        for line in inf:
            l = line.strip()
            if (len(l) == 0 ):
                continue
            if ( issplit ):
                l = label + ' ' + l
                issplit = False
            else:
                tokens = l.split(' ')
                if ( len(tokens) == 1 ):
                    label = tokens[0]
                    issplit = True

                    continue
            lines.append(l)

    return lines

def add_entry( addr, lines, tuple ):
    if ( addr in lines ):
        # When duplicate address - keep the largest size
        if ( lines[addr][2] < size ):
            lines[addr] = tuple
    else:
        lines[addr] = tuple

#-------------------------------------------------------------------------------
# BEGIN
if __name__ == '__main__':
    # Parse command line
    args = docopt(__doc__, version=MAP_VERSION  )
    if ( args['-v'] ):
        be_verbose = True
    start_of_ram = int(args['--ram'],16)
    
    # Get the entries we care about
    sum = 0
    lines = {}
    p  = re.compile( r"^(.*) 0x(.*) .*0x(.*) (.*\.a\()(.*)") 
    p2 = re.compile( r"^(.\*fill\*) .*0x(.*) .*0x(.*).*") 
    p3 = re.compile( r"^(.*) 0x(.*) .*0x(.*) (.*.o$)") 
    p4 = re.compile( r"^(\._user_heap_stack) 0x(.*) .*0x(.*) (load address)") 
    rawlines = fix_split_lines( args['<map-file>'] )
    rodataentry = None
    for line in rawlines:
        # Search for map entries (including the fill/pad byte allocations)
        line = line.strip()
        matched = False
        label,addr_s,size_s,lib,obj = (None,None,None,None,None)
        m = p.match( line )
        if ( m != None ):
            label,addr_s,size_s,lib,obj = m.groups()
            matched = True
        else:
            m2 = p2.match(line)
            if ( m2 != None ):
                label, addr_s, size_s = m2.groups()
                lib = "*FILL*"
                obj = "*FILL*"
                matched = True
            else:
                m3 = p3.match(line)
                if ( m3 != None ):
                    label, addr_s, size_s, lib = m3.groups()
                    obj = lib 
                    matched = True
                else:
                    m4 = p4.match(line)
                    if ( m4 != None ):
                        label, addr_s, size_s, lib = m4.groups()
                        lib = "heap-stack"
                        obj = lib 
                        matched = True

        # Store the matched record (while filter out duplicate addresses)
        if ( matched ):
            label = label.strip()
            if ( label == None or label == ""):
                label = "---"
            addr  = int(addr_s.strip(),16)
            size  = int(size_s.strip(),16)
            lib   = lib.strip().rstrip('(')
            obj   = obj.strip().rstrip(')')
            tokens = line.split(' ')

            add_entry(addr, lines, (label,addr,size,lib,obj) )

    # Process filtered lines
    libs = {}
    for k,v in lines.items():
        label,addr,size,lib,obj = v
        
        # Filter by library or object when requested
        if ( args['-o'] and args['-o'] != obj ):
            continue
        if ( args['-a'] and args['-a'] != lib ):
            continue

        # Skip all address zero stuffs
        if ( addr == 0 ):
            continue

        # Skip all things 'debug' ish
        if ( label.startswith(".debug" ) ):
            continue

        # Determine type based on address
        size_ram   = 0
        size_flash = 0
        if ( addr < start_of_ram ):
            size_flash = size
            dtype = "text"
        else:
            size_ram = size
            dtype = "data"

        # Add the entry to the dictionary (the key is the library name)
        if ( size > 0 and addr > 0 ):
            entry   = (label, addr, size, obj, dtype)
            if ( lib in libs ):
                entries,sflash,sram = libs[lib]
                size_ram   = sram + size_ram
                size_flash = sflash + size_flash
                entries.append( entry )
                libs[lib] = (entries, size_flash, size_ram) 
            else:
                entries = []
                entries.append( entry )
                libs[lib] = (entries, size_flash, size_ram )
                
    
    # Sort by size
    size_index = 1 if not args['-r'] else 2
    slibs = dict(sorted(libs.items(), key=lambda item: item[1][size_index]))

    # Output the results
    totalf = 0
    totalr = 0
    for k,v in slibs.items():
        if ( be_verbose == False ):
            totalf = totalf + v[1]
            totalr = totalr + v[2]
        print( str(v[1]).rjust(7), str(v[2]).rjust(7), " ", k )
        if ( be_verbose ):
            for e in v[0]:
                if ( e[4] == 'text'):
                    totalf = totalf + e[2]
                if ( e[4] == 'data'):
                    totalr = totalr + e[2]
                print(f"           {e[4].ljust(5)}  0x{e[1]:08x}  {e[2]:05}  {e[0]}  {e[3]}" )
    
    print("")
    print(f"TOTAL FLASH: {totalf:8} {(totalf/1024):8.1f} KB" )
    print(f"TOTAL RAM:   {totalr:8} {(totalr/1024):8.1f} KB" )


            