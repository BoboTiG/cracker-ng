
/*!
 * \file gui.cc
 * \brief GUI management.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.02.08
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./gui.h"


GUI::GUI(
	const std::string& title, const std::string& file, const std::string& chosen_one,
	const std::string& encryption, const std::string& method, const std::string& generator
):
	row(0),
	col(0),
	title(title),
	file(file),
	chosen_one(chosen_one),
	encryption(encryption),
	method(method),
	generator(generator)
{
	this->init();
}

GUI::~GUI() {
	/*move(HEIGHT, 0);
	refresh();
	printf("\r\n");*/
	printf(" ^ Ex(c)iting.\n\n");
}

void GUI::init() {
	// curses initializations
	//initscr();                   // Start the curses mode
	//raw();				         // Line buffering disabled - intercept CTRL+C, ...
	//getmaxyx(stdscr, row, col);  // Get the number of rows and columns

	// Sanitize strings to correctly feet into GUI
	char* _file = new char[WIDTH];
    sprintf(_file, "File......: %s", functions_ng::substr(this->file, MAXLEN).c_str());
    this->file = _file;
	delete[] _file; _file = 0;
    
	char* _chosen_one = new char[WIDTH];
 	sprintf(_chosen_one, "Chosen one: %s", functions_ng::substr(this->chosen_one, MAXLEN).c_str());
    this->chosen_one = _chosen_one;
	delete[] _chosen_one; _chosen_one = 0;
 	
	char* _generator = new char[WIDTH];
	sprintf(_generator, "Generator.: %s", functions_ng::substr(this->generator, MAXLEN).c_str());
    this->generator = _generator;
	delete[] _generator; _generator = 0;
}

/*bool GUI::is_ok() const {
	if ( this->col < WIDTH || this->row < HEIGHT + 2 ) {
		this->stop();
		printf(" ! I need a terminal with a minimum size of %dx%d.\n", WIDTH, HEIGHT);
		return false;
	}
	return true;
}*/

void GUI::run() const {
	//unsigned int y = 0;    // Y axis indice
	//unsigned int i         = 0;    // Indice for "Try a word" input string entered
	//unsigned int title_len = (WIDTH - this->title.length()) / 2;
	//int c;
	//char* str = new char[MAXLEN];  // String from "Try a word" input

	/*// Borders
	// top left - top - top right
	mvaddch(y, 0, ACS_ULCORNER);
	mvhline(y, 1, ACS_HLINE, WIDTH - 1);
	mvaddch(y, WIDTH, ACS_URCORNER);
	// left - bottom left
	mvvline(++y, 0, ACS_VLINE, HEIGHT - 1);
	mvaddch(HEIGHT, 0, ACS_LLCORNER);
	mvaddch(HEIGHT, WIDTH - 1, ACS_LRCORNER);
	// right - bottom right
	mvvline(y, WIDTH, ACS_VLINE, HEIGHT - 1);
	mvaddch(HEIGHT, WIDTH, ACS_LRCORNER);
	// bottom
	mvhline(HEIGHT, 1, ACS_HLINE, WIDTH - 1);

	// Title + informations
	mvprintw(y, title_len, "%s",  this->title.c_str());
	mvchgat(y, 1, WIDTH - 1, A_REVERSE, 1, NULL);  // Reverse colors
	++y;
	mvprintw(++y, 2, this->file.c_str());
	mvchgat(y, SHIFT, WIDTH - SHIFT, A_BOLD, 0, NULL);  // Bold the filename
	mvprintw(++y, 2, this->chosen_one.c_str());
	mvprintw(++y, 2, this->encryption.c_str());
	mvprintw(++y, 2, this->method.c_str());
	++y;

	// Separator
	mvaddch(++y, 0, ACS_LTEE);
	mvhline(y, 1, ACS_HLINE, WIDTH - 1);
	mvaddch(y, WIDTH, ACS_RTEE);

	// Manual entry
	mvprintw(++y, 2, "Try a word: ");

	// Separator
	mvaddch(++y, 0, ACS_LTEE);
	mvhline(y, 1, ACS_HLINE, WIDTH - 1);
	mvaddch(y, WIDTH, ACS_RTEE);

	// Generator and stats
	mvprintw(++y, 2, this->generator.c_str());
	mvprintw(++y, 2, "Stats.....: ");

	// Help
	++y;
	mvprintw(++y, WIDTH - 21, "Press CTRL+C to abort");
	refresh();*/
	
	printf("\n %s\n", this->title.c_str());
	printf(" - %s\n", this->file.c_str());
#ifdef ZIP
	printf(" * %s\n", this->chosen_one.c_str());
	printf(" - %s\n", this->encryption.c_str());
	printf(" - %s\n", this->method.c_str());
#endif
	printf(" - %s\n", this->generator.c_str());
	printf(" . Working ...\n");


	//
	// The loop
	//
	/*do {
		// Go to the manual entry area
		move(input, SHIFT);
		for ( i = 0; i < MAXLEN; ++i ) {
			printw(" ");
		}
		i = 0;
		do {
			move(input, SHIFT + i);
			c = wgetch(stdscr);
			str[i] = c;
			++i;
		} while ( c != KEY_ESC && c != CTRL_C && c != '\n' && i < MAXLEN );

		move(HEIGHT, 0);
		refresh();
	} while ( c != KEY_ESC && c != CTRL_C );*/
}
