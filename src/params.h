// these are the setting for looking for the keccak state flip-flops. the intervals are inclusive.
// FIF
#define FANIN_FLOOR 33
// FIC
#define FANIN_CEILING 99999
// FOF
#define FANOUT_FLOOR 34
// FOC
#define FANOUT_CEILING 35

// define this one to print a lot of debug messages on the terminal
#undef PRINT_DETAILS 
//#define PRINT_DETAILS 1

// pick one out these two strategies. G = group strat. I = individual strat
#define STRAT_G
//#define STRAT_I

// these are experimental settings, not used in results
#define THRESHOLD 64

