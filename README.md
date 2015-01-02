KPgolem
=======

Interface to IBM 029 and 026 keypunches to allow remote punching and reading of cards

Consists of cabling added to the keypunch plus an Arduino Mega based interface box inside the machine. Access is via 9600 baud 8-n-1 serial port DB9 connector on the interface box. Simple ascii based protocol to control the interface, supporting ASCII-ish EBCDIC or BCD encoding, plus a binary mode directly specifying each punch to fire. A user table can be created in place of the two ASCII encodings. 

Commands can use single character short verbs or full word verbs as a configuration choice, e.g. MODE ASCII or M A to set the encoding mode to ASCII. All commands begin with a delimiter, set by default to underscore (_) with the ability to escape using two sequential delimiters if the delimiter character itself is needed within the command line. 

Primary usage is punching cards, accomplished by sending _P plus up to 80 characters. Short lines will cause a release of the card and autofeed of the next blank card for any following punch commands. Due to the high volume of this verb compared to most others, its short and verbose form is the same, just the letter P. 

If the interface is set to verify mode, then each card will be verified during the punching of the following card. This slows the punching rate of the machine and requires one blank card be at the end of the deck following the last card punched with data. This is accomplished using the command _VERIFY ON after which the interface will emit messages either confirming a match between the physical card and what was expected to have been punched in it, or highlighting the column(s) where the holes don't match. 

Reading a deck is done by issuing the command _R and capturing the message from the interface with the data captured from the card. A dummy punch operation should be made to the first card (e.g. by a _P command with no data or just spaces), to move the first card into the read station which is sequentially after the punch station of the keypunch. Cards are returned in the mode and encoding set by the user - ASCII encoding of EBCDIC, BCD or user table, as well as four hex digits per column for binary mode representing the 12 rows of the card as bits, right justified, 12 down to 9 from left to right.

Diagnostic functions are included, accessible by the _DIAG command, in addition to the commands _P, _R, _MODE, _VERIFY, _CODE (to set BCD or EBCDIC encoding into ASCII) and _LOAD to update the ASCII or user tables for a specified ASCII character typed over the channel to the interface. 

The project will eventually include full details of the Arduino code, modifications to the keypunches, parts required for the interface box and a user guide. 
