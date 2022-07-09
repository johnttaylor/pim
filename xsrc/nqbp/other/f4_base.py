#!/usr/bin/python3
#=============================================================================
#
import sys   
import os
import subprocess
from datetime import datetime
#
from nqbplib.docopt.docopt import docopt
from nqbplib import utils
from other import utils2



# 
usage = """ 
f4 - Forms For files.  Generates a text file from template file(s) and dynamic 
     user input.
===============================================================================
Usage: f4 [options] <tname> [-o OUTFILE] -f MAPFILE
       f4 [options] <tname> [-o OUTFILE] map <entries>...
       f4 --guide


Arguments:

  <tname>          Name of the template file to expand.
  -o OUTFILE       Name of the output file that contains the result of the
                   expansion.  If no output is specified, then the output
                   is STDOUT.
  map              Keyword that marks the start of Substituion map entries.
  <entries>        The values for the Substitution Map.  Each entry is 
                   seperated by white space.  Quotes can be used to define
                   entries that contain whitespace.
  -f MAPFILE       Name of the file that contains the entries for the 
                   Substitution Map.  The format of the file is that each
                   line is entry in the Map.  No comments or blank lines
                   are allowed in the file.                  
Options:

  -d DELIMITER     Sets the delimiter for seperating multi-value map entry.  
                   [Default: ,]  
  -r ROOTPATH      Sets the 'root path' for the _RELPATH token. The default
                   is no root path, i.e. root path = current working directory.
  --local PATH     Sets the local search path when trying to open/find the  
                   specified template file and/or subscript template files. See
                   the Notes section for more details.
  --global PATH    Sets the global search path when trying to open/find the
                   specified template file and/or subscript template files. See
                   the Notes section for more details.
  -z, --outcast    Uses 'Outcast' notation for all path options. A leading '//'
                   specifies the root of the Outcast workspace and a leading
                   '/' specified the root of the current package.
  -v               Be verbose
  -g               Display f4 debug information
  -?, --guide      Display user guide.
  -h, --help       Display command help.
        
   
NOTES:
    o DO NOT USE tabs in your template files.
    o The search order for template files is as follows:
        1) The current working directory is searched first
        2) Then the --local PATH (if there is one)
        3) Finally the --global PATH (if there is on)
    o The environment variables - F4_LOCAL_PATH and F4_GLOBAL_PATH - can be use
      to set the default values for the --local and --global options.
      directory (unless the -z option is used).
    o The environment variable - F4_ROOT_PATH - can be used to the set default
      value for the -r option.
              
"""

#
import subprocess
import re
import sys

# globals
WRKSPACE_ROOT = ''
PKG_ROOT      = ''



# Hardwire 
#------------------------------------------------------------------------------
# Parse command line
def run( argv, default_local_path, default_global_path, default_root_path, use_outcast=False ):
    global WRKSPACE_ROOT
    global PKG_ROOT

    # Process command line args...
    args = docopt(usage, version="0.0.1" )

    # Debug
    if ( args['-g'] ):
        print(args, '\n')
    
    # Set default for --outcast option
    if ( use_outcast ):
        args['--outcast'] = True
           
    # Find the Package & Workspace root
    if ( args['--outcast'] ):
        WRKSPACE_ROOT, PKG_ROOT = utils2.set_pkg_and_wrkspace_roots()

    # Print guide
    if ( args['--guide'] ):
        print(guide)
        sys.exit()

    # Calculate path for _RELPATH token
    compute_relative_path( args, default_root_path )

    # Set default for --local path option
    if ( not args['--local'] ):
        lpath = os.environ.get('F4_LOCAL_PATH')
        if ( lpath == None ):
            lpath = default_local_path
        args['--local'] = lpath

    # Set default for --global path option
    if ( not args['--global'] ):
        gpath = os.environ.get('F4_GLOBAL_PATH')
        if ( gpath == None ):
            gpath = default_global_path
        args['--global'] = gpath

    # Build the Substitution Map
    smap  = build_map( args )
    iters = Iteration()
   
    # Initial parse of the Template file
    header, content = read_template_file( args['<tname>'], iters, args )

    # Expand the template file
    outfd = utils2.Output( args['-o'] )
    expand_content( header, content, smap, iters, outfd, args )
    outfd.close()

#------------------------------------------------------------------------------
###
def expand_content( header, content, smap, iters, outfd, args ):
    # Verbose
    if ( args['-v'] ):
        print("{}Expanding: {}, iters={} ...".format( iters.get_verbose_indent(), header.get_fname(), iters.get_max_iter_count() ))

    # Debug
    if ( args['-g'] ):
        print("{}{}".format( iters.get_verbose_indent(), header ))
        print("{}{}".format( iters.get_verbose_indent(), smap ))

    loop = True if iters.get_max_iter_count() > 0 else False
    while( loop ):
        # Verbose
        if ( args['-v'] ):
            print("{}  Iteration={}, iter_value={} ...".format( iters.get_verbose_indent(), iters.get_iter_number(), iters.get_iter_value() ))
        
        lnum = 0
        skip = 0
        for l in content:
            if ( skip > 0 ):
                skip -= 1
            else:
                eline, skip = expand_line( l, header, smap, iters, outfd, content, lnum, args )
                if ( eline != None ):
                    outfd.write( eline )
                
            lnum += 1

        loop = iters.next_iteration()

###
def expand_line( line, header, smap, iters, outfd, content, lnum, args ):
    outline = iters.get_indent()
    escaped = False

    # Loop though all characters in the line
    idx   = 0
    count = len(line)
    skip  = 0   
    while( idx < count ):

        # Handle esacping the mark & escape characters
        if ( escaped ):
            outline += line[idx]
            idx     += 1
            escaped  = False
            continue

        elif ( line[idx] == header.get_esc() ):
            escaped  = True
            idx     += 1
            continue
            
        # Check for Subscripting
        if ( subscripting( line, idx, header, smap, iters, outfd, lnum, args ) ):
            return None, skip

        # Check for Inlined subscripting
        skip = inlining( line, idx, header, smap, iters, outfd, content, lnum, args )
        if ( skip > 0 ):
            return None, skip
             
        # Found the mark character
        if ( line[idx] == header.get_mark() ):
            text, consumed_count = substitute_token( line[idx+1:], header, smap, iters, idx, lnum, args )
            outline += text
            idx     += consumed_count + 1
            
        # literal text
        else:
            outline += line[idx]   
            idx     += 1
            
    return outline, skip
    
        
###
def substitute_token( line, header, smap, iters, col, lnum, args ):
    # extract the token
    end = line.find( header.get_mark() )
    if ( end == -1 ):
        sys.exit( "ERROR: Bad token specified. File={}, line# {}, col# {}".format( header.get_fname(), header.get_line_count() + lnum, col+1 ) ) 
    token     = line[:end]
    end_idx   = end
    consumed  = len(token) + 1
    start_idx = 0

    # Housekeeping
    lcmd      = None
    loper     = None
    rcmd      = None
    roper     = None

    # extract the L-Command (if there is one)
    lmark = token.find( '<' )
    if ( lmark != -1 ):
        lcmd      = token[0:1]
        loper     = token[1:lmark]
        start_idx = lmark+1

    # extract the R-Command (if there is one)
    rmark = token.find( '>' )
    if ( rmark != -1 ):
        end_idx = rmark
        rcmd    = token[rmark+1:rmark+2]
        roper   = token[rmark+2:]

 
    # extract the token_id
    token_id = line[start_idx:end_idx]
    
    # Check for literal value
    if ( (token_id.startswith('"') and token_id.endswith('"')) or (token_id.startswith("'") and token_id.endswith("'")) ):
        entry = SMapEntry( token_id[1:-1], None )

    # Check for special tokens
    elif ( token_id.startswith( '_' ) ):
        entry = process_special_token( token_id, iters, args )
       
    # Look-up token id
    else:
        tidx = header.find_token( token_id )
        if ( tidx == -1 ):
            sys.exit( "ERROR: Token - {} - not defined in the template header. File={}, line# {}, col# {}".format( token_id, header.get_fname(), header.get_line_count() + lnum, col+1 ) ) 
        entry = smap.get_entry( tidx )
        if ( entry == None ):
            sys.exit( "ERROR: More Token IDs than Map entries (token id number={}, map count={}). File={}, line# {}, col# {}".format( tidx+1, smap.get_num_entries(), header.get_fname(), header.get_line_count() + lnum, col+1 ) ) 
            
    # apply L/R commands
    new_value = apply_token_commands( entry, iters, lcmd, loper, rcmd, roper, header, args, lnum, col )

    # Return final value
    return new_value, consumed


###
def apply_token_commands( entry, iters, lcmd, loper, rcmd, roper, header, args, lnum, col ):
    # Get value
    raw_value = entry.get_value( iters.get_iter_number() )

    # Process R-Command first    
    if ( rcmd != None ):
        if ( rcmd == '+' ):
            if ( entry.get_count() > 1 ):
                return do_rcmd_concatenate( entry, iters, roper, lcmd, loper, header, args, lnum, col )
            else:
                if ( lcmd != None ):
                    raw_value = process_lcmds( raw_value, lcmd, loper, header, args, lnum, col )
                return raw_value

        elif ( rcmd == '#' ):
            return do_rcmd_count( entry, iters, roper, lcmd, loper, header, args, lnum, col )

        elif ( rcmd == ':' ):
            return do_rcmd_index( entry, iters, roper, lcmd, loper, header, args, lnum, col )

        elif ( rcmd == 'f' ):
            return do_rcmd_first( entry, iters, roper, lcmd, loper, header, args, lnum, col )

        elif ( rcmd == 'F' ):
            return do_rcmd_not_first( entry, iters, roper, lcmd, loper, header, args, lnum, col )

        elif ( rcmd == 'l' ):
            return do_rcmd_last( entry, iters, roper, lcmd, loper, header, args, lnum, col )

        elif ( rcmd == 'L' ):
            return do_rcmd_not_last( entry, iters, roper, lcmd, loper, header, args, lnum, col )

        else:
            sys.exit( "ERROR: Unsupported R-Command:{}. File={}, line# {}, col# {}".format( rcmd, header.get_fname(), header.get_line_count() + lnum, col+1 ) ) 
            
    # No R-Commands -->process L-Commands
    elif ( lcmd != None ):
        return process_lcmds( raw_value, lcmd, loper, header, args, lnum, col )

    # No command specified
    else:
        return raw_value


###
def do_rcmd_concatenate( entry, iters, roper, lcmd, loper, header, args, lnum, col ):
    new_value = entry.get_value( 0 )
    if ( lcmd != None ):
        new_value = process_lcmds( new_value, lcmd, loper, header, args, lnum, col )
    
    for i in range(1, entry.get_count() ):
        new_value += roper
        if ( lcmd != None ):
            new_value += process_lcmds( entry.get_value( i ), lcmd, loper, header, args, lnum, col )
        else:
            new_value += entry.get_value( i )

    return new_value 
 
                   
###
def do_rcmd_count( entry, iters, roper, lcmd, loper, header, args, lnum, col ):
    new_value = str(entry.get_count())
    if ( lcmd != None ):
        new_value = process_lcmds( new_value, lcmd, loper, header, args, lnum, col )

    return new_value 

###
def do_rcmd_index( entry, iters, roper, lcmd, loper, header, args, lnum, col ):
    if ( roper == None ):
        sys.exit( "ERROR: Missing operand for R-Command:{}. File={}, line# {}, col# {}".format( rcmd, header.get_fname(), header.get_line_count() + lnum, col+1 ) ) 

    new_value = entry.get_value( int(roper) )
    if ( lcmd != None ):
        new_value = process_lcmds( new_value, lcmd, loper, header, args, lnum, col )

    return new_value 


###
def do_rcmd_first( entry, iters, roper, lcmd, loper, header, args, lnum, col ):
    if ( iters.get_iter_number() == 0 ):
        return entry.get_value()
    else:
        return ""

###
def do_rcmd_not_first( entry, iters, roper, lcmd, loper, header, args, lnum, col ):
    if ( iters.get_iter_number() != 0 ):
        return entry.get_value()
    else:
        return ""

###
def do_rcmd_last( entry, iters, roper, lcmd, loper, header, args, lnum, col ):
    if ( iters.get_iter_number() == iters.get_max_iter_count() - 1 ):
        return entry.get_value()
    else:
        return ""

###
def do_rcmd_not_last( entry, iters, roper, lcmd, loper, header, args, lnum, col ):
    if ( iters.get_iter_number() != iters.get_max_iter_count() - 1 ):
        return entry.get_value()
    else:
        return ""

    
###
def process_lcmds( raw_value, lcmd, loper, header, args, lnum, col ):

    # Upper case first letter
    if ( lcmd == 'u' ):
        return re.sub('([a-zA-Z])', lambda x: x.groups()[0].upper(), raw_value, 1)

    # Upper case ALL leters
    elif ( lcmd == 'U' ):
        return raw_value.upper() 

    # Lower case first letter
    elif ( lcmd == 'l' ):
        return re.sub('([a-zA-Z])', lambda x: x.groups()[0].lower(), raw_value, 1)

    # Lower case ALL leters
    elif ( lcmd == 'L' ):
        return raw_value.lower()

    # Add value
    elif ( lcmd == '+' and loper != None ):
        voper,foper = utils2.string_to_number(loper)
        v,f         = utils2.string_to_number(raw_value)
        if ( foper and f ):
            num = v + voper
            return str(num)
        else:
            return raw_value

    # Subtract value
    elif ( lcmd == '-' and loper != None ):
        voper,foper = utils2.string_to_number(loper)
        v,f         = utils2.string_to_number(raw_value)
        if ( foper and f ):
            num = v - voper
            return str(num)
        else:
            return raw_value


    # Replace value
    elif ( lcmd == 'r' and loper != None ):
        return loper
        
    # Conditionally (true) Replace value
    elif ( lcmd == 't' and loper != None ):
        if ( raw_value == '0' ):
            return ''
        else:
            return loper
            
    # Conditionally (false) Replace value
    elif ( lcmd == 'f' and loper != None ):
        if ( raw_value != '0' ):
            return ''
        else:
            return loper
        
    # Unsupport Command    
    else:
        sys.exit( "ERROR: Unsupported L-Command:{}. File={}, line# {}, col# {}".format( lcmd, header.get_fname(), header.get_line_count() + lnum, col+1 ) ) 
  
    
           
###
def process_special_token( token_id, iters, args ):
    # Year
    if ( token_id == '_YYYY' ):
        return SMapEntry( "{:04}".format( datetime.now().year ), args['-d'] )
 
    # Month
    if ( token_id == '_MM' ):
        return SMapEntry( "{:02}".format( datetime.now().month ), args['-d'] )
               
    # Day
    if ( token_id == '_DD' ):
        return SMapEntry( "{:02}".format( datetime.now().day ), args['-d'] )

    # Iteration count
    if ( token_id == '_ITERNUM' ):
        return SMapEntry( str(iters.get_iter_number()), args['-d'] )

    # Iteration Value
    if ( token_id == '_ITERVAL' ):
        return SMapEntry( str(iters.get_iter_value()), args['-d'] )

    # Iteration Value
    if ( token_id == '_RELPATH' ):
        return SMapEntry( args['-r'], args['-d'] )

    # If I get here, then no special token found
    return None


###
def subscripting( line, idx, header, smap, iters, outfd, lnum, args ):
    marker = header.get_mark() + "_SUBSCRIPT" 
    if ( line[idx:].startswith( marker ) ):   
        process_subscript_statement( line[idx+len(marker):], header, iters, smap, outfd, line[:idx], lnum, args )
        return True
               
    return False

###
def inlining( line, idx, header, smap, iters, outfd, content, lnum, args ):
    marker = header.get_mark() + "_INLINE" 
    if ( line[idx:].startswith( marker ) ):   
        return process_inline_statement( line[idx+len(marker):], idx, header, iters, smap, outfd, line[:idx], content, lnum, args )
               
    return 0

###
def process_inline_statement( parms, indent_idx, header, iters, smap, outfd, indent, content, lnum, args ):
    # Strip off S-CMD(s) if any
    scmd_mark   = parms.find( ':' )
    scmd_string = ''
    if ( scmd_mark != -1 ):
        scmd_string = parms[scmd_mark+1:]
        parms       = parms[:scmd_mark]

    # Process token values
    sargs = parms.split(',')

    # Build inline content
    newcontent = read_inline_content( indent_idx, content, header, args, lnum )

    # Build the header & map
    newheader = Header( os.path.join(header.get_fname(), "_INLINE"), header.get_mark(), header.get_esc() )
    newmap    = SMap( args['-d'] )
    newheader.set_line_count( header.get_line_count() + lnum + 1 )
    for t in range(1, len(sargs) ):
        entry, token_id = convert_to_map_entry( sargs[t].strip(), header, smap, iters, args, lnum )
        newheader.add_token( token_id )
        newmap.add_entry( entry )

    # Build iteration info
    newiters = calc_iterations( indent, newmap, scmd_string, smap, header, lnum, iters.get_indent_level() )

    # Expand Subscript
    expand_content( newheader, newcontent, newmap, newiters, outfd, args )

    # Return the number of of content line consumed by the Inline subscript
    return len(newcontent) + 1


###
def read_inline_content( indent_idx, content, header, args, lnum ):
    idx               = lnum + 1
    inline_statements = 1
    start_mark        = header.get_mark() + "_INLINE"
    end_mark          = header.get_mark() + "_END_INLINE"
    new_content       = []
  
    while( inline_statements > 0and idx < len(content) ):
        line = content[idx]
        if ( line.find(start_mark) != -1 ):
            inline_statements += 1

        elif ( line.find(end_mark) != -1 ):
            inline_statements -= 1

        new_content.append( line[indent_idx:] )
        idx += 1

    if ( inline_statements > 0 ):
        sys.exit( "ERROR: Unmatched _INLINE and/or missing _END_INLINE statement (unmatched count={}). File={}, starting at line# {}".format( inline_statements, header.get_fname(), header.get_line_count() + lnum ) )

    # Remove trailing _END_LINE statement
    new_content.pop()
    return new_content
      

###
def process_subscript_statement( parms, header, iters, smap, outfd, indent, lnum, args ):
    # Strip off S-CMD(s) if any
    scmd_mark   = parms.find( ':' )
    scmd_string = None
    if ( scmd_mark != -1 ):
        scmd_string = parms[scmd_mark+1:]
        parms       = parms[:scmd_mark]

    # Process token values
    sargs = split_parms( parms, ',')
    if ( len(sargs) == 0 ):
        sys.exit( "ERROR: Invalid _SUBSCRIPT statement. File={}, line# {}".format( header.get_fname(), header.get_line_count() + lnum ) ) 

    # Open template file
    newheader, newcontent = read_template_file( sargs[1].strip(), iters, args )

    # Build the map
    newmap = SMap( args['-d'] )
    if ( len(sargs) > 1 ):
        for t in range(2, len(sargs)):
            entry, token_id = convert_to_map_entry( sargs[t].strip(), header, smap, iters, args, lnum )
            newmap.add_entry( entry )

    # Build iteration info
    newiters = calc_iterations( indent, newmap, scmd_string, smap, header, lnum, iters.get_indent_level() )
    
    # Expand Subscript
    expand_content( newheader, newcontent, newmap, newiters, outfd, args )
   
                
###
def calc_iterations( indent, smap, scmd_string, parent_map, header, lnum, old_level ):
    # Calculate iterations number based on the Substitution map
    max_iter_count = 1
    for e in smap.get_entries():
        if ( e.get_iter_count() > max_iter_count ):
            max_iter_count = e.get_iter_count()

    # Check for S-Cmd
    max_iter_count, start, step = do_scmd_iterate( max_iter_count, scmd_string, parent_map, header, lnum )

    # Constructor iteration data structure
    return Iteration( indent, max_iter_count, start, step, old_level + 1 )
    

### 
def do_scmd_iterate( max_iter_count, scmd_string, parent_map, header, lnum ):
    # Skip if not the iterate command
    if ( scmd_string == None ):
        return max_iter_count, 0, 1

    # Parse parameters. Format: iter [<start> [<step> [<min_iters>]]] 
    parms = scmd_string.strip().split()
    if ( len(parms) == 0 or (parms[0] != 'iter' and parms[0] != 'ITER') ):
        return max_iter_count, 0, 1
     
       
    # Default values
    start     = 0
    step      = 1
    min_iters = 1
 
    # Set values
    if ( len(parms) > 1 ):
        v,f = convert_to_number( parms[1], parent_map, header, lnum )  
        if ( f ):
            start = v

    if ( len(parms) > 2 ):
        v,f = convert_to_number( parms[2], parent_map, header, lnum )  
        if ( f ):
            step = v
   
    if ( len(parms) > 3 ):
        v,f = convert_to_number( parms[3], parent_map, header, lnum )  
        if ( f ):
            if ( parms[0] == 'iter' ):
                min_iters = max( max_iter_count, v )
            else:     
                min_iters = v

    # Return results
    return min_iters, start, step


### 
def convert_to_number( parm, parent_map, header, lnum ):
    print("parm=[{}]".format( parm ))
    if ( parm.startswith('$') and parm.endswith('$') ):
        # Get map index for token ID
        idx = header.find_token( parm[1:-1] )
        if ( idx == -1 ):
            sys.exit( "ERROR: _SUBSCRIPT/_INLINE statement - Invalid S-CMD token name: {}. File={}, line# {}".format( parm[1:-1], header.get_fname(), header.get_line_count() + lnum ) ) 
        entry = parent_map.get_entry( idx )
        if ( entry == None ):
            sys.exit( "ERROR: _SUBSCRIPT/_INLINE statement - No matching map entry for S-CMD token_id={}. File={}, line# {}".format( parm[1:-1], header.get_fname(), header.get_line_count() + lnum ) ) 
        parm = entry.get_raw_value()
        
    return utils2.string_to_number( parm )  
        

###
def convert_to_map_entry( token_id, header, smap, iters, args, lnum ):

    # Check for pass all or pass iteration
    use_all = False
    if ( token_id.startswith( '*' ) ):
        use_all  = True
        token_id = token_id[1:]

    # Trap literal string as the token value
    if ( (token_id.startswith('"') and token_id.endswith('"')) or (token_id.startswith("'") and token_id.endswith("'")) ):
        token_id = token_id[1:-1]
        return SMapEntry( token_id, args['-d'], use_all ), token_id

    # Get map index for token ID
    idx = header.find_token( token_id )
    if ( idx == -1 ):
        sys.exit( "ERROR: _SUBSCRIPT/_INLINE statement - Invalid token name: {}. File={}, line# {}".format( token_id, header.get_fname(), header.get_line_count() + lnum ) ) 
    entry = smap.get_entry( idx )
    if ( entry == None ):
        sys.exit( "ERROR: _SUBSCRIPT/_INLINE statement - No matching map entry for token_id={}. File={}, line# {}".format( token_id, header.get_fname(), header.get_line_count() + lnum ) ) 
        
    # Construct Map entery for token id                
    return SMapEntry( entry.get_raw_value(), args['-d'], use_all ), token_id


#------------------------------------------------------------------------------
###
def build_map( args ):
    # Select source of the map
    if ( args['-f'] ):
        input = open_map_file( args['-f'] )
    elif( args['<entries>'] ):
        input = args['<entries>'] 
    else:
        sys.exit( "ERROR: No substitution map entries provided. Use 'f4 -h' for help" )

    # build the map
    smap = SMap( args['-d'] )
    for entry in input:
        e = SMapEntry( normalize_entry( entry ), args['-d'] )
        smap.add_entry( e )

    return smap


###
def normalize_entry( raw_entry ):
    e = raw_entry.strip();
    if ( e.startswith('"') and e.endswith('"') ):
        e = e[1:-1]
    elif ( e.startswith("'") and e.endswith("'") ):
        e = e[1:-1]
    
    return e

                        
###
def open_map_file( fname ):
    try:
        inf = open( fname )
        return inf;
    except EnvironmentError:
        sys.exit( "ERROR: Cannot open substitution map input file: {}".format( fname ) )


#------------------------------------------------------------------------------
###
def read_template_file( template, iters,  args ):
    header        = None
    header_found  = False
    content       = []
    content_found = False
    header_lnums  = 1

    # Read template file
    try:
        infile, full_name = open_template_file( template, args, iters )
        for line in infile:
            # Load #CONTENT section into memory
            if ( content_found ):
                content.append( line )

            # Parse #HEADER Section
            else:
                header_lnums += 1
                if ( line.startswith(';') ):
                    pass
                elif ( not line.strip() ):
                    pass
                elif ( line.startswith( '#HEADER' ) ):
                    header       = process_header( full_name, line, args )
                    header_found = True
                elif ( header_found and line.startswith( '#CONTENT') ):
                    content_found = True
                elif ( header_found ):
                    header.add_token( line.split()[0].split(',')[0].strip() )

            
    except EnvironmentError:
        infile.close()
        sys.exit( "ERROR: Cannot read template file: {}".format( template ) )

    # Housekeeping
    infile.close()

    # Trap errors
    if ( content_found == False or header_found == False ):
        sys.exit( "ERROR: Invalid Template file ({}).  Missing #HEADER and/or #CONTENT markers.".format( template ) )
    
    # Return results
    header.set_line_count( header_lnums )
    return header, content


###
def process_header( fname, line, args ):
    options = line.split(',')
    count   = len(options)
    mark    = '$'
    esc     = '`'

    # Override default options
    if ( count > 1 ):
        mark = options[1].strip()
        if ( count > 2 ):
            esc = options[2].strip()

    return Header( fname, mark, esc )
        

###
def open_template_file( fname, args, iters ):
 
    # Create alternate search paths   
    lpath = process_path_argument( args['--local'], args )
    gpath = process_path_argument( args['--global'], args )
    if ( args['-g'] ):
        print("{}Opening template file= {}...".format( iters.get_verbose_indent(), fname ))
        print("{}  search path: cwd      = {}".format( iters.get_verbose_indent(), os.getcwd() ))
        print("{}  search path: --local  = {}".format( iters.get_verbose_indent(), lpath ))
        print("{}  search path: --global = {}".format( iters.get_verbose_indent(), gpath ))

    # Search CWD
    try:
        inf = open( fname )
        if ( args['-g'] ):
            print("{}  -->opened from cwd.".format( iters.get_verbose_indent() ))

        return inf, fname;
    except EnvironmentError:
        pass
            
    # Search local path
    try:
        f = os.path.join( lpath, fname )
        inf = open( f )
        if ( args['-g'] ):
            print("{}  -->opened from --local".format( iters.get_verbose_indent() ))

        return inf, f
    except EnvironmentError:
        pass

    # Search global path
    try:
        f = os.path.join( gpath, fname )
        inf = open( f )
        if ( args['-g'] ):
            print("{}  -->opened from --global.".format( iters.get_verbose_indent() ))

        return inf, f
    except EnvironmentError:
        pass

    # If I get here -->could not find the requested template file
    sys.exit( "ERROR: Cannot open/find template file: {}".format( fname ) )
    


###
def process_path_argument( path, args ):

    # standardize the direction seperator
    path = utils.standardize_dir_sep( path )
    if ( path.endswith( os.sep ) ):
        path = path[:-1]

    # No extra work if NOT using Outcast notation
    if ( not args['--outcast'] ):
        return path
        
    # Outcast notation: Workspace root
    if ( path.startswith(os.sep + os.sep) ):
        # Handle the case of where the package is MY package (i.e. do NOT map through the xpkgs/ tree)
        d = path[2:].split(os.sep)
        if ( d[0] == os.path.basename( PKG_ROOT ) ):
            if ( len(d) == 1 ):
                path = os.path.join(WRKSPACE_ROOT, PKG_ROOT)
            else:
                path = os.path.join(WRKSPACE_ROOT, PKG_ROOT, os.path.join(*d[1:]) )
        else:
            path = os.path.join(WRKSPACE_ROOT, 'xpkgs', path[2:] )


    # Outcast notation: Package root
    elif ( path.startswith( os.sep ) ):
        path = os.path.join(WRKSPACE_ROOT, PKG_ROOT, path[1:] )

    # NOT Outcast notation -->Treat as a non-outcast path
    return path

    
                     
###
class Header(object):
    def __init__( self, fname, mark, esc ):
        self.num_tokens = 0
        self.mark       = mark
        self.esc        = esc
        self.tokens     = []
        self.lcount     = 0
        self.fname      = fname

    def get_mark( self ):
        return self.mark

    def get_esc( self ):
        return self.esc

    def add_token( self, token_name ):
        self.tokens.append( token_name )
        self.num_tokens += 1

    def find_token( self, name ):
        idx = 0
        for n in self.tokens:
            if ( n == name ):
                return idx
            idx +=1

        # If I get here -->no matching token
        return -1

    def set_line_count( self, count ):
        self.lcount = count

    def get_line_count( self ):
        return self.lcount

    def get_fname( self ):
        return self.fname


    def __repr__(self):
        ret = "HEADER: count={}, mark={}, esc={}, Tokens={}".format( self.num_tokens, self.mark, self.esc, self.tokens )
        return ret


#------------------------------------------------------------------------------
###
class SMapEntry(object):
    def __init__( self, value, delimiter, force_no_iter_count=False ):
        # Parse the raw value into multiple values 
        if ( delimiter ):
            self.values = value.split( delimiter )

        # No delimimeter -->forces no parsing
        else:
            self.values = []
            self.values.append( value )

        self.org       = value
        self.count     = len(self.values)
        self.itercount = self.count
        if ( force_no_iter_count ):
            self.itercount = 1
        

    def get_value(self, index = 0 ):
        if ( index >= self.count ):
            index = self.count - 1
        return self.values[index]

    def get_raw_value( self ):
        return self.org
             
    def get_count(self):
        return self.count

    def get_iter_count(self):
        return self.itercount

    def __repr__(self):
        ret = "<count={}, icount={}, values={}>".format( self.count, self.itercount, self.values )
        return ret


###
class SMap(object):
    def __init__( self, delimiter=',' ):
        self.num_entries = 0
        self.delimiter   = delimiter
        self.entries     = []
        
    def add_entry( self,  entry_object ):
        self.entries.append( entry_object )
        self.num_entries += 1
        
    def get_num_entries( self ):
        return self.num_entries

    def get_entries( self ):
        return self.entries

    def get_entry( self, index ):
        if ( index >= self.num_entries ):
            return None
        else:
            return self.entries[index]

        
    def __repr__(self):
        ret = "SMAP: count={}, del={}. Entries={}".format( self.num_entries, self.delimiter, self.entries )
        return ret


#------------------------------------------------------------------------------
### 
def split_parms( line, delimiter ):
    parms  = []
    idx    = 0
    mark   = 0
    dquote = False
    squote = False

    while( idx < len(line) ):
        # Inside a double quote
        if ( dquote ):
            if ( line[idx] == '"' ):
                dquote = False
 
        # Inside a single quote
        elif ( squote ):
            if ( line[idx] == "'" ):
                squote = False
        
        # Start of a double quote
        elif ( line[idx] == '"' ):
            dquote = True

        # Start of a single quote
        elif ( line[idx] == "'" ):
            squote = True
  
        # found a comma
        elif ( line[idx] == delimiter ):
            parms.append( line[mark:idx] )
            mark = idx + 1
            
        # advance index
        idx += 1
        
    # Return an error is bad quoting
    if ( squote or dquote ):
        return []

    # capture the last parm
    parms.append( line[mark:] )
    return parms

      
###
def compute_relative_path( args, default_root_path ):
    # Set default for -r path option
    rpath = args['-r']
    if ( rpath == None ):
        rpath = os.environ.get('F4_ROOT_PATH')
        if ( rpath == None ):
            rpath = default_root_path

    # Get parent path
    rpath = process_path_argument( rpath, args )
    cwd   = os.getcwd()

    # calc relative path
    if ( cwd.startswith( rpath ) ):
        relpath = cwd.replace( rpath, '', 1 )
    else:
        relpath = ""


    # store the relative path (as a multi-value string) back into the args dictionary (its convenient)
    relpath    = relpath.strip(os.sep)
    args['-r'] = relpath.replace(os.sep, args['-d'] )
    
    # Debug
    if ( args['-g'] ):
        print("_RELPATH={}".format( args['-r'] ))



###
class Iteration(object):
    def __init__( self, indent="", number_of_iterations=1, initial_value=0, step_size=1, indent_level=0 ):
        self.max_iter     = number_of_iterations
        self.iter_num     = 0
        self.iter_value   = initial_value
        self.iter_step    = step_size        
        self.indent       = indent
        self.indent_level = indent_level
        self.verbose      = "" if indent_level == 0 else "  " * indent_level

    def next_iteration( self ):
        if ( self.iter_num+1 < self.max_iter ):
            self.iter_num   += 1
            self.iter_value += self.iter_step
            return True
        else:
            return False
            
    def get_iter_number( self ):
        return self.iter_num
        
    def get_iter_value( self ):
        return self.iter_value
        
    def get_max_iter_count( self ):
        return self.max_iter
    
    def get_indent( self ):
        return self.indent
            
    def get_indent_level( self ):
        return self.indent_level
   
    def get_verbose_indent( self ):
        return self.verbose

    def __repr__(self):
        ret = "current={}, max={}, val={}, step={}, indent_len={}".format( self.iter_num, self.max_iter, self.iter_value, self.iter_step, len(self.indent) )
        return ret
        
                
#------------------------------------------------------------------------------
guide = r"""
F4 USER GUIDE
===============================================================================
F4 takes an input template and a user supplied Substitution Map and generates
an output file by replacing token identifiers within the template file with
values from the Substitution Map.

F4 is an updated, python version of the Jcl ASX command line applet. ASX in 
turn was inspired from the VAX FMS (Forms Management System).  So yes, you can 
teach an old programmer new tricks as long as it is an old trick.


NOTE: It is HIGHLY recommended that you do NOT use tabs in your template files,
      especially when using subscripting.


TEMPLATE FILE FORMAT:
-----------------------------------
#HEADER [,<mark_char>][,<esc_char>]
tokenid_0 [,description comment]
tokenid_1 [,description comment]
...
tokenid_n [,description comment]
#CONTENT
<all lines after the #CONTENT marker are expanded literally to the output
 file.  When one of the above tokenid is encountered, the token is
 replace with its corresponding text from the Substitution Map. >

---------------------------------------


mark_char:=         A one character symbol used to indicate the start of a 
                    token. The default mark character is '$'.

esc_char:=          Escape character used to embedded a literal mark and/or 
                    escape characters.  The default escape character is '`'

tokenid_n:=         Identifier for a token. Tokens are match up with the 
                    Substitution Map by their order listed.  That is the first 
                    token listed maps to index 0 in the Subsitution map, second
                    token maps to index 1, etc. Token identifier can be 
                    any alpha-numeric string that does NOT start with a '_' or 
                    contain spaces. There is are reserved token identfiers.  
                    All reserverd token identifiers start with a leading '_'.

NOTE: Comment lines and Blank lines are ignored from the top of the file
      until the #CONTENT marker is encounters.  Comment lines start with
      a ';'.



REPLACEMENT TOKEN SYNTAX:
-------------------------
The following syntax is used within the #CONTENT section of the template to
embedded token identifiers.

<mark>[<lcmd>[<operand>]<]<tokenid>[><rcmd>[operand]<mark>
<esc><mark>
<esc><esc>

Where:
    <mark>      Is the mark character defined by the #HEADER statement.  All
                embedded token IDs start with a mark character.  
            

    <esc>       Is the escape character defined by the #HEADER statement. The
                escape character is used to embedded a literal mark and/or
                escape characters in the #CONTENT section.

    <lcmd>      Single character that selects an optional L-Command.  See 
                below for a list of L-Commands.

    <operand>   Optional string input for L/R-Commands.
                
    <tokenid>   One of the token identifiers defined in the #HEADER section.
                Also a string literal (encapsulated in single or double quotes)
                can be used in lieu of a token identify.  When this is done the
                string literal is the substitution value.  Typically this is
                done when it is desirable to apply an L or R command to fixed
                string.
               
    <rcmd>      Single character that selects an optional R-Command.  See 
                below for a list of R-Commands.

    Examples:
        Given: className="bob", namspeaces="Rte,Db,Set"

            $className$        expands to: "bob"
            $u<className>$     expands to: "Bob"
            $namespaces>+::$   expands to: "Rte::Db::Set" 



ITERATION:
----------
Iteration of the #CONTENT section is done by specifying two or more values for
at least one of the entries in the Substitution Map.  The Substitution Map 
defines a single character that is used to delineate multi-vaules in a single
entry.  The default delimiter character is ','.   The  number of iterations
that occur is the number of delimiter characters contained  in the substitution
text + 1.  When more than one Substitution map entry contains multiple values,
then the number of iteration is the maximum of the combined iterators.  If an
multi-valued entry exhausted its lists before the overall iteration process is
finished, its  last value is used for the remaining iterations.

Iteration can also be 'forced' by using the S-Command "iter" when invoking a
subscript (see below for details about S-Commands).  The number of iterations
performs is the maximum iteracount derived from the Substitution Map and the
'iter' S-Command. 

NOTE: Iteration ONLY APPLIES to subscripts.  The iteration rules are not 
      applied when processing the top level template.




SPECIAL TOKENS:
---------------
The following are special predefined tokens that can be used in the '#CONTENT'
section.  
_YYYY       - Embeds the current year in with the following format: '%04d'
_MM         - Embeds the current month in with the following format: '%02d'
_DD         - Embeds the current day in with the following format: '%02d'
_ITERNUM    - Embeds the iteration number with the following format: '%d'. The
              vaule of the iteration number always starts at zero.
_ITERVAL    - Embeds the iteration counter value with the following format: '%d'
_RELPATH    - A multi-value token that contains a directory path - where each
              directory in the path is a value - of current working directory
              relative to a 'root directory' that is specified when the
              Substitution Map is created. For example:
                Given: 
                    1) The CWD is: c:\_workspaces\zoe\src\Cpl\Bob
                    2) The 'root directory' is: c:\_workspaces\zoe\src
                
                    Then: _RELPATH = "Cpl,Bob"
            



L-COMMANDS:
----------------
L-Commands allow an operation to be performed on the value being subsituted.
For example, the token operator 'U', will convert the value to all uppercase
letters before being substituted into the output file.  A token operator is
specified by a character pair of the token seperator and operator character
immediately following  the initial token seperator.  For example: To apply the
uppercase operator to the token - 'name' - use: $U<name.  The following is a
list of currently supported L-Commands:

    U   - upper case token value
    L   - lower case token value
    u   - Force the FIRST character in the token value to upper case
    l   - Force the FIRST character in the token value to lower case
    +   - Adds the value the L-cmd operand parameter to the substitution value.
          If the substitution value is not a numeric value, nothing is done.
    -   - Subtracts the value the L-cmd operand parameter to the substitution 
          value. If the substitution value is not a numeric value, nothing is 
          done.
    r   - Replace the token value with the L-cmd operand parameter.  This is
          typically used in conjunction with the R-Cmd '+' operator.
    t   - Conditionally replace the token value with the L-cmd operand parameter 
          when the token values does NOT equal '0'; else the token is replaced
          with a null value
    f   - Conditionally replace the token value with the L-cmd operand parameter 
          when the token values does equal '0'; else the token is replaced
          with a null value


R-COMMANDS:
--------------
R-Commands perform posfix operations on the replacement being done.  The
following is list of currently supported R-Commands.  A single substitution can
include both L and R commands with the L command being performed (on all
values) before the R command is performed.

    f   - First Only.  This command substitues the token identifier's value 
          ONLY on the first iteration.  All other iterations, an empty string 
          is substituted.
          
    F   - Not First.  This command substitues the token identifier's value 
          on all iterations EXCEPT the first iteration. On the first iteration
          an empty string is substituted.

    l   - Last Only.  This command substitues the token identifier's value 
          ONLY on the last iteration.  All other iterations, an empty string 
          is substituted.
          
    L   - Not Last.  This command substitues the token identifier's value 
          on all iterations EXCEPT the last iteration. On the last iteration
          an empty string is substituted.

    :   - Indexing. This command will select the Nth value from a multi-valued
          token.  This command takes a 'index' operand that is zero based 
          index.  If the token identifier is not a multi-valued entry or the 
          'index' value exceeds the entrys range - then last (only) value is
          used for the substitution.

            Given token: names="bob,henry,fred"
            
                $names>:0$    results in: "bob"
                $names>:32$   results in: "fred"
                $u<names>:1$  results in: "Henry"
       
       
    +   - Concatention. This command will expand a multi-value into a single 
          value when the token is replaced.  The command takes an optional
          'string' operand that will be inserted between the individual 
          expanded values.  There is no trailing 'string' when used. Examples:
            
            Given token: names="bob,henry,fred"
            
                $names>+$     results in: "bobhenryfred"
                $names>+, $   results in: "bob, henry, fred"
                $U<names>+_$  results in: "BOB_HENRY_FRED"

    #   - Count. This command returns the number of values in a multi-valued
          token.  If the token is not a multi-valued token, the operator 
          returns 1
          
           

SUBSCRIPTS:
----------
The expander supports nesting templates within templates. Special/reserved 
token identifiers are used to invoke the nesting process.  The "subscripting"
process also support iteration when expanding the subscript.  The following
are the different forms that can be used to 'invoke' a subscript.

NOTE: When the subscripting occurs, the entire contents of the subscript is
      "embedded" into its parent script with the same indentation level as
      the "$_SUBSCRIPT" token.


FORMAT #1 - Reference an external template file

    <mark>_SUBSCRIPT,<fname> [,[*]<tokenA>[,[*]<tokenB>]....[,[*]<tokenN>][:<scmd> [<args>...]]*
    
    Where:
        fname:=   File name of the template to include.  The template file is
                  assumed to be the cwd (current-working-directory) or 'fname'
                  is an absolute path.
                  
        tokenA:=  The current iteration substitution value of "tokenA" is used 
                  as the  first entry in the  substitution map of for the
                  sub-script that will be  expanded.  The current substitution
                  value of "tokenB" is used as the second  entry in the
                  substitution map, etc.  ITERATION NOTE: If a leading '*'
                  characacter is used when specifying the token name, then ALL 
                  values of the token are used to create the entry in the 
                  substituion map AND the entry will NOT trigger the iteration
                  mechnaism for the subscript.  If the iteration mechanism is
                  trigger via a different multi-valued (non leading '*') token,
                  the multi-valued tokens that start with a '*' will be treated
                  as single valued token with respect to iteration within the
                  subscript.

                  NOTE: 'tokenA' can be a literal string enclosed with in 
                        double or single quotes.  The literal string can 
                        also contain multi-values using the delimiter character
                        specified by the '-d' option.  For example:
                            $_SUBSCRIPT, example_1.f4t, "MyClassName", "blue,red"
                   
        scmd      A S-Command string identifier followed by zero or more
                  command arguments.  Multiple S-Commands can be specified using
                  a comma to seperate the commands.  When multiple commands are
                  specified, there are executed if left to right order
                  
        args      Optional arguments to a S-Command.  Individual arguments are
                  seperated by spaces.
                  
                  

FORMAT #2 - Inlined Subscript

    <mark>_INLINE [,[*]<tokenA>[,[*]<tokenB>]....[,[*]<tokenN>]:[<scmd> [<args>...]]*
    .....
    <mark>_END_INLINE
     
    
    Subscripts can be inlined.  This is done by place the content of the
    'subscript file' between the start-of-subscript-line marker and the
    end-of-subscript-line marker.  The start-of-subscript-line marker is:
    "$_INLINE,...".  The end-of-subscript-line marker is "$_END_INLINE". 
    
    The #HEADER section of inlined subscripts are implied using the following
    rules:
        1) num_tokens is determined by the number of tokens supplied with
           '$_INLINE' statement.
        2) The mark_char, esc_char, and iteration_char values are inherited
           from the parent template's #HEADER section
        3) The token identifier names are set/assumed to match the names of 
           the tokens supplied on the '$_INLINE' statement.
      
              
    NOTES: 
        o For the indentation to work properply TABS can not be used as leading 
          characters in the contents of the inlined subscript! 
        o The <tokenA> arguments can NOT be literal strings


S-COMMANDS:
--------------
S-Commands modify/aguments the behavior when invoking subscripting. The 
following is a list of currently supported S-Commands:

    Explicit Iteration:
    
        iter [<start> [<step> [<min_iters>]]] 
    
        This command forces iteration on subscript.  The <start> value is
        a numeric value for the initial value of the _ITERVAL token.  The
        default for <start> is 0.  The <step> value is the step that _ITERVAL
        is increment on subsequent iterations.  The default for <step> is 1.
        The <min_iters> value forces at least that many iterations.  The 
        default value for <min_iters> is 0. The actual number of iteration is 
        the maximum of <min_iters> value and the iteration count derived from 
        the Substitution Map. 
        
        The <start>, <step>, and <min_iters> values numeric constants or
        taken from token values.  The syntax for using a token value is
        '$tokenname$.  For example:
            
            $_SUBSCRIPT, mytemplate.f4t : iter $token_startingValue$ 2 5
            
            
        
        ITER <start> <step> <min_iters> 
        
        This command is the same as 'iter' except ALL arguments are required
        AND the actual number of iterations is ONLY defined by the ITER
        command, i.e. NOT a union of the ITER command and the iteration count
        derived from the Substitution map.
        
        

EXAMPLES
===============================================================================

**** EXAMPLE#1 ****

Simple Template with token operator
-------------------------------------------
; Simple Script
#HEADER

className
flagName,  Name of the flag variable 

; Begin Expansion Text
#CONTENT
class $u<className$
{
public:
    bool $l<flagName$;
};
-------------------------------------------

GIVEN: 
    className:= 'bob'
    flagName:=  'Foobar'
    
RESULT:
-------------------------------------------
class Bob
{
public:
    bool foobar;
};
-------------------------------------------



**** EXAMPLE#2 ****

Template With External Subscripting
-------------------------------------------
; Nested Script
#HEADER,%,~

embeddedClassName, Name of the embedded class
flagName,          Name of the flag variable for the above class
myClass            Name of My class that will contain the embedded class

; Begin Expansion Text
#CONTEXT
class %myClass%
{
    %_SUBSCRIPT, simpleScript, embeddedClassName, flagName
};
-------------------------------------------

GIVEN: 
    embeddedClassName:= 'oak'
    flagName:=          'Foobar'
    myClass:=           'bob'
    
RESULT:
-------------------------------------------
class bob
{
    class Oak
    {
    public:
        bool foobar;
    };
};
-------------------------------------------



**** EXAMPLE#3 ****

Template With Subscripting Iteration (the subscript is repeated N time, 
once for each name in 'embeddedNames')
-------------------------------------------
; Nested Script with iteration
#HEADER

embeddedNames,     Names of the N class to embedded. Each name is seperated 
;                  with ',' (Foo,Foo2,...)
flagName,          Name of the flag variable for the above class
myClass            Name of My class that will contain the embedded class

; Begin Expansion Text
#CONTENT
class $myClass$
{
    $_SUBSCRIPT, simpleScript, embeddedNames, flagName
};
-------------------------------------------

GIVEN: 
    embeddedNames:= 'oak,pine,willow'
    flagName:=      'foobar'
    myClass:=       'bob'
    
RESULT:
-------------------------------------------
class bob
{
    class Oak
    {
    public:
        bool foobar;
    };
    class Pine
    {
    public:
        bool foobar;
    };
    class Willow
    {
    public:
        bool foobar;
    };
};
-------------------------------------------


**** EXAMPLE#4 ****

Script With Inlined Subscripting
-------------------------------------------
; Simple Script
#HEADER
className
flagName,  Name of the flag variable 

; Begin Expansion Text
#CONTENT
class $u<className$
{
public:
    bool m_$l<flagName$;

    $_INLINE, className, flagName
    void $u<className$::set$u<flagName$( bool newvalue  );
    $_END_INLINE
    
public: // MORE stuff here
};
-------------------------------------------

GIVEN: 
    className:=     'bob'
    flagName:=      'foobar'
    
RESULT:
-------------------------------------------
class Bob
{
public:
    bool m_foobar;
    
    void Bob::setFoobar( bool newvalue );
    
public: // MORE stuff here
};
-------------------------------------------

"""

    