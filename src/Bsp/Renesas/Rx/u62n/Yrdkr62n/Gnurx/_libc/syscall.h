#ifndef Bsp_libfake_syscall_h_
#define Bsp_libfake_syscall_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#define	BSP_LIBFAKE_SYS_exit	        1 
#define	BSP_LIBFAKE_SYS_open	        2 
#define	BSP_LIBFAKE_SYS_close	        3 
#define	BSP_LIBFAKE_SYS_read	        4 
#define	BSP_LIBFAKE_SYS_write	        5 
#define	BSP_LIBFAKE_SYS_lseek	        6 
#define	BSP_LIBFAKE_SYS_unlink	        7 
#define	BSP_LIBFAKE_SYS_getpid	        8 
#define	BSP_LIBFAKE_SYS_kill	        9 
#define	BSP_LIBFAKE_SYS_isatty	        10
#define BSP_LIBFAKE_SYS_fstat           11
#define BSP_LIBFAKE_SYS_sbrk	        12
#define BSP_LIBFAKE_SYS_argvlen	        13
#define BSP_LIBFAKE_SYS_argv	        14
#define BSP_LIBFAKE_SYS_chdir	        15
#define BSP_LIBFAKE_SYS_stat	        16
#define BSP_LIBFAKE_SYS_chmod 	        17
#define BSP_LIBFAKE_SYS_utime 	        18
#define BSP_LIBFAKE_SYS_time 	        19
#define BSP_LIBFAKE_SYS_gettimeofday    20
#define BSP_LIBFAKE_SYS_times	        21
#define BSP_LIBFAKE_SYS_link	        22
#define BSP_LIBFAKE_SYS_argc	        23
#define BSP_LIBFAKE_SYS_argnlen	        24
#define BSP_LIBFAKE_SYS_argn	        25
#define BSP_LIBFAKE_SYS_reconfig	    26



/// Error Handler. 'n' is one of the constants above
void bsp_libfake_fatalerror( int n );



#ifdef __cplusplus
};
#endif

#endif  // end header latch
