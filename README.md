
Cracker-ng
==========

Copyright (C) 2011-2012 by Mickaël 'Tiger-222' Schoentgen.
 
Cracker-ng comes with ABSOLUTELY NO WARRANTY.

This is free software, and you are welcome to redistribute it under 

certain conditions. See the GNU General Public Licence for details.


What
----

Cracker-ng, a multiple file cracker.
For now, there are these modules:

* zip cracker (zip, apm, ipa, ... ),
* ccrypt cracker (cpt)

What this tool does _not_:

1. call a tierce tool like unrar or unzip to check the password
2. compute permutations (will change) or create wordlists

What this tool does:

1. understand the data format (see src/*)
2. check the password using the way official tool does
3. eventually use optimized routines
4. read passwords from STDIN:
	- john, crunch or whatever you want for bruteforce attacks
	- cat (or whatever you want) for wordlist attack


Man
---

Usage: <module>cracker-ng <file>

Examples with ZIP module:

- wordlist attack:

	cat wordlist.lst | zipcracker-ng file[.zip]

- bruteforce attack:

	john --incremental --stdout | zipcracker-ng file[.zip]
	
	crunch 1 8 -f charset.lst lalpha | zipcracker-ng file[.zip]


Why
---

I wrote this tool when I saw poor resources we have on *unix for ZIP and
RAR files password cracking. Hope it will be helpful.
