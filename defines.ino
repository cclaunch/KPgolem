// 
// assignments of pins by logical name as well as creation of important constants by name
//
#define VERSION "2.12beta"
#define FULLVERBS 1    // 0 to use only one letter verbs, 1 to require full verb name

#define PUN12 25
#define PUN11 24
#define PUN0  23
#define PUN1  22
#define PUN2  52
#define PUN3  53
#define PUN4  50
#define PUN5  51
#define PUN6  48
#define PUN7  49
#define PUN8  46
#define PUN9  47
#define SPC   26
#define REL   27
#define COL1  29
#define MPC   28
#define REG   44
#define PCHT1 42
#define PCHT2 43

#define RDR12  2
#define RDR11  3
#define RDR0   4
#define RDR1   5
#define RDR2   6
#define RDR3   7
#define RDR4   8
#define RDR5   9
#define RDR6  10
#define RDR7  11
#define RDR8  12
#define RDR9  13
#define RDRT1 36
#define RDRT2 39

#define TX    16
#define RX    17
#define CTS   32
#define RTS   35

// durations of various events in the punch
#define PUNCHTIME   20  // 20 ms to trigger interposers
#define CYCLETIME   80  // additional time for punch cycle to complete
#define READTIME    40  // additional time from dropping punch until sensing holes at read station
#define RELEASETIME 15  // 15 ms to trigger the release button (plus 500ms plus time to move to column 80 of current card
#define FIRSTCOL    20  // 20 ms to trigger dummy punch cycle
#define SENSECOL    20  // 20 ms wait after dummy punch cycle started before sensing pins 

// definition for picking and dropping relays
// substituted due to possible confusion from inverted logic
#define PICK LOW
#define DROP HIGH

// flow control definitions
#define XON 17
#define XOFF 19

// command delimiter character
#define DELIM '_'
