"""Collection of helper functions"""


import os, sys, errno, fnmatch, subprocess, time, copy
#import symlinks, platform, tarfile
#from collections import deque


# Module globals
_dirstack = []
quite_mode   = False
verbose_mode = False

#-----------------------------------------------------------------------------
def print_warning( msg ):
    if ( not quite_mode ):
        print( "Warn:  " + msg )

def set_quite_mode( newstate ):
    global quite_mode
    quite_mode = newstate
       

#-----------------------------------------------------------------------------
def print_verbose( msg, no_new_line=False, bare=False ):
    if ( verbose_mode ):
        if ( not no_new_line ):
            if ( bare ):
                print( msg )
            else:
                print( "Info:  " + msg )
        else:
            if ( bare ):
                print( msg, end='' )
            else:
                print( "Info:  " + msg, end='' )
        
def set_verbose_mode( newstate ):
    global verbose_mode
    verbose_mode = newstate
       
#-----------------------------------------------------------------------------
def walk_file_list( pattern, pkgpath ):
    list = []
    for root, dirs, files in os.walk(pkgpath):
        for f in fnmatch.filter(files,pattern):
            list.append( os.path.join(root,f) )
            
    return list        


        
#-----------------------------------------------------------------------------
def push_dir( newDir ):
    global _dirstack
    
    if ( not os.path.exists(newDir) ):
        exit( "ERROR: Invalid path: "+ newDir )
    
    _dirstack.append( os.getcwd() )
    os.chdir( newDir )
    
    
def pop_dir():
    global _dirstack
    os.chdir( _dirstack.pop() )

def mkdirs( dirpath, exist_ok=True ):
    try:
        os.makedirs(dirpath,exist_ok=exist_ok)
    except: 
        sys.exit( "ERROR: cannot create directory tree: {}.[{}]".format( dirpath, sys.exc_info()[0] ) )

##-----------------------------------------------------------------------------
#def render_dot_file_as_pic( pictype, oname ):
#    if ( pictype ):
#        cmd   = "dot -T{} -x -O {}".format(pictype,oname)
#        fname = "{}.{}".format( oname, pictype )
#        if ( os.path.isfile(fname) ):
#            print_warning("Removing existing rendered {} file: {}".format(pictype,fname) )
#            try:
#                os.remove(fname)
#            except Exception as ex:
#                exit( "ERROR: {}".format(ex) )
        
#        p  = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
#        r  = p.communicate()
#        r0 = '' if r[0] == None else r[0].decode()
#        r1 = '' if r[1] == None else r[1].decode()
#        if ( p.returncode ):
#            print( r0 + ' ' + r1 )
#            exit( "ERROR: Failed rendering the .DOT file (ensure that the GraphVis 'bin/' directory is in your path)" )
        


##-----------------------------------------------------------------------------
#def set_workspace( args, skipwrk=False ):
#    if ( skipwrk ):
#        return
        
#    if ( not args['-w'] ):
#        wrkroot = detect_workspace_root( os.getcwd() )
#        if ( wrkroot == None ):
#            exit( "ERROR: Can not auto detect the Workspace root directory" )
#        args['-w'] = wrkroot

#    args['-w'] = standardize_dir_sep( os.path.realpath(args['-w']) )  
#    if ( not os.path.exists( args['-w'] ) ):
#        exit( "ERROR: Invalid path - {} - for the Workspace root directory".format( args['-w'] ) ) 
        
        
#def detect_workspace_root( starting_dirname ):
#    result = starting_dirname
    
#    if ( _test_for_markers( starting_dirname ) == False ):
        
#        # get parent directory
#        parent = os.path.abspath(os.path.join(starting_dirname, os.path.pardir))
        
#        # Fail if I get to the root directory without finding my markers
#        if ( os.path.split(parent)[0] == starting_dirname ):
#            return None

#        # Try my parent
#        result = detect_workspace_root( parent )
     
#    return result
        
#def _test_for_markers( dir ):
#    result = False

#    if ( os.path.isdir(dir + os.sep + OUTCAST_XPKGS_DIRNAME()) and os.path.isdir(dir + os.sep + OUTCAST_XINC_DIRNAME()) ):
#        result = True

#    return result    


#def check_use_current_package( args ):
#    if ( args['<pkgname>'] == '.' ):
#        path   = os.getcwd();
#        wspace = detect_workspace_root( path )
#        pkg    = path[len(wspace):]
#        args['<pkgname>'] = pkg.split(os.sep)[1]
        

##-----------------------------------------------------------------------------
#def set_uverse( args, skipenv=False ):
#    if ( skipenv ):
#        return
        
#    # Check the environment variable 
#    if ( not args['--uverse'] ):
#        uverse = os.environ.get( OUTCAST_PKGS_UVERSE() )
#        if ( uverse == None ):
#            exit( "ERROR: The {} environment variable is not set.".format(OUTCAST_PKGS_UVERSE()) )
#        args['--uverse'] = uverse
    
#    args['--uverse'] = standardize_dir_sep( os.path.realpath(args['--uverse']) )  

#    if ( not os.path.exists( args['--uverse'] ) ):
#        exit( "ERROR: Invalid path - {} - for native Outcast universe directory".format( args['--uverse'] ) ) 
    
    
#def set_packages_dir( args, skipenv=False ):
#    if ( skipenv ):
#        return
        
#    # Check the environment variable 
#    if ( not args['-p'] ):
#        pkgroot = os.environ.get( OUTCAST_PKGS_ROOT() )
#        if ( pkgroot == None ):
#            exit( "ERROR: The {} environment variable is not set.".format(OUTCAST_PKGS_ROOT()) )
#        args['-p'] = pkgroot
            
#    args['-p'] = standardize_dir_sep( os.path.realpath(args['-p']) )  

#    if ( not os.path.exists( args['-p'] ) ):
#        exit( "ERROR: Invalid path - {} - for the Packages directory".format( args['-p'] ) ) 
    
    
#def set_user_name( args, skipenv=False ):
#    if ( skipenv ):
#        return
        
#    # Get user name from the environment
#    if ( not args['--user'] ):
#        user = os.environ.get( OUTCAST_USER_NAME() )
#        if ( user == None ):
#            exit( "ERROR: The {} environment variable is not set.".format(OUTCAST_USER_NAME()) )
#        args['--user'] = user
    
        
#def set_password( args ):
#    # Set an empty/blank password when not explicitly specified
#    if ( not args['--passwd'] ):
#        args['--passwd'] = 'none'   
                    

##-----------------------------------------------------------------------------
#def standardize_dir_sep( pathinfo ):
#    return pathinfo.replace( '/', os.sep ).replace( '\\', os.sep )
    

#def epoch_secs_to_short_local( epoch_secs ):
#    sec = int(epoch_secs)
#    return time.strftime("%Y/%m/%d %H:%M:%S", time.localtime(sec))


#def short_local_to_epoch_secs( string ):
#    return int( time.mktime( time.strptime( string, "%Y/%m/%d %H:%M:%S" ) ) )
    
    

##-----------------------------------------------------------------------------
#def get_pkg_source( pkg_symlink, pkg_root, wrk_root=None, suffix=None ):
#    info   = symlinks.get_link_source( pkg_symlink )
#    result = info.replace( pkg_root, '', 1  )
#    if ( wrk_root != None ):
#        if ( info.startswith(wrk_root) ):
#            result = info.replace( wrk_root, '', 1 ) + os.sep + suffix

#    return result
  
##-----------------------------------------------------------------------------
#def mkdirs( dirpath ):
    
#    # Attempt to create the workspace directory    
#    try:
#        os.makedirs(dirpath)
#    except OSError as exc: 
#        if exc.errno == errno.EEXIST and os.path.isdir(dirpath):
#            pass
#        else: raise

    
#def cat_file( fobj, strip_comments=True, strip_blank_lines=True ):
#    # process all entries in the file        
#    for line in fobj:
       
#        # drop comments and blank lines
#        line = line.strip()
#        if ( strip_comments and line.startswith('#') ):
#            continue
#        if ( strip_blank_lines and line == '' ):
#            continue

#        if ( type(line) is bytes ):
#            line = line.decode()

#        print( line )
        
        
##-----------------------------------------------------------------------------
#def parse_pattern( string ):
#    # default result values
#    parent_dir = ''
#    pattern    = string
#    rflag      = False
    
#    # Search for supported wildcards 
#    idx = string.find('...')
#    if ( idx != -1 ):
#        rflag = True
#    else:
#        idx = string.find('?')
#        if ( idx == -1 ):
#            idx = string.find('[')
#            if ( idx == -1 ):
#                idx = string.find('*')
                
                
#    # Seperate parent directory from pattern/filename
#    if ( idx != -1 ):
#        end_idx    = string.rindex(os.sep,0,idx)
#        parent_dir = string[:end_idx]
#        if ( rflag ):
#            pattern = string[end_idx+1+3:] 
#        else:    
#            pattern = string[end_idx+1:]
        
#    return (idx!=-1, rflag, parent_dir, pattern )


##------------------------------------------------------------------------------
#def parse_entry( line, pkgpath ):
#    file_include = False
#    if ( line.startswith('!') ):
#        line         = line[1:]
#        file_include = True
        
#    has_pattern, rflag, parent_dir, pattern = parse_pattern( line )
#    path  = pkgpath + os.sep + parent_dir
#    files = []

#    # Process a 'normal' entry 
#    if ( not file_include ):
#        if ( not has_pattern ):
#            files.append( pkgpath + os.sep + pattern )
#        elif ( not rflag ):
#            files.extend( get_file_list( pattern, path ) )
#        else:
#            files.extend( walk_file_list( pattern, path ) )

#    # Process a 'include' entry (reference to another export header file)
#    else:
#        if ( not has_pattern ):
#            read_export_header_file( pkgpath + os.sep + pattern, pkgpath, files )
#        elif ( not rflag ):
#            export_files = get_file_list( pattern, path )
#            for f in export_files:
#                read_export_header_file( f, pkgpath, files )
#        else:
#            export_files = walk_file_list( pattern, path )
#            for f in export_files:
#                read_export_header_file( f, pkgpath, files )
        
#    return files
        
        
##-----------------------------------------------------------------------------
#def get_file_list( pattern, parentdir ):
#    list  = []
#    if ( os.path.exists( parentdir ) ):
#        files = os.listdir(parentdir)
#        for f in fnmatch.filter(files,pattern):
#            list.append( os.path.join(parentdir,f) )
      
#    return list
    
        
##-----------------------------------------------------------------------------
#def concatenate_commands( cmd1, cmd2 ):
#    if ( platform.system() == 'Windows' ):
#        return cmd1 + " & " + cmd2
#    else:
#        return cmd1 + " , " + cmd2


##-----------------------------------------------------------------------------
#def read_export_header_file( hfile, pkgpath, headers ):         
#    try:
#        inf = open( hfile, 'r' )

#        # process all entries in the file        
#        for line in inf:
           
#            # drop comments and blank lines
#            line = line.strip()
#            if ( line.startswith('#') ):
#                continue
#            if ( line == '' ):
#                continue
       
#            # 'normalize' the file entries
#            line = standardize_dir_sep( line.strip() )
        
#            # Process 'add' entries
#            if ( not line.startswith('-') ):
#                headers.extend( parse_entry(line,pkgpath) )
            
#                # make sure there are not duplicates in the list
#                headers = list(set(headers))
     
#            # Process 'exclude' entries
#            else:    
#                xlist = parse_entry(line[1:],pkgpath)
#                for e in xlist:
#                    try:
#                        headers.remove(e)
#                    except:
#                        pass
    
#        inf.close()
#    except:
#        print_warning( "Unable to open export header file: {}".format(hfile) )
        
#    return headers
    
##-----------------------------------------------------------------------------
#def walk_linksrc( linksrc, parentpath, common_args ):
#    list = []
#    for root, dirs, files in os.walk(parentpath):
#        for f in files:
#            info    = get_pkg_source( os.path.join(root,f), common_args['-p'], common_args['-w'], OUTCAST_LOCAL_PKG_SUFFIX() )
#            display = info
#            if ( OUTCAST_LOCAL_PKG_SUFFIX() in info ):
#                parts   = info.split(os.sep)
#                path    = os.path.join( *parts[1:-1] )
#                info    = os.path.join( common_args['-w'], path )
#                display = os.path.join( OUTCAST_LOCAL_PKG_SUFFIX(), path )
                
#            if ( info.startswith(linksrc) ):
#                list.append( (os.path.join(root,f), display) ) 
            
#    return list
    
##-----------------------------------------------------------------------------
#def walk_clean_empty_dirs( path ):
#    for dirpath, _, _ in os.walk(path, topdown=False):
#        if dirpath == path:
#            break
                
#        try:
#            os.rmdir(dirpath)
#        except OSError as ex:
#            if ( ex.errno == errno.ENOTEMPTY ):
#                pass

##-----------------------------------------------------------------------------
#class Node(object):
#    def __init__(self, data ):
#        self.data     = data
#        self.nodenum  = 0
#        self.parent   = None
#        self.children = []

#    def __repr__(self, level=0):
#        ret = "{} {}\n".format( "  "*level, repr(self.data) )
#        for child in self.children:
#            ret += child.__repr__(level+1)
#        return ret
    
       
#    def add_child_node( self, obj ):
#        self.children.append( obj )
#        obj.parent = self
        
#    def remove_child_node( self, obj ):
#        self.children.remove(obj)
#        obj.parent = None
            
#    def remove_all_children_nodes( self ):
#        for c in self.children:
#            c.parent = None
#        self.children = []
        
#    def add_child_data( self, data ):
#        c = Node(data)
#        self.add_child_node( c )
        
#    def add_children_nodes( self, list ):
#        for o in list:
#            self.add_child_node( o )
            
#    def add_children_data( self, list ):
#        for d in list:
#            self.add_child_data( d )
            
#    def get_children( self ):
#        return self.children    
        
#    def get_data( self ):
#        return self.data
            
#    def set_data( self, newdata ):
#        self.data = newdata
        
#    def set_nodenum( self, num ):
#        self.nodenum = num
        
#    def get_nodenum( self ):
#        return self.nodenum
        

#    def set_node_number_by_height( self, num ):
#        queue = deque()
#        queue.append( self )
#        while( len(queue) > 0 ):
#            node         = queue.popleft()
#            node.nodenum = num
#            num         += 1
#            for c in node.children:
#                queue.append(c)
    
#    def clone_sub_tree_from( self, src ):
#        self.data     = copy.deepcopy(src.data)
#        self.nodenum  = 0
#        self.children = []
#        self._clone_children_from( src )
        
            
#    def _clone_children_from( self, src ):
#        for x in src.children:
#            newnode = Node(None)
#            self.add_child_node( newnode )
#            newnode.clone_sub_tree_from( x )
                             
                               
##-----------------------------------------------------------------------------
#def find_duplicates_in_list(l):
#    """ Returns a list of duplicate entries.  If no duplicates, then an empty list is returned"""
#    return list(set([x for x in l if l.count(x) > 1]))


#def remove_duplicates( l ):
#    return list(set(l))
        
#def remove_from_list( item, l ):
#    if ( item in l ):
#        l.remove(item)
        
##-----------------------------------------------------------------------------
#def run_shell( cmd, verbose_flag=False, on_err_msg=None ):
#    if ( verbose_flag ):
#        print_verbose(cmd)
#        p = subprocess.Popen( cmd, shell=True )
#    else:
#        p = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )

#    r  = p.communicate()
#    r0 = '' if r[0] == None else r[0].decode()
#    r1 = '' if r[1] == None else r[1].decode()
#    if ( p.returncode != 0 and on_err_msg != None ):
#        exit(on_err_msg)
    
#    return (p.returncode, "{} {}".format(r0,r1) )


##-----------------------------------------------------------------------------
#def parse_vernum( string ):
#    pre = None
#    t   = string.split('.')
#    if ( len(t) < 3 or len(t) > 3 ):
#        exit( "ERROR: Malformed version number: [{}].".format( string ) )
        
#    # Parse pre-release (if it exists)
#    t2 = t[2].split('-')
#    if ( len(t2) == 2 ):
#        pre  = t2[1]
#        t[2] = t2[0]
#    elif ( len(t2) > 2 ):
#        exit( "ERROR: Malformed version number (prerelease id): [{}]".format( string ) )
    
    
#    return (t[0], t[1], t[2], pre)
    
#def parse_package_name( string ):
#    # Returns a tuple: (pkg, branch, (maj.min.pat[-pre]), original_string )
#    pkg, branch, ver = string.split('-',2) 
#    return (pkg, branch, ver, string )

#def build_vernum( m, n, p, pre=None ):
#    ver = "{}.{}.{}".format(m,n,p)
#    if ( pre != None and pre != ''):
#        ver = "{}-{}".format(ver,pre)
       
#    return ver

#def numerically_compare_versions( ver1, ver2 ): # Returns -1, 0, 1 when ver1<ver2, ver1==ver2, ver1>ver2 respectively
#    ver1_major, ver1_minor, ver1_patch, ver1_pre = parse_vernum( ver1 )
#    ver2_major, ver2_minor, ver2_patch, ver2_pre = parse_vernum( ver2 )
    
#    # Compare major indexes
#    if ( ver1_major < ver2_major ):
#        return -1
#    if ( ver1_major > ver2_major ):
#        return 1
    
#    # Compare minor indexes (If I get here the major indexes are equal)
#    if ( ver1_minor < ver2_minor ):
#        return -1
#    if ( ver1_minor > ver2_minor ):
#        return 1
            
#    # Compare patch indexes (If I get here the minor indexes are equal)
#    if ( ver1_patch < ver2_patch ):
#        return -1
#    if ( ver1_patch > ver2_patch ):
#        return 1
            
#    # Compare prerel indexes (If I get here the patch indexes are equal)
#    if ( ver1_pre < ver2_pre ):
#        return -1
#    if ( ver1_pre > ver2_pre ):
#        return 1
            
#    # If I get there - both versions are numerically equal
#    return 0
    
            
#def is_ver1_backwards_compatible_with_ver2( newer, older, newer_bhist=None ):
#    pkg_older, branch_older, ver_older = older
#    pkg_newer, branch_newer, ver_newer = newer
    
    
#    # Same branch...
#    if ( branch_older == branch_newer ):
#        return _test_compatible( ver_older, ver_newer )
    
#    # Different branches, but NO branch history provided
#    elif ( newer_bhist == None ):
#        # Withouth branch history, there is no way to determine compatibility
#        return False   

#    # Reverse traversal of branch history looking for compatibility
#    else:
#        # Walk the tree 'backwards'
#        nodes, first, last = bhist
#        return _inspect_parents( branch_newer, ver_newer, branch_older, ver_older, last )

        
#def increment_version( base_ver, inc_patch=False, inc_minor=False, inc_major=False, new_prelease=None ):
#    m,n,p,pre = parse_vernum( base_ver )
#    if ( inc_major ):
#        m   = int(m) + 1
#        n   = 0
#        p   = 0
#        pre = ''
#    elif ( inc_minor ):
#        n   = int(n) + 1
#        p   = 0
#        pre = ''
#    elif ( inc_patch ):
#        p   = int(p) + 1
#        pre = ''

#    if ( new_prelease != None ):
#        pre = new_prelease
        
#    return build_vernum( m, n, p, pre )            

#def _inspect_parents( now_bname, now_ver, older_bname, older_ver, curnode, level=0 ):
    
#    # Verbose info
#    if ( level == 0 ):
#        print_verbose("BEGIN branch compatibility traversal for target/older node: {} {}".format(older_bname, older_ver) )
#    print_verbose( "{:>2} : curnode={}, now={} {}".format(level, curnode, now_bname, now_ver ) )
#    level += 1
    
#    # NOTE: There are two scenarios for what my "parent" link means:
#    #       1) Direct lineage, i.e. same branch just older publish point.  At this
#    #          point I need to check if there was compatibility break between the
#    #          two publish points.  And since we are still on the same branch, the
#    #          semantic versioning is still valid
#    #       2) Start of a new branch.  This is potentially problematic because I
#    #          now need to test compatibility across branches which breaks 
#    #          semantic versioning.  HOWEVER, the branching/version rules for when
#    #          starting a new branch is that the 1st publish point on the new 
#    #          branch is based/derived from the origin line.  What this means is
#    #          in this very specific use case it is OKAY to compare version across
#    #          branches.
   
#    # Trap the case of a 'new branch'
#    if ( now_bname != curnode.name ):
#        if ( curnode.name == older_bname ):
#            return _test_compatible( older_ver, now_ver )

#    # Check if I am compatible with latest branch node on my direct lineage
#    if ( not _test_compatible( curnode.version, now_ver ) ):
#        return False
    
#    # Stepparent -->i.e. go back via merge path
#    if ( curnode.stepparent != None ):
#        if ( _get_merge_wgt( curnode.stepparent, curnode ) == 'major' ):
#            return False
        
#        # if the step-parent branch IS the 'right branch' -->do compatibility check immediately
#        if ( curnode.stepparent.name == older_bname ):
#            return _test_compatible( older_ver, curnode.stepparent.version )
        
#        # keep searching    
#        if ( _inspect_parents( curnode.stepparent.name, curnode.stepparent.version, older_bname, older_ver, curnode.stepparent, level ) ):
#            return True
                
#    # Go back in direct lineage
#    if ( curnode.parent != None ):
#        return _inspect_parents( curnode.name, curnode.version, older_bname, older_ver, curnode.parent, level )
#    else:
#        return False
        

#def _test_compatible( older, newer ):

#    older_major, older_minor, older_patch, older_pre = parse_vernum( older )
#    newer_major, newer_minor, newer_patch, newer_pre = parse_vernum( newer )
    
#    # Compare: Major
#    if ( older_major != newer_major ):
#        return False
    
#    # Compare: Minor
#    if ( older_minor > newer_minor ):
#        return False
    
#    # Compare: Patch
#    if ( (older_minor == newer_minor) and (older_patch > newer_patch) ):
#        return False

#    # Compare: Pre-release fields
#    if ( (older_minor == newer_minor) and (older_patch == newer_patch) ):
        
#        # Pre-release ALWAYS has less precendence than a 'normal' release
#        if ( newer_pre != None ):
#            if ( older_pre == None ):
#                return false
 
#            # TODO: Currently only support simple 'string compare' for the prerelease field
#            if ( older_pre > newer_pre ):
#                return False
      
#    # If I get here - the versions are compatible
#    return True


##-----------------------------------------------------------------------------
#def append_to_file( fd, filename ):
#    try:
#        f   = open( filename, "r" )
#        buf = f.read(1024)
#        while( buf != '' ):
#            fd.write(buf)
#            buf = f.read(1024)
#        f.close()
        
#    except Exception as ex:
#        exit( "ERROR: {}".format(ex) )        

##-----------------------------------------------------------------------------
#def print_tar_list( tar_file_name, verbose=False ):
#    try:
#        print( tar_file_name )
#        t = tarfile.open( tar_file_name )
#        t.list( verbose )
#        t.close()
#    except Exception as ex:
#        exit( "ERROR: Can't read archive file: {}. Error={}.".format( tar_file_name, ex ) )
        



##------------------------------------------------------------------------------
##
## BRANCHING SUPPORT
##
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
#def parse_branch_log( fobj, fname='' ):
    
#    nodes    = {}
#    last     = None
#    branches = set()

#    # process all entries in the file        
#    for line in fobj:
       
#        # drop comments and blank lines
#        line = line.strip()
#        if ( line.startswith('#') ):
#            continue
#        if ( line == '' ):
#            continue
   
#        # tokenize the line
#        tokens = line.split()
    
#        # Parse BRANCH entries
#        if ( tokens[0] == 'BRANCH' and tokens[2] == 'TO' ):
        
#            # create new nodes (but only when needed)
#            src = _get_node( nodes, tokens[1] )
#            dst = _get_node( nodes, tokens[3] )
    
#            # capture the semantics from the entry
#            src.add_branch( dst )
#            dst.set_parent( src )

#            # House keeping
#            last = dst
#            branches.add( src.name )
#            branches.add( dst.name )
                            
#        # Parse MERGE entries
#        elif ( tokens[0] == 'MERGE' and tokens[5] == 'TO' ):
        
#            # Parse 'weight' fields
#            wgt_stp = _parse_wgt(tokens[2])
#            wgt_par = _parse_wgt(tokens[4]) 
#            if ( wgt_stp == None ):
#                print_warning( "Invalid entry - bad <src-wgt> [{}]".format(line) )
#                continue
#            if ( wgt_par == None ):
#                print_warning( "Invalid entry - bad <dst-wgt> [{}]".format(line) )
#                continue
            
#            # create new nodes (but only when needed)
#            step   = _get_node( nodes, tokens[1] )
#            parent = _get_node( nodes, tokens[3] )
#            dst    = _get_node( nodes, tokens[6] ) 

#            # capture the semantics from the entry
#            step.add_merged_to( dst, wgt_stp )
#            parent.set_child( dst )
#            parent.set_wgt( wgt_par )
#            dst.set_parent( parent )
#            dst.set_stepparent( step )
            
#            # House keeping
#            last = dst
#            branches.add( parent.name )
#            branches.add( step.name )
#            branches.add( dst.name )
                            
#        # Spit out a warning for invalid entries
#        else:
#            print_warning( "Invalid entry [{}]".format(line) )
                


#    first, blists = _derive_child_links( nodes, branches )
#    if ( first == None ):
#        exit( "ERROR: There is no valid 'root' to the branch history in file:{}.  The branch history MUST start with a BRANCH entry from 'main'.".format(fname) )
#    return (nodes, first, last, blists)

    
#def get_branch_and_min_ver_info( fobj, fname='' ):
#    # process all entries in the file        
#    last = None
#    for line in fobj:
       
#        # drop comments and blank lines
#        line = line.strip()
#        if ( line.startswith('#') ):
#            continue
#        if ( line == '' ):
#            continue
   
#        # tokenize the line
#        tokens = line.split()
    
#        # Parse BRANCH entries
#        if ( tokens[0] == 'BRANCH' and tokens[2] == 'TO' ):
#            last = tokens[3]
                            
#        # Parse MERGE entries
#        elif ( tokens[0] == 'MERGE' and tokens[5] == 'TO' ):
#            last = tokens[6]
                            
#        # Spit out a warning for invalid entries
#        else:
#            print_warning( "Invalid entry [{}]".format(line) )
                
#    # Spit an warning if no entries found
#    if ( last == None ):
#        print_warning( "No valid entries in the branch history file: {}.  Default to 'NO BRANCHES'.".format(fname) )
#        return (None, None)
        
#    return standardize_dir_sep(last).split(os.sep)   
     
        
##------------------------------------------------------------------------------
#def _derive_child_links( nodes, branches ):

#    # set child links one branch at time
#    fist  = None
#    lists = []
#    for b in branches:
#        l = []
#        for n in nodes.values():
#            if ( n.name == b ):
#                l.append( n )
        
#        # Sort oldest to newest
#        l.sort(BranchNode.cmp)
        
#        # update child pointers
#        next  = 1
#        count = len(l)
#        for i in l:
#            if ( next < count ): 
#                child = l[next].fullname
#                i.set_child( nodes[child] )
#                i.child.set_parent( i )
#            next += 1
                
#        # capture the root of branch history (MUST be main/* something by definition)
#        if ( b == 'main' ):
#            first = l[0]
#            lists.insert(0,l)
#        else:
#            lists.append( l )
            
#    return first, lists
                            
#def _get_node( nodes, name_ver, create_if_not_found=True ):
#    key = standardize_dir_sep(name_ver)
#    if ( key in nodes ):
#        return nodes[key]
#    elif ( create_if_not_found ):
#        n = BranchNode( key )
#        nodes[key] = n
#        return n
#    else:
#        return None

#def _parse_wgt( token ):
#    w = token.strip("()")
#    if ( w == 'major' ):
#        return w
#    if ( w == 'minor' ):
#        return w
#    if ( w == 'patch' ):
#        return w
#    if ( w == 'zero' ):
#        return w
        
#    return None
       
##------------------------------------------------------------------------------
#def print_branch_history( bhist ):
#    nodes, first, last, blists = bhist
    
#    for b in blists:
#        print_single_branch_history( b, nodes )
#        print()

#def print_single_branch_history( blist, nodes ):
#    for i in blist:
#        if ( i.stepparent != None ):
#            print( " "*4, "<-- MERGE ", i.stepparent.fullname, "({})".format( _get_merge_wgt(i.stepparent,i) ), "({})".format(i.parent.wgt), )
#        print( i.fullname )
#        if ( len(i.branches) > 0 ):
#            for b in i.branches:
#                print( " "*4, "BRANCH -->", b.fullname )
#        if ( len(i.mergedto) > 0 ):
#            for m in i.mergedto:
#                print( " "* 4, "MERGE  -->", m.fullname )

#def _get_merge_wgt( src, dst ):
#    idx = 0
#    for i in src.mergedto:
#        if ( i.fullname == dst.fullname ):
#            return src.merge_wgts[idx]
#        idx += 1
        
#    return None
                           
##------------------------------------------------------------------------------
#def create_branch_dot_file( fname, bhist ):
#    with open(fname, 'w' ) as f:
#        f.write( "digraph { \n" )
        
#        nodes, first, last, blists = bhist
#        for branch in blists:
#            for n in branch:
#                p = n.fullname.replace(os.sep,"/")
#                if ( n.child ):
#                    value = None
#                    if ( n.wgt != None ):
#                        value = n.wgt
#                    _write_trans( f, p, n.child.fullname, bold=True, label=value )
#                for b in n.branches:
#                    _write_trans( f, p, b.fullname, arrow_empty=True )
#                for m in n.mergedto:
#                    _write_trans( f, p, m.fullname, dashed=True, arrow_empty=True, label=_get_merge_wgt(n,m) )

#        f.write( "}\n" )


#def _write_trans( fd, p, childname, bold=False, solid=True, dotted=False, dashed=False, arrow_empty=False, arrow_normal=True, label=None ):
#    c = childname.replace(os.sep,"/")
#    fd.write( '"{}" -> "{}"'.format(p,c) )
#    if ( bold ):
#        fd.write( ' [style=bold]' )
#    elif( dotted ):
#        fd.write( ' [style=dotted]' )
#    elif( dashed ):
#        fd.write( ' [style=dashed]' )
#    elif( solid ):
#        fd.write( ' [style=solid]' )
    
#    if ( arrow_empty ):
#        fd.write( ' [arrowhead=empty]' )
#    elif ( arrow_normal ):
#        fd.write( ' [arrowhead=normal]' )
    
#    if ( label != None ):
#        fd.write( ' [label={}]'.format(label) )
                
#    fd.write( ' \n' )

##------------------------------------------------------------------------------
#class BranchNode:
#    def __init__(self,name_and_version=None):
#        self.name       = ''
#        self.version    = ''
#        self.branches   = []
#        self.child      = None
#        self.mergedto   = []
#        self.merge_wgts = []
#        self.parent     = None
#        self.stepparent = None
#        self.wgt        = None
        

#        # Set name & version when provided: format is "<name>/<vernum>"
#        if ( name_and_version != None ):
#            self.name, self.version = standardize_dir_sep(name_and_version).split(os.sep)
        
#        self.fullname = self.name + os.sep + self.version
            
#    def add_branch(self, branch_node):
#        self.branches.append( branch_node )
        
#    def add_merged_to(self, merge_node, merge_wgt ):
#        self.mergedto.append( merge_node )
#        self.merge_wgts.append( merge_wgt )
        
#    def set_parent(self, parent ):
#        self.parent = parent
        
#    def set_stepparent(self, stepparent ):
#        self.stepparent = stepparent
        
#    def set_child(self, child ):
#        self.child = child
    
#    def set_wgt(self, wgt ):
#        self.wgt = wgt
        
#    def cmp(self,other):
#        if ( other != None ):
#            return numerically_compare_versions(self.version, other.version)
#        return NotImplemented

#    def __str__(self):
#        return self.fullname
        
#    def _repr__(self):
#        return self.fullname
        
#    def print_all(self):
#        print( "{}/{}: P={}, C={}, wgt={}. Branches={}, Merges={}, MWgts={}".format( self.name, self.version, self.parent, self.child, self.wgt, self.branches, self.mergedto, self.merge_wgts ) )

#        self.name       = ''
#        self.version    = ''
#        self.branches   = []
#        self.child      = None
#        self.mergedto   = []
#        self.merge_wgts = []
#        self.parent     = None
#        self.stepparent = None
#        self.wgt        = None
        

                 
##------------------------------------------------------------------------------
#now   = 0
#local = None

#def mark_time( current_time = False ):
#    global now, local
#    if ( not current_time ):
#        current_time = time.time()
        
#    now       = int( current_time ) 
#    local     = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime(now))
#    return now, local

#def get_marked_time():
#    global now, local
#    return now, local
    
    
##------------------------------------------------------------------------------
#def update_journal_publish( journal_file, user, summary_comment, comment_file, version, branch_name, scm_label, exit_on_error=True ):
#    return _update_journal( journal_file, comment_file, _create_journal_publish_marker( summary_comment, user, version, branch_name, scm_label), exit_on_error  )
 
    
#def update_journal_promote( journal_file, user, summary_comment, comment_file, version, branch_name, from_package_nbv, exit_on_error=True  ):
#    return _update_journal( journal_file, comment_file, _create_journal_promote_marker( summary_comment, user, version, branch_name, from_package_nbv), exit_on_error  )
 
    
#def update_journal_deprecate( journal_file, user, summary_comment, comment_file, version, branch_name, exit_on_error=True  ):
#    return _update_journal( journal_file, comment_file, _create_journal_deprecate_marker( summary_comment, user, version, branch_name), exit_on_error  )
    

#def update_journal_do_not_use( journal_file, user, summary_comment, comment_file, version, branch_name, exit_on_error=True  ):
#    return _update_journal( journal_file, comment_file, _create_journal_do_not_use_marker( summary_comment, user, version, branch_name), exit_on_error  )
    
    

#def _update_journal( journal_file, comment_file, marker_string, exit_on_error=True ):
#    try:
#        f = open( journal_file, "a" )
#        f.write( marker_string + "\n" )
#        if ( comment_file ):
#            append_to_file( f, comment_file )
#        f.write( "\n\n" )
#        f.close()
        
#    except Exception as ex:
#        if ( exit_on_error ):
#            exit( "ERROR: {}".format(ex) )        
#        return ex

#    return None        
        
#def _create_journal_publish_marker( summary, user, version, branch_name, scmlabel ):
#    now, local = get_marked_time()
#    return "{}; {}; {}; PUBLISH; {}; {}; {}; {}; {}".format( '~' * 5, now, local, user, version, branch_name, scmlabel, summary )

#def _create_journal_promote_marker( summary, user, version, branch_name, from_package_nbv ):
#    now, local = get_marked_time()
#    return "{}; {}; {}; PROMOTE; {}; {}; {}; {}; {}".format( '~' * 5, now, local, user, version, branch_name, scmlabel, from_package_nbv )

#def _create_journal_do_not_use_marker( summary, user, version, branch_name ):
#    now, local = get_marked_time()
#    return "{}; {}; {}; DO_NOT_USE; {}; {}; {}; {}".format( '~' * 5, now, local, user, version, branch_name, summary )

#def _create_journal_deprecate_marker( summary, user, version, branch_name ):
#    now, local = get_marked_time()
#    return "{}; {}; {}; DEPRECATE; {}; {}; {}; {}".format( '~' * 5, now, local, user, version, branch_name, summary )


##------------------------------------------------------------------------------
#def read_journal_file( file_name, fh=None ):
#    # Open local file
#    if ( fh == None ):
#        try:
#            fh = open( file_name, "r" )
#        except:
#            exit( "ERROR: File - {} - does not exist or is not a valid file name.".format( file_name ) )
        
#        print_verbose( "Reading file: " + file_name )
                
#    # Provided a file handle (e.g. reading the file from a tar file)
#    else:
#        utils.print_verbose( "Reading file: "  + file_name  )

 
#    # Scan file and load markers
#    publish_list     = []
#    promote_list     = []   
#    do_not_use_list  = []
#    deprecate_list   = []
#    linenum          = 0
#    for line in fh:
#        # skip ALL non-marker lines
#        linenum += 1
#        if ( not line.startswith('~' * 5) ):
#            continue    
         
#        # Break into fields
#        tokens = line.split(';')
#        tokens = [x.strip() for x in tokens]
#        if ( tokens[3] == "PUBLISH" ):
#            publish_list.append( tokens[1:] )
#        elif ( tokens[3] == "DO_NOT_USE" ):
#            do_not_use_list.append( tokens[1:] )
#        elif ( tokens[3] == "DEPRECATE" ):
#            deprecate_list.append( tokens[1:] )
#        elif ( tokens[3] == "PROMOTE" ):
#            promote_list.append( tokens[1:] )
#        else:
#            print_warning( "Invalid Journal marker at line {} [{}].".format( linenum, line ) )
 
#    # clean-up
#    fh.close()    

#    # return markers
#    markers = {}
#    markers['publish']    = publish_list
#    markers['promote']    = promote_list
#    markers['do_not_use'] = do_not_use_list
#    markers['deprecate']  = deprecate_list
#    return markers

    
#def extract_pbv_from_journal( journal, entry_type, pkgname ):
#    entries  = journal[entry_type]
#    pbv_list = []    
#    for e in entries:
#        pbv_list.append( (pkgname, e[5], e[4]) )

#    return pbv_list    
             

##------------------------------------------------------------------------------
#def workspace_namespaces_as_list( wrkspace ):
#    pkgs = os.listdir( wrkspace )
    
#    # Attempt to filter out all non-package directories
#    remove_from_list( OUTCAST_XINC_DIRNAME(), pkgs )
#    remove_from_list( OUTCAST_XPKGS_DIRNAME(), pkgs )
#    for p in pkgs:
#        if ( not p[0:1].isalpha() ):
#            pkgs.remove(p)

#    # Create a super list of namespaces
#    all     = []
#    pkgsall = []
#    for p in pkgs:
#        ns = read_namespaces_local( wrkspace, p )
#        if ( len(ns) > 0 ):
#            all.extend( ns )
#            pkgsall.append( (p,ns) )

#    return (all, pkgsall)
    
    
#def read_namespaces_local( wrkspace, pkgname ):
#    f = os.path.join( wrkspace, pkgname, 'top', 'pkg.namespaces' )
#    if ( not os.path.isfile( f ) ):
#        return []
        
#    try:
#        fd = open( f, 'r' )
#        ns = _read_namespaces( fd )
#        fd.close()
        
#    except Exception as ex:
#        exit( "ERROR: Unable to open/read pkg.namespaces file: {})".format(f,ex) )

#    return ns
    
        
#def read_namespaces_top( uverse, pkgnbv ):
    
#    # Open top file
#    f = os.path.join( uverse, pkgnbv + '.top' )
#    try:
#        tar = tarfile.open( f )
#        fh  = tar.extractfile( 'top/pkg.namespaces' )
    
#    except KeyError:
#        print_warning( "No pkg.namespace file in package: {}".format( f ) )
#        tar.close()
#        return []
        
#    except Exception as ex:
#        exit("ERROR: Trying to locate/read Package Top File: {}".format(f) )
    
#    # Read the namespaces file
#    ns  = _read_namespaces( fh )
#    tar.close()
#    return ns


#def _read_namespaces( fh ):
#    namespaces = []
#    for line in fh:
#        line = line.strip()
#        namespaces.append( line )    
    
#    return namespaces
