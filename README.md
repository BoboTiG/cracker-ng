Cracker-ng
==========

Cracker-ng, a multiple file password finder.
For now, there are these modules:

* **zip cracker** (zip, winzip, apm, ipa, and all zip based files),
* **ccrypt cracker** (cpt, antigift) [from the 2nd version (1.1) to the latest (1.10), 1.0 is buggy]

What this tool does **not**:

* call a tierce tool like unrar or unzip to check the password (and will never)
* compute permutations (will change) or create wordlists

What this tool does:

* understand the data format (see src/*)
* check the password using the way official tool does
* use optimized routines
* read passwords from a wordlist or STDIN:
	- cat, john, crunch or whatever you want for wordlist attack


Installation
---

```shell
$ git clone https://github.com/BoboTiG/cracker-ng.git
$ cd cracker-ng
```

For testers and contributors, always work with on the devel branch:
```shell
$ git checkout devel
```

To show available commands:
```shell
$ make
```

Example to build the ZIP module:
```shell
$ make zip
```

Build the debug version (for testers and contributors):
```shell
$ DEBUG=1 make zip
```

The final executable file will be stored into `bin` folder.

Man
---

Usage: `<MODULE>cracker-ng -f FILE`

Examples with ZIP module:

```shell
# Using STDIN to use power of other tools
$ cat wordlist.lst | zipcracker-ng -f FILE  -
$ john --incremental --stdout | zipcracker-ng -f FILE -
$ crunch 1 8 -f charset.lst lalpha | zipcracker-ng -f FILE -

# Or using a wordlist
$ zipcracker-ng -f FILE -w wordlist.lst
```

Why
---

I wrote this tool when I saw poor resources we have on Unix for ZIP and
RAR files password cracking. Hope it will be helpful.

License
-------

```
Copyright (C) 2011-2022 by Mickaël 'Tiger-222' Schoentgen.

Cracker-ng comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under
certain conditions. See the GNU General Public Licence for details.
```
