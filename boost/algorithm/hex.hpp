/* 
   Copyright (c) Marshall Clow 2011.

   Distributed under the Boost Software License, Version 1.0. (See accompanying
   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
   
   Thanks to Nevin for his comments/help.
*/

/*
    General problem - turn a sequence of integral types into a sequence of hexadecimal characters.
    - and back.

TO DO:
    1. these should really only work on integral types. (see the >> and << operations)
        -- this is done, I think.
    2. The 'value_type_or_char' struct is really a hack.
        -- but it's a better hack now that it works with back_insert_iterators
*/

/// \file  hex.hpp
/// \brief Convert sequence of integral types into a sequence of hexadecimal 
///     characters and back. Based on the MySQL functions HEX and UNHEX
/// \author Marshall Clow

#ifndef BOOST_ALGORITHM_HEXHPP
#define BOOST_ALGORITHM_HEXHPP

#include <iterator>     // for std::iterator_traits
#include <stdexcept>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/exception/all.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>


namespace boost { namespace algorithm {

//  exceptions for these routines to throw
struct hex_decode_error: virtual boost::exception, virtual std::exception {};
struct non_hex_input : public hex_decode_error {};
struct not_enough_input : public hex_decode_error {};

namespace detail {

    template <typename T, typename OutputIterator>
    OutputIterator encode_one ( T val, OutputIterator out ) {
        char res [ 2 * sizeof ( T ) ];
        char  *p = res + sizeof ( res );
        for ( std::size_t i = 0; i < sizeof ( res ); ++i, val >>= 4 )
            *--p = "0123456789ABCDEF" [ val & 0x0F ];
        return std::copy ( res, res + sizeof ( res ), out );
        }

    unsigned hex_char_to_int ( char c ) {
        if ( c >= '0' && c <= '9' ) return c - '0';
        if ( c >= 'A' && c <= 'F' ) return c - 'A' + 10;
        if ( c >= 'a' && c <= 'f' ) return c - 'a' + 10;
        BOOST_THROW_EXCEPTION (non_hex_input ());
        return 0;   // keep dumb compilers happy
        }
    

//  My own iterator_traits class.
//  It is here so that I can "reach inside" some kinds of output iterators
//      and get the type to write.
    template <typename Iterator>
    struct hex_iterator_traits {
        typedef typename std::iterator_traits<Iterator>::value_type value_type;
    };

    template<typename Container>
    struct hex_iterator_traits< std::back_insert_iterator<Container> > {
        typedef typename Container::value_type value_type;
    };

    template<typename Container>
    struct hex_iterator_traits< std::front_insert_iterator<Container> > {
        typedef typename Container::value_type value_type;
    };

    template<typename Container>
    struct hex_iterator_traits< std::insert_iterator<Container> > {
        typedef typename Container::value_type value_type;
    };

//	ostream_iterators have three template parameters.
//	The first one is the output type, the second one is the character type of
//	the underlying stream, the third is the character traits.
//		We only care about the first one.
  template<typename T, typename charType, typename traits>
  struct hex_iterator_traits< std::ostream_iterator<T, charType, traits> > {
      typedef T value_type;
  };

//  Output Iterators have a value type of 'void'. Kinda sucks. 
//	We special case some output iterators, but we can't enumerate them all.
//  If we can't figure it out, we assume that you want to output chars.
//  If you don't, pass in an iterator with a real value_type.
    template <typename T> struct value_type_or_char       { typedef T value_type; };
    template <>           struct value_type_or_char<void> { typedef char value_type; };
    
//  All in one step
    template <typename Iterator> 
    struct iterator_value_type {
        typedef typename value_type_or_char<typename hex_iterator_traits<Iterator>::value_type>::value_type value_type;
        };
        
//  What can we assume here about the inputs?
//      is std::iterator_traits<InputIterator>::value_type always 'char' ?
//  Could it be wchar_t, say? Does it matter?
//      We are assuming ASCII for the values - but what about the storage?
    template <typename InputIterator, typename OutputIterator>
    typename boost::enable_if<boost::is_integral<typename iterator_value_type<OutputIterator>::value_type>, OutputIterator>::type
    decode_one ( InputIterator &first, InputIterator last, OutputIterator out ) {
        typedef typename iterator_value_type<OutputIterator>::value_type T;
        T res = 0;

    //  Need to make sure that we get can read that many chars here.
        for ( std::size_t i = 0; i < 2 * sizeof ( T ); ++i, ++first ) {
            if ( first == last ) 
                BOOST_THROW_EXCEPTION (not_enough_input ());
            res <<= 4;
            res += hex_char_to_int ( *first );
            }
        
        *out++ = res;
        return out;
        }

    }


template <typename InputIterator, typename OutputIterator>
typename boost::enable_if<boost::is_integral<typename detail::hex_iterator_traits<InputIterator>::value_type>, OutputIterator>::type
hex ( InputIterator first, InputIterator last, OutputIterator out ) {
    while ( first != last )
        out = detail::encode_one ( *first++, out );
    return out;
    }
    
template <typename T, typename OutputIterator>
typename boost::enable_if<boost::is_integral<T>, OutputIterator>::type
hex ( const T *ptr, OutputIterator out ) {
    while ( *ptr )
        out = detail::encode_one ( *ptr++, out );
    return out;
    }

template <typename Range, typename OutputIterator>
typename boost::enable_if<boost::is_integral<typename detail::hex_iterator_traits<typename Range::iterator>::value_type>, OutputIterator>::type
hex ( const Range &r, OutputIterator out ) {
    return hex (boost::begin(r), boost::end(r), out);
}


template <typename InputIterator, typename OutputIterator>
OutputIterator unhex ( InputIterator first, InputIterator last, OutputIterator out ) {
    while ( first != last )
        out = detail::decode_one ( first, last, out );
    return out;
    }

template <typename T, typename OutputIterator>
OutputIterator unhex ( const T *ptr, OutputIterator out ) {
    typedef typename detail::iterator_value_type<OutputIterator>::value_type OutputType;
//  If we run into the terminator while decoding, we will throw a
//      malformed input exception. It would be nicer to throw a 'Not enough input'
//      exception - but how much extra work would that require?
//  I just make up an "end iterator" which we will never get to - 
//      two Ts per byte of the output type.
    while ( *ptr )
        out = detail::decode_one ( ptr, ptr + 2 * sizeof(OutputType), out );
    return out;
    }


template <typename Range, typename OutputIterator>
OutputIterator unhex ( const Range &r, OutputIterator out ) {
    return unhex (boost::begin(r), boost::end(r), out);
    }


//	Simple wrappers
template<typename String>
String hex ( const String &input ) {
	String output;
	output.reserve (input.size () * (2 * sizeof (typename String::value_type)));
	hex (input, std::back_inserter (output));
	return output;
	}

template<typename String>
String unhex ( const String &input ) {
	String output;
	output.reserve (input.size () / (2 * sizeof (typename String::value_type)));
	unhex (input, std::back_inserter (output));
	return output;
	}

}}

#endif // BOOST_ALGORITHM_HEXHPP
