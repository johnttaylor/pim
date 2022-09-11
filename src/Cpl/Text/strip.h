#ifndef Cpl_Text_strip_h_
#define Cpl_Text_strip_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

    This file contains a collection of methods that provide basic building
    blocks for parsing 'tokens' with in a null terminated string. It also
    provide functions for removing leading/trailing whitespace.

 */



///
namespace Cpl {
///
namespace Text {


/** This method returns a pointer to the FIRST non-whitespace character in the
    the specified null-terminated string.
    NOTES:

        o This method does NOT modify the original string in any way!
        o If 's' is null, then null is returned.
 */
const char* stripSpace( const char* s );


/** This method returns a pointer to the FIRST whitespace character in the
    the specified null-terminated string.
    NOTES:

        o This method does NOT modify the original string in any way!
        o If 's' is null, then null is returned.


    This method is useful in finding the 'next' token in a string, for example: 
	@code

        // Returns a pointer to the first token in 'input'
        const char* token = stripSpace(input)

        // Returns a pointer to the second token in 'input'
        token = stripSpace(stripNotSpace(token))

	@endcode
 */
const char* stripNotSpace( const char* s );


/** This method returns a pointer to the LAST non-whitespace character in the
    the specified null-terminated string.

    NOTES:

        o This method does NOT modify the original string in any way!
        o If 's' is null, then null is returned.
        o If the entire string is whitespace, a pointer to the start of
          the string is returned (i.e. 's' returned)
 */
const char* stripTrailingSpace( const char* s );


/** This method TRUNCATES the specified null-terminated string by eliminating
    any trailing white space.

    NOTE: If 's' is null, then nothing is done.
 */
void removeTrailingSpace( char* s );


/** This method is the same as stripSpace(), except the specified character
    set is used to terminate the search instead of the standard isspace()
    characters.

    NOTES:

        o If 's' is null, then null is returned
        o If 'charsSet' is null, then 's' is returned and nothing is done.
 */
const char* stripChars( const char* s, const char* charsSet );


/** This method is the same as stripNotSpace(), except the specified character
    set is used to terminate the search instead of the standard isspace()
    characters.

    NOTES:

        o If 's' is null, then null is returned
        o If 'charsSet' is null, then 's' is returned and nothing is done.
 */
const char* stripNotChars( const char* s, const char* charsSet );

/** This method is the same as stripTrailingSpaces(), except the specified
    character set is used to identify the last "non-whitespace" character.

    NOTES:

        o If 's' is null, then null is returned
        o If 'charsSet' is null, then 's' is returned and nothing is done.
        o If the entire string is made up of 'charsSet', a pointer to the start
          of the string is returned (i.e. 's' returned)
 */
const char* stripTrailingChars( const char* s, const char* charsSet );

/** This method is the same as removeTrailingSpaces(), except the specified
    characters set is used for "whitespace"
    any trailing white space.

    NOTE: If 's' OR 'charsSet' is null, then nothing is done.
 */
void removeTrailingChars( char* s, const char* charsSet );


/** This method returns true if the character 'c' is one of
    character(s) contained in 'charsSet'; else false is
    returned.
 */
bool isCharInString( const char* charsSet, const char c );


};      // end namespaces
};
#endif  // end header latch
