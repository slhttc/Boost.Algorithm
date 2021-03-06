# This is the proposed Boost.Algorithm library
## Update: Jan 2012 The library has been approved, and the contents are being migrated to the Boost subversion server at svn.boost.org

It will be reviewed by the [Boost](<http://www.boost.org>) community from September 22nd through October 1st, 2011. It is licensed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt)

The Boost.Algorithms library is a work in progress; it is not meant to be a complete set of general purpose algorithms for C++ programming; but rather a small collection of useful algorithms, and a structure for adding more over time.

The algorithms here fall into three basic categories:

1. Searching
	* Boyer-Moore Search
	* Boyer-Moore-Horspool Search
	* Knuth-Morris-Pratt search
1. Sequence properties
	* Tests to see if a sequence is ordered
	* Tests for the elements of a sequence
1. Miscellaneous
	* Constrain a value between two 'boundaries' (minmax)
	
TONGARI has been nice enough to put a copy of the [generated documentation online](http://jamboree.zzl.org/boost_algorithm/)

### Update (12-05)
I have added a lot of the C++11 algorithms to the library. Check out:

* copy\_if (and copy\_while, copy\_until)
* copy\_n
* find\_if\_not
* iota
* is\_partitioned
* is\_permutation
* minmax
* minmax\_element (satisfying the C++11 performance criteria)
* move (using ether std::move or boost::move, depending)
* partition\_copy
* partition\_point

The docs are still lagging, but coming along.
