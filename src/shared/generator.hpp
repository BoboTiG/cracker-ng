
/*!
 * \file generator.hpp
 * \brief Cracker-ng password generator headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2011.12.28
 */


// Charsets ------------------------------------------------------------
static string charset_a = "abcdefghijklmnopqrstuvwxyz"; // 0
static string charset_A = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 1
static string charset_0 = "0123456789";                 // 2
static string charset__ = "-_!@#$%*[];?()";             // 3
static string charset_h = "0123456789abcdef";           // 4
static string charset_H = "0123456789ABCDEF";           // 5


// Prototypes ---------------------------------------------------------- 
/*!
 * \fn 
 * \param argv Array containing arguments.
 */
/*inline void recurse(
	const string       & charset, 
	const unsigned int & charset_len, 
	unsigned int       & max_len, 
	unsigned int         position,
	const string       & word),
*/
