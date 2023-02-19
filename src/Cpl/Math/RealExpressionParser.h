#ifndef Cpl_Math_RealExpressionParser_h_
#define Cpl_Math_RealExpressionParser_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file

    This file contains a collection of methods that parses a null terminated
    string that contains an arithmetic expression, and evaluates the expression.


    This module is adapted from Kim Walisch's code at: https://github.com/kimwalisch/calculator
    ===========================================================================
    BSD 2-Clause License

    Copyright (c) 2013 - 2018, Kim Walisch.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    ===========================================================================
 */

#include "colony_config.h"
#include "Cpl/Container/Stack.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Math/real.h"
#include <stdlib.h>
#include <math.h>

 /** Maximum depth of the operator stack used when evaluating an expression
  */
#ifndef OPTION_CPL_MATH_REALEXPR_MAX_STACK_SIZE
#define OPTION_CPL_MATH_REALEXPR_MAX_STACK_SIZE     20
#endif

  ///
namespace Cpl {
///
namespace Math {


/** This template class evaluates an null terminate string that represents an
    real-number arithmetic expression.

    Template Arg(s):
        T   Double/float type to use for the numeric expression/calculations
 */
template <typename T>
class RealExpressionParser
{
public:
    /// Constructor
    RealExpressionParser()
        : m_stack( OPTION_CPL_MATH_REALEXPR_MAX_STACK_SIZE, m_stackMemory )
    {
    }

public:
    /** Evaluates an integer arithmetic expression and returns true if the
        expression was successfully evaluated.  The evaluated value is
        returned via, the 'result' argument.
     */
    bool eval( const char* expressionAsText, T& result )
    {
        result    = 0;
        m_index   = 0;
        m_expr    = expressionAsText;
        m_exprLen = strlen( expressionAsText );
        m_stack.clearTheStack();

        if ( !parseExpr( result ) )
        {
            return false;
        }
        return true;
    }

private:
    /// List of operations    
    enum
    {
        OPERATOR_NULL,
        OPERATOR_ADDITION,       //!< +
        OPERATOR_SUBTRACTION,    //!< -
        OPERATOR_MULTIPLICATION, //!< *
        OPERATOR_DIVISION,       //!< /
        OPERATOR_MODULO,         //!< %
        OPERATOR_POWER           //!< **
    };

    /// Operator struct
    struct Operator
    {
        /// Operator, one of the OPERATOR_* enum definitions
        int op;

        /// Precedence
        int precedence;

        /// 'L' = left or 'R' = right
        int associativity;

        /// Constructor
        Operator( int opr, int prec, int assoc )
            : op( opr )
            , precedence( prec )
            , associativity( assoc )
        { }

        /// Default constructor (used when allocating the stack)
        Operator()
            : op( OPERATOR_NULL )
            , precedence( 0 )
            , associativity( 'L' )
        {
        }
    };

    /// Operator value struct
    struct OperatorValue
    {
        /// Operator
        Operator op;

        /// My value
        T value;

        /// Constructor
        OperatorValue( const Operator& opr, T val )
            : op( opr )
            , value( val )
        { }

        /// Default constructor (used when allocating the stack)
        OperatorValue()
            : op()
            , value( 0 )
        {
        }

        /// Data accessor
        int getPrecedence() const
        {
            return op.precedence;
        }

        /// Check for null operation
        bool isNull() const
        {
            return op.op == OPERATOR_NULL;
        }
    };


    bool calculate( T& result, T v1, T v2, const Operator& op ) const
    {
        bool success = true;
        switch ( op.op )
        {
        case OPERATOR_ADDITION:
            result =  v1 + v2;
            break;

        case OPERATOR_SUBTRACTION:
            result =  v1 - v2;
            break;

        case OPERATOR_MULTIPLICATION:
            result =  v1 * v2;
            break;

        case OPERATOR_POWER:
            result =  (T) pow( (double)v1, (double) v2 );
            break;

        case OPERATOR_DIVISION:
            if ( !Cpl::Math::almostEquals<T>(v2,0,sizeof(T) == sizeof(float)? CPL_MATH_REAL_FLOAT_EPSILON: CPL_MATH_REAL_DOUBLE_EPSILON ))
            {
                result = v1 / v2;
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( "Cpl::Math", ("Divide by zero at index %d", m_index) );
                success = false;
            }
            break;

        case OPERATOR_MODULO:
            if ( !Cpl::Math::almostEquals<T>( v2, 0, sizeof( T ) == sizeof(float)? CPL_MATH_REAL_FLOAT_EPSILON : CPL_MATH_REAL_DOUBLE_EPSILON ) )
            {
                result = (T) fmod( (double)v1, (double)v2 );
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( "Cpl::Math", ("Modulo by zero at index %d", m_index) );
                success = false;
            }
            break;

        default:
            CPL_SYSTEM_TRACE_MSG( "Cpl::Math", ("Unsupported Operator (%d)", op.op) );
            success = false;
            break;
        }

        return success;
    }

    /// Check if the end of the expression has been reached
    bool isEnd() const
    {
        return m_index >= m_exprLen;
    }

    /** Returns the character at the current expression index or
        0 if the end of the expression is reached.
     */
    char getCharacter() const
    {
        if ( !isEnd() )
        {
            return m_expr[m_index];
        }

        return 0;
    }

    /// 'Record' that an error occurred
    void unexpected() const
    {
        CPL_SYSTEM_TRACE_MSG( "Cpl::Math", ("Syntax error: unexpected token at index %d [%s]",
                                             m_index,
                                             m_expr) );
    }

    /** Eat all white space characters at the
        current expression index.
     */
    void eatSpaces()
    {
        while ( isspace( getCharacter() ) != 0 )
        {
            m_index++;
        }
    }

    /** Parse a binary operator at the current expression index.
        @return Operator with precedence and associativity.
     */
    bool parseOp( Operator& result )
    {
        bool success = true;

        eatSpaces();
        switch ( getCharacter() )
        {
        case '+':
            m_index++;
            result = Operator( OPERATOR_ADDITION, 10, 'L' );
            break;

        case '-':
            m_index++;
            result = Operator( OPERATOR_SUBTRACTION, 10, 'L' );
            break;

        case '/':
            m_index++;
            result = Operator( OPERATOR_DIVISION, 20, 'L' );
            break;

        case '%':
            m_index++;
            result = Operator( OPERATOR_MODULO, 20, 'L' );
            break;

        case '*':
            m_index++;
            if ( getCharacter() != '*' )
            {
                result = Operator( OPERATOR_MULTIPLICATION, 20, 'L' );
            }
            else
            {
                m_index++;
                result = Operator( OPERATOR_POWER, 30, 'R' );
            }
            break;

        default:
            result  = Operator( OPERATOR_NULL, 0, 'L' );
            break;
        }

        return success;
    }

    T parseReal()
    {
        char* endPtr = 0;
        T     value  = (T) strtod( m_expr + m_index, &endPtr );
        m_index      = endPtr - m_expr;
        return value;
    }

    /** Parse an integer value at the current expression index.
        The unary `+', `-' and `~' operators and opening
        parentheses `(' cause recursion.
     */
    bool parseValue( T& valFinal )
    {
        T val = 0;
        eatSpaces();
        switch ( getCharacter() )
        {
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            val = parseReal();
            break;

        case '(':
            m_index++;
            if ( !parseExpr( val ) )
            {
                return false;
            }
            eatSpaces();
            if ( getCharacter() != ')' )
            {
                CPL_SYSTEM_TRACE_MSG( "Cpl::Math", ("Syntax error: `)' expected at end of expression (index=%d",
                                                     m_index) );
                return false;
            }
            m_index++;
            break;

        case '+':
            m_index++;
            if ( !parseValue( val ) )
            {
                return false;
            }
            break;

        case '-':
            m_index++;
            if ( !parseValue( val ) )
            {
                return false;
            }
            val =  val * static_cast<T>(-1);
            break;

        default:
            if ( !isEnd() )
            {
                unexpected();
                return false;
            }
        }

        valFinal = val;
        return true;
    }

    /** Parse all operations of the current parenthesis
        level and the levels above, when done
        return the result (value).
     */
    bool parseExpr( T& finalValue )
    {
        m_stack.push( OperatorValue( Operator( OPERATOR_NULL, 0, 'L' ), 0 ) );

        // first parse value on the left
        T value;
        if ( !parseValue( value ) )
        {
            return false;
        }

        while ( !m_stack.isEmpty() )
        {
            // parse an operator (+, -, *, ...)
            Operator op;
            if ( !parseOp( op ) )
            {
                return false;
            }

            // Peek at the next value on the stack (which I know is NOT empty)
            OperatorValue topVal;
            m_stack.peekTop( topVal );

            while ( op.precedence < topVal.getPrecedence() ||
                    (op.precedence == topVal.getPrecedence() &&
                      op.associativity == 'L')
                    )
            {
                // end reached
                if ( topVal.isNull() )
                {
                    m_stack.pop();
                    finalValue = value;
                    return true;
                }

                // do the calculation ("reduce"), producing a new value
                if ( !calculate( value, topVal.value, value, topVal.op ) )
                {
                    return false;
                }
                m_stack.pop();

                // Peek at new top value
                if ( !m_stack.peekTop( topVal ) )
                {
                    unexpected();
                    return false;
                }
            }

            // store on m_stack and continue parsing ("shift")
            if ( !m_stack.push( OperatorValue( op, value ) ) )
            {
                CPL_SYSTEM_TRACE_MSG( "Cpl::Math", ("Operator Stack FULL at index=%d", m_index) );
                return false;
            }

            // parse value on the right
            if ( !parseValue( value ) )
            {
                return false;
            }
        }

        return false;
    }

private:
    /** The current operator and its left value
        are pushed onto the stack if the operator on
        top of the stack has lower precedence.
     */
    Cpl::Container::Stack<OperatorValue> m_stack;

    /// Expression string
    const char* m_expr;

    /// Current expression index, incremented whilst parsing
    size_t      m_index;

    /// Length of the expression
    size_t      m_exprLen;

    /// Memory for the stack
    OperatorValue m_stackMemory[OPTION_CPL_MATH_REALEXPR_MAX_STACK_SIZE];
};


};      // end namespaces
};
#endif  // end header latch
