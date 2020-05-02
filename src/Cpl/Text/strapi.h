#ifndef Cpl_Text_strapi_h_
#define Cpl_Text_strapi_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

	This file attempts to provide a "standardized" set of C library functions
	that operate on C null terminated strings.  In a perfect world there
	would be no need for this interface since all compilers would follow
	the ISO standards - however there are some compilers (VS comes to mind)
	that still do no compile with C99 or C99 support as part of C++11.  It
	also facilities solutions for embedded platforms that need alternate
	implementation of snprintf.

 */

#include "colony_map.h"



 ///////////////////////////////////////////////////////////////////////////////

 /** Compares two string ignoring case.  It has the same semantics as
	 strcmp.

	 \b Prototype:
		 int strcasecmp(const char *s1, const char *s2);
  */
#ifndef HAVE_CPL_TEXT_STRCASECMP
#define strcasecmp          strcasecmp_MAP
#endif


  /** Same as strcasecmp, but only compares up to 'n' bytes.  It has the same
	  semantics as strncmp.

	  \b Prototype:
		  int strncasecmp(const char *s1, const char *s2, size_t n);
   */
#ifndef HAVE_CPL_TEXT_STRNCASECMP
#define strncasecmp         strncasecmp_MAP
#endif


   /** Converts the entire string to upper case letters. Returns the
	   converted string (i.e. same pointer that is/was passed in)

	   \b Prototype:
		   char* strupr( char* s1 );
	*/
#ifndef HAVE_CPL_TEXT_STRUPR
#define strupr              strupr_MAP
#endif


	/** Converts the entire string to lower case letters. Returns the
		converted string (i.e. same pointer that is/was passed in)

		\b Prototype:
			char* strlwr( char* s1 );
	 */
#ifndef HAVE_CPL_TEXT_STRLWR
#define strlwr              strlwr_MAP
#endif

	 /** Fill a string with a given character. Returns the
		 converted string (i.e. same pointer that is/was passed in)

		 \b Prototype:
			 char* strset( char* s1, int fill );
	  */
#ifndef HAVE_CPL_TEXT_STRSET
#define strset              strset_MAP
#endif

	  /** Fill a string with a given character, to a given length. Returns the
		  converted string (i.e. same pointer that is/was passed in)

		  \b Prototype:
			  char* strnset( char* s1, int fill, size_t len );
	   */
#ifndef HAVE_CPL_TEXT_STRNSET
#define strnset             strnset_MAP
#endif


	   /** Reverse the specified string. Returns the reversed string (i.e. same
		   pointer that is/was passed in)

		   \b Prototype:
			   char* strrev( char* s1 );
		*/
#ifndef HAVE_CPL_TEXT_STRREV
#define strrev              strrev_MAP
#endif

		/** Break a string into tokens, i.e. same as strtok(), but strtok_r is a
			reentrant implementation. 'lasts' is the address of a pointer to a
			character, which the function can use to store information necessary
			for it to continue scanning the same string.

			\b Prototype:
				char* strtok_r( char* s, const char* sep, char** lasts );
		 */
#ifndef HAVE_CPL_TEXT_STRTOK_R
#define strtok_r            strtok_r_MAP
#endif

		 /** Same functionality as snprintf(), except at most 'count-1' character
			 will be stored in the output string.  The result is guaranteed to be
			 null terminated.

			 Returns the number of characters that would have been written into the
			 array, not counting the terminating null character, had count been
			 large enough. It does this even if count is zero; in this case buf can
			 be NULL. If an error occurred, snprintf() returns a negative value
			 and sets errno.

			 \b Prototype:
				 int snprintf( char* buf, size_t count, const char* format, ... );
		  */
#ifndef HAVE_CPL_TEXT_SNPRINTF
#define snprintf            snprintf_MAP
#endif

		  /** Same as snprintf except uses vaargs arguments.

			  \b Prototype:
				  int vsnprintf(char *str, size_t size, const char *format, va_list ap);
		   */
#ifndef HAVE_CPL_TEXT_VSNPRINTF      
#define vsnprintf           vsnprintf_MAP
#endif



#endif  // end header latch
