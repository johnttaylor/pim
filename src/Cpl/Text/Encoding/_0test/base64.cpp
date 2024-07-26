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

#include "Catch/catch.hpp"
#include "Cpl/Text/Encoding/Base64.h"    
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/System/Trace.h"
#include <string.h>
#include <stdint.h>
#include "colony_map.h"

#define SECT_ "_0test"

/// 
using namespace Cpl::Text::Encoding;


////////////////////////////////////////////////////////////////////////////////


#define GOLDEN1_IN      "Many hands make light work."
#define GOLDEN1_OUT     "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"

#define GOLDEN2_IN      "hello world"
#define GOLDEN2_OUT     "aGVsbG8gd29ybGQ="

#define GOLDEN3_IN      "hello worl"
#define GOLDEN3_OUT     "aGVsbG8gd29ybA=="

#define GOLDEN4_OUT     "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+/wABAgMEBQYHCAkKCwwNDg8QERITFBUWFxgZGhscHR4fICEiIyQlJicoKSorLC0uLzAxMjM0NTY3ODk6Ozw9Pj9AQUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVpbXF1eX2BhYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ent8fX5/gIGCg4SFhoeIiYqLjI2Oj5CRkpOUlZaXmJmam5ydnp+goaKjpKWmp6ipqqusra6vsLGys7S1tre4ubq7vL2+v8DBwsPExcbHyMnKy8zNzs/Q0dLT1NXW19jZ2tvc3d7f4OHi4+Tl5ufo6err7O3u7/Dx8vP09fb3+Pn6+/z9/v8AAQIDBAUGBwgJCgsMDQ4PEBESExQVFhcYGRobHB0eHyAhIiMkJSYnKCkqKywtLi8wMTIzNDU2Nzg5Ojs8PT4/QEFCQ0RFRkdISUpLTE1OT1BRUlNUVVZXWFlaW1xdXl9gYWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXp7fH1+f4CBgoOEhYaHiImKi4yNjo+QkZKTlJWWl5iZmpucnZ6foKGio6SlpqeoqaqrrK2ur7CxsrO0tba3uLm6u7y9vr/AwcLDxMXGx8jJysvMzc7P0NHS09TV1tfY2drb3N3e3+Dh4uPk5ebn6Onq6+zt7u/w8fLz9PX29/j5+vv8/f7/AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+/wABAgMEBQYHCAkKCwwNDg8QERITFBUWFxgZGhscHR4fICEiIyQlJicoKSorLC0uLzAxMjM0NTY3ODk6Ozw9Pj9AQUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVpbXF1eX2BhYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ent8fX5/gIGCg4SFhoeIiYqLjI2Oj5CRkpOUlZaXmJmam5ydnp+goaKjpKWmp6ipqqusra6vsLGys7S1tre4ubq7vL2+v8DBwsPExcbHyMnKy8zNzs/Q"

const char* GOLDEN5_IN = R"(One of the parameters of the EdDSA algorithm is the "prehash"
function.This may be the identity function, resulting in an
algorithm called PureEdDSA, or a collision - resistant hash function
such as SHA - 512, resulting in an algorithm called HashEdDSA.

Choosing which variant to use depends on which property is deemed to
be more important between 1) collision resilience and 2) a single -
pass interface for creating signatures.The collision resilience
property means EdDSA is secure even if it is feasible to compute
collisions for the hash function.The single - pass interface property
means that only one pass over the input message is required to create
a signature.PureEdDSA requires two passes over the input.Many
existing APIs, protocols, and environments assume digital signature
algorithms only need one pass over the input and may have API or
bandwidth concerns supporting anything else.

Note that single - pass verification is not possible with most uses of
signatures, no matter which signature algorithm is chosen.This is
because most of the time, one can't process the message until the
signature is validated, which needs a pass on the entire message.)";

#define GOLDEN5_OUT     "T25lIG9mIHRoZSBwYXJhbWV0ZXJzIG9mIHRoZSBFZERTQSBhbGdvcml0aG0gaXMgdGhlICJwcmVoYXNoIgpmdW5jdGlvbi5UaGlzIG1heSBiZSB0aGUgaWRlbnRpdHkgZnVuY3Rpb24sIHJlc3VsdGluZyBpbiBhbgphbGdvcml0aG0gY2FsbGVkIFB1cmVFZERTQSwgb3IgYSBjb2xsaXNpb24gLSByZXNpc3RhbnQgaGFzaCBmdW5jdGlvbgpzdWNoIGFzIFNIQSAtIDUxMiwgcmVzdWx0aW5nIGluIGFuIGFsZ29yaXRobSBjYWxsZWQgSGFzaEVkRFNBLgoKQ2hvb3Npbmcgd2hpY2ggdmFyaWFudCB0byB1c2UgZGVwZW5kcyBvbiB3aGljaCBwcm9wZXJ0eSBpcyBkZWVtZWQgdG8KYmUgbW9yZSBpbXBvcnRhbnQgYmV0d2VlbiAxKSBjb2xsaXNpb24gcmVzaWxpZW5jZSBhbmQgMikgYSBzaW5nbGUgLQpwYXNzIGludGVyZmFjZSBmb3IgY3JlYXRpbmcgc2lnbmF0dXJlcy5UaGUgY29sbGlzaW9uIHJlc2lsaWVuY2UKcHJvcGVydHkgbWVhbnMgRWREU0EgaXMgc2VjdXJlIGV2ZW4gaWYgaXQgaXMgZmVhc2libGUgdG8gY29tcHV0ZQpjb2xsaXNpb25zIGZvciB0aGUgaGFzaCBmdW5jdGlvbi5UaGUgc2luZ2xlIC0gcGFzcyBpbnRlcmZhY2UgcHJvcGVydHkKbWVhbnMgdGhhdCBvbmx5IG9uZSBwYXNzIG92ZXIgdGhlIGlucHV0IG1lc3NhZ2UgaXMgcmVxdWlyZWQgdG8gY3JlYXRlCmEgc2lnbmF0dXJlLlB1cmVFZERTQSByZXF1aXJlcyB0d28gcGFzc2VzIG92ZXIgdGhlIGlucHV0Lk1hbnkKZXhpc3RpbmcgQVBJcywgcHJvdG9jb2xzLCBhbmQgZW52aXJvbm1lbnRzIGFzc3VtZSBkaWdpdGFsIHNpZ25hdHVyZQphbGdvcml0aG1zIG9ubHkgbmVlZCBvbmUgcGFzcyBvdmVyIHRoZSBpbnB1dCBhbmQgbWF5IGhhdmUgQVBJIG9yCmJhbmR3aWR0aCBjb25jZXJucyBzdXBwb3J0aW5nIGFueXRoaW5nIGVsc2UuCgpOb3RlIHRoYXQgc2luZ2xlIC0gcGFzcyB2ZXJpZmljYXRpb24gaXMgbm90IHBvc3NpYmxlIHdpdGggbW9zdCB1c2VzIG9mCnNpZ25hdHVyZXMsIG5vIG1hdHRlciB3aGljaCBzaWduYXR1cmUgYWxnb3JpdGhtIGlzIGNob3Nlbi5UaGlzIGlzCmJlY2F1c2UgbW9zdCBvZiB0aGUgdGltZSwgb25lIGNhbid0IHByb2Nlc3MgdGhlIG1lc3NhZ2UgdW50aWwgdGhlCnNpZ25hdHVyZSBpcyB2YWxpZGF0ZWQsIHdoaWNoIG5lZWRzIGEgcGFzcyBvbiB0aGUgZW50aXJlIG1lc3NhZ2Uu"

const char* GOLDEN6_OUT = R"(T25lIG9mIHRoZSBwYXJhbWV0ZXJzIG9mIHRoZSBFZERTQSBhbGdvcml0aG0gaXMgdGhlICJwcmVo
YXNoIgpmdW5jdGlvbi5UaGlzIG1heSBiZSB0aGUgaWRlbnRpdHkgZnVuY3Rpb24sIHJlc3VsdGlu
ZyBpbiBhbgphbGdvcml0aG0gY2FsbGVkIFB1cmVFZERTQSwgb3IgYSBjb2xsaXNpb24gLSByZXNp
c3RhbnQgaGFzaCBmdW5jdGlvbgpzdWNoIGFzIFNIQSAtIDUxMiwgcmVzdWx0aW5nIGluIGFuIGFs
Z29yaXRobSBjYWxsZWQgSGFzaEVkRFNBLgoKQ2hvb3Npbmcgd2hpY2ggdmFyaWFudCB0byB1c2Ug
ZGVwZW5kcyBvbiB3aGljaCBwcm9wZXJ0eSBpcyBkZWVtZWQgdG8KYmUgbW9yZSBpbXBvcnRhbnQg
YmV0d2VlbiAxKSBjb2xsaXNpb24gcmVzaWxpZW5jZSBhbmQgMikgYSBzaW5nbGUgLQpwYXNzIGlu
dGVyZmFjZSBmb3IgY3JlYXRpbmcgc2lnbmF0dXJlcy5UaGUgY29sbGlzaW9uIHJlc2lsaWVuY2UK
cHJvcGVydHkgbWVhbnMgRWREU0EgaXMgc2VjdXJlIGV2ZW4gaWYgaXQgaXMgZmVhc2libGUgdG8g
Y29tcHV0ZQpjb2xsaXNpb25zIGZvciB0aGUgaGFzaCBmdW5jdGlvbi5UaGUgc2luZ2xlIC0gcGFz
cyBpbnRlcmZhY2UgcHJvcGVydHkKbWVhbnMgdGhhdCBvbmx5IG9uZSBwYXNzIG92ZXIgdGhlIGlu
cHV0IG1lc3NhZ2UgaXMgcmVxdWlyZWQgdG8gY3JlYXRlCmEgc2lnbmF0dXJlLlB1cmVFZERTQSBy
ZXF1aXJlcyB0d28gcGFzc2VzIG92ZXIgdGhlIGlucHV0Lk1hbnkKZXhpc3RpbmcgQVBJcywgcHJv
dG9jb2xzLCBhbmQgZW52aXJvbm1lbnRzIGFzc3VtZSBkaWdpdGFsIHNpZ25hdHVyZQphbGdvcml0
aG1zIG9ubHkgbmVlZCBvbmUgcGFzcyBvdmVyIHRoZSBpbnB1dCBhbmQgbWF5IGhhdmUgQVBJIG9y
CmJhbmR3aWR0aCBjb25jZXJucyBzdXBwb3J0aW5nIGFueXRoaW5nIGVsc2UuCgpOb3RlIHRoYXQg
c2luZ2xlIC0gcGFzcyB2ZXJpZmljYXRpb24gaXMgbm90IHBvc3NpYmxlIHdpdGggbW9zdCB1c2Vz
IG9mCnNpZ25hdHVyZXMsIG5vIG1hdHRlciB3aGljaCBzaWduYXR1cmUgYWxnb3JpdGhtIGlzIGNo
b3Nlbi5UaGlzIGlzCmJlY2F1c2UgbW9zdCBvZiB0aGUgdGltZSwgb25lIGNhbid0IHByb2Nlc3Mg
dGhlIG1lc3NhZ2UgdW50aWwgdGhlCnNpZ25hdHVyZSBpcyB2YWxpZGF0ZWQsIHdoaWNoIG5lZWRz
IGEgcGFzcyBvbiB0aGUgZW50aXJlIG1lc3NhZ2Uu
)";


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "base64" )
{

    Cpl::System::Shutdown_TS::clearAndUseCounter();
    char   buf1[2048];
    char   buf2[2048];
    size_t outLen;

    SECTION( "golden" )
    {
        bool r = base64Encode( GOLDEN1_IN, strlen( GOLDEN1_IN ), buf1, sizeof( buf1 ), outLen );
        REQUIRE( r );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ENCODE: in=%s, outlen=%lu, out=[%.*s]", GOLDEN1_IN, outLen, outLen, buf1) );
        REQUIRE( strlen( buf1 ) == strlen( GOLDEN1_OUT ) );
        REQUIRE( strcmp( buf1, GOLDEN1_OUT ) == 0 );

        r = base64Decode( buf1, outLen, buf2, sizeof( buf2 ), outLen );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("DECODE: in=%s, outlen=%lu, out=[%.*s]", buf1, outLen, outLen, buf2) );
        REQUIRE( outLen == strlen( GOLDEN1_IN ) );
        REQUIRE( strncmp( buf2, GOLDEN1_IN, outLen ) == 0 );


        r = base64Encode( GOLDEN2_IN, strlen( GOLDEN2_IN ), buf1, sizeof( buf1 ), outLen );
        REQUIRE( r );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ENCODE: in=%s, outlen=%lu, out=[%.*s]", GOLDEN2_IN, outLen, outLen, buf1) );
        REQUIRE( strlen( buf1 ) == strlen( GOLDEN2_OUT ) );
        REQUIRE( strcmp( buf1, GOLDEN2_OUT ) == 0 );

        r = base64Decode( buf1, outLen, buf2, sizeof( buf2 ), outLen );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("DECODE: in=%s, outlen=%lu, out=[%.*s]", buf1, outLen, outLen, buf2) );
        REQUIRE( outLen == strlen( GOLDEN2_IN ) );
        REQUIRE( strncmp( buf2, GOLDEN2_IN, outLen ) == 0 );


        r = base64Encode( GOLDEN3_IN, strlen( GOLDEN3_IN ), buf1, sizeof( buf1 ), outLen );
        REQUIRE( r );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ENCODE: in=%s, outlen=%lu, out=[%.*s]", GOLDEN3_IN, outLen, outLen, buf1) );
        REQUIRE( strlen( buf1 ) == strlen( GOLDEN3_OUT ) );
        REQUIRE( strcmp( buf1, GOLDEN3_OUT ) == 0 );

        r = base64Decode( buf1, outLen, buf2, sizeof( buf2 ), outLen );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("DECODE: in=%s, outlen=%lu, out=[%.*s]", buf1, outLen, outLen, buf2) );
        REQUIRE( outLen == strlen( GOLDEN3_IN ) );
        REQUIRE( strncmp( buf2, GOLDEN3_IN, outLen ) == 0 );


        uint8_t bin[1233]={ 0, };
        for ( size_t i=0; i < sizeof( bin ); i++ )
        {
            bin[i] = 0xFF & i;
        }
        r = base64Encode( bin, sizeof(bin), buf1, sizeof( buf1 ), outLen );
        REQUIRE( r );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ENCODE: outlen=%lu, out=[%.*s]", outLen, outLen, buf1) );
        REQUIRE( strlen( buf1 ) == strlen( GOLDEN4_OUT ) );
        REQUIRE( strcmp( buf1, GOLDEN4_OUT ) == 0 );

        r = base64Decode( buf1, outLen, buf2, sizeof( buf2 ), outLen );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("DECODE: outlen=%lu", outLen) );
        REQUIRE( outLen == sizeof(bin) );
        REQUIRE( memcmp( buf2, bin, sizeof(bin)) == 0 );

        r = base64Encode( GOLDEN5_IN, strlen( GOLDEN5_IN ), buf1, sizeof( buf1 ), outLen );
        REQUIRE( r );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ENCODE: in=%s, outlen=%lu, out=[%.*s]", GOLDEN5_IN, outLen, outLen, buf1) );
        REQUIRE( strlen( buf1 ) == strlen( GOLDEN5_OUT ) );
        REQUIRE( strcmp( buf1, GOLDEN5_OUT ) == 0 );

        r = base64Decode( buf1, outLen, buf2, sizeof( buf2 ), outLen );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("DECODE: in=%s, outlen=%lu, out=[%.*s]", buf1, outLen, outLen, buf2) );
        REQUIRE( outLen == strlen( GOLDEN5_IN ) );
        REQUIRE( strncmp( buf2, GOLDEN5_IN, outLen ) == 0 );

        r = base64Encode( GOLDEN5_IN, strlen( GOLDEN5_IN ), buf1, sizeof( buf1 ), outLen, true );
        REQUIRE( r );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ENCODE: in=%s, outlen=%lu, out=[%.*s]", GOLDEN5_IN, outLen, outLen, buf1) );
        REQUIRE( strlen( buf1 ) == strlen( GOLDEN6_OUT ) );
        REQUIRE( strcmp( buf1, GOLDEN6_OUT ) == 0 );

        r = base64Decode( buf1, outLen, buf2, sizeof( buf2 ), outLen );
        CPL_SYSTEM_TRACE_MSG( SECT_, ("DECODE: in=%s, outlen=%lu, out=[%.*s]", buf1, outLen, outLen, buf2) );
        REQUIRE( outLen == strlen( GOLDEN5_IN ) );
        REQUIRE( strncmp( buf2, GOLDEN5_IN, outLen ) == 0 );
    }

    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0u );
}
