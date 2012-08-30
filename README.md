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

What this tool does **not**:

* call a tierce tool like unrar or unzip to check the password
* compute permutations (will change) or create wordlists

What this tool does:

* understand the data format (see src/*)
* check the password using the way official tool does
* eventually use optimized routines
* read passwords from STDIN:
	- john, crunch or whatever you want for bruteforce attacks
	- cat (or whatever you want) for wordlist attack


Man
---

Usage: **module**cracker-ng FILE

Examples with ZIP module:

* wordlist attack:  
	cat wordlist.lst | zipcracker-ng FILE  
* bruteforce attack:  
	john --incremental --stdout | zipcracker-ng FILE  	
	crunch 1 8 -f charset.lst lalpha | zipcracker-ng FILE  

Why
---

I wrote this tool when I saw poor resources we have on *unix for ZIP and  
RAR files password cracking. Hope it will be helpful.
