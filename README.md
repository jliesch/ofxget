# ofxget
C++ tool for downloading OFX files and interacting with ofxhome.

This is meant to be the simplest and most self contained OFX client possible. It is intended to be embedded in other programs. As such the command line options are fairly limited. There are better alternatives out there but hopefully this is the most pluggable.

Example for downloading Vanguard investments from the command line.
1. Download the repo
1. make
1. Get accounts: ./ofxget -institution 479 -request accounts.txt
   1. Enter missing USERID and USERPASS.
1. Download investments: ./ofxget -institution 479 -request investment.txt
   1. Enter missing USERID, USERPASS, and ACCTID.
1. Optionally, enter account info in passwords.txt file.
1. Optionally, refresh institutions.txt: ./ofxhome > institutions.txt

The ofxget tool makes no effort to hide or secure your password and account information. It is meant to be used embedded another program that provides thoes protections.
