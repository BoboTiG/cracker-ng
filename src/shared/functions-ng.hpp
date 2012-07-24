
/*!
 * \file functions-ng.hpp
 * \brief Cracker-ng optimized functions headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 */


// Optimized strncmp()/memcmp() ----------------------------------------
template<class T>
inline int memcmp_ng(const T *lhs, const T *rhs, size_t n);
