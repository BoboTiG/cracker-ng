
/*!
 * \file gui.h
 * \brief GUI management.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.02.08
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_SHARED_GUI_H_
#define SRC_SHARED_GUI_H_

#include <string>
//#include <curses.h>
#include "./functions.h"


static const unsigned int WIDTH  = 80;             //!< GUI width min
static const unsigned int HEIGHT = 13;             //!< GUI height min
static const unsigned int SHIFT  = 14;             //!< Shift of the area where to write ("File........: "HERE)
static const unsigned int MAXLEN = WIDTH - SHIFT;  //!< Maximum length for strings to no bypass borders

// Few interesting keys
//static const int KEY_ESC = 27;
//static const int CTRL_C  =  3;


class GUI {
public:
	GUI(
		const std::string&, const std::string&, const std::string&,
		const std::string&, const std::string&, const std::string&
	);
	~GUI();
	
	//bool is_ok() const;
	void run() const;
	void stop() const { /*endwin(); printf("\r\n");*/ };
	
protected:
	size_t row, col;
	std::string title, file, chosen_one, encryption, method, generator;
	
	void init();
};

#endif  // SRC_SHARED_GUI_H_
