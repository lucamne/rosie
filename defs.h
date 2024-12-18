#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

// uncomment this line to turn off assertion checks
//#define NDEBUG
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define NAME "Rosie 1.0"

#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 0 2"
#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

/* ==========================================================================
 * TYPES
 * ========================================================================== */

/***Typedefs***/

// number of squares in our 10 x 12
#define BRD_SQ_NUM 120
#define PIECE_NUM 13
#define COLOR_NUM 3
#define MAX_GAME_MOVES 2048
#define MAX_POSITION_MOVES 256

typedef  uint64_t U64;
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, OFFBOARD };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum { WHITE, BLACK, BOTH };
// Castling permissions will be stored in a 4 bit int with each bit representing a castling permission
enum { WKCA = 0b0001, WQCA = 0b0010, BKCA = 0b0100, BQCA = 0b1000 };

enum {
	NO_SQ = -1,
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8 
};

/***GAME_MOVE***/
/* 7 bits are needed to represent square 64
 * 12 pieces (Not including EMPTY or OFFBOARD) will fit into 4 bits
 *
 * 0000 0000 0000 0000 0000 0111 1111 -> From square >> 0, 0x7F
 * 0000 0000 0000 0011 1111 1000 0000 -> To square >> 7, 0x7f
 * 0000 0000 0011 1100 0000 0000 0000 -> Captured piece >> 14, 0xF
 * 0000 0011 1100 0000 0000 0000 0000 -> Pawn Promotion >> 18, 0xF
 * 0000 0100 0000 0000 0000 0000 0000 -> is En Passant capture? 0x400000
 * 0000 1000 0000 0000 0000 0000 0000 -> Pawn start 0x800000
 * 0001 0000 0000 0000 0000 0000 0000 -> Was Castling Move? 0x1000000
 * */

// move accessors
static inline int get_from_sq(unsigned int move) { return move & 0x7F; }
static inline int get_to_sq(unsigned int move) { return (move >> 7) & 0x7F; }
static inline int get_captured_piece(unsigned int move) { return (move >> 14) & 0xF; }
static inline int get_promotion(unsigned int move) { return (move >> 18) & 0xF; }
// move flags
static const int F_EP_CAPTURE = 0x400000;
static const int F_PAWN_START = 0x800000;
static const int F_CASTLE = 0x1000000;
static const int F_CAPTURE = 0x43C000;
static const int F_PROMOTION = 0x3C0000;

/***STRUCTS***/
typedef struct {
	unsigned int move;
	int score;
} Move;

typedef struct {
	Move moves[MAX_POSITION_MOVES];
	int count;
} MoveList;

// Stores a past board state
typedef struct {
	int move;
	unsigned char castlePerm;
	int enPassantSquare;
	int fiftyMoveCounter;
	U64 positionKey;
} PastState;

typedef struct {
	// Tracks state of each square
	int pieces[BRD_SQ_NUM];
	// pawn bitboards for white, black, and combined
	U64 pawns[COLOR_NUM];
	int kingSquare[COLOR_NUM - 1];
	int enPassantSquare;
	int sideToMove;
	int fiftyMoveCounter;
	// current search ply
	int ply;
	// total game ply
	int historyPly;

	// bitwise and of castling rights
	unsigned char castlePerm;
	U64 positionKey;
	
	// How many pieces of each type are on the board
	int pieceCounts[PIECE_NUM];
	// Counts of piece types by color
	int bigPieceCounts[COLOR_NUM];
	int majorPieceCounts[COLOR_NUM];
	int minorPieceCounts[COLOR_NUM];
	int materialScores[COLOR_NUM];

	// Piece list holds square of each piece on the board
	// 1st dimension is piece type and second dimension is instance of said piece type
	int pieceList[PIECE_NUM][10];		

	PastState history[MAX_GAME_MOVES];
} BoardState;


/* ==========================================================================
 * BOARD
 * ========================================================================== */

/***FUNCTIONS***/

// inits all data structures
int init(void);
int file_and_rank_to_120(int f, int r);
int file_and_rank_to_64(int f, int r);
int reset_board(BoardState *state);
int parse_fen(char* fen, BoardState* state);
int print_board(const BoardState* state);
int update_material_list(BoardState* state);
bool check_board(const BoardState* state);

/***Constants***/
// lookup arrays to convert between indices of 120 square and 64 square boards
extern const int SQ_64_TO_120[64];
extern const int SQ_120_TO_64[BRD_SQ_NUM];

/* ==========================================================================
 * BITBOARDS
 * ========================================================================== */


/***FUNCTIONS***/

int init_bit_mask(void);
int print_bitboard(U64 bb);
// pops lowest bit
int pop_bit(U64 *bb);
int count_bits(U64 b);
int set_bit(U64 *bb, int sq);
int clear_bit(U64 *bb, int sq);

/***CONSTANTS***/
extern const int bitTable[64];


/* ==========================================================================
 * HASHKEY
 * ========================================================================== */


/***FUNCTIONS***/

int init_hashkeys(void);
U64 generate_position_key(const BoardState const *state);

/***GLOBALS***/
extern U64 pieceKeys[PIECE_NUM][BRD_SQ_NUM];
extern U64 sideKey;
extern U64 castleKeys[16];


/* ==========================================================================
 * DATA
 * ========================================================================== */

/***CONSTANTS***/
extern const char* PIECE_CHAR;
extern const char* SIDE_CHAR;
extern const char* RANK_CHAR;
extern const char* FILE_CHAR;

extern const int PIECE_VAL[PIECE_NUM];
extern const int PIECE_COLOR[PIECE_NUM];

extern const bool PIECE_BIG[PIECE_NUM];
extern const bool PIECE_MAJOR[PIECE_NUM];
extern const bool PIECE_MINOR[PIECE_NUM];
extern const bool PIECE_KNIGHT[PIECE_NUM];
extern const bool PIECE_KING[PIECE_NUM];
extern const bool PIECE_ROOK_QUEEN[PIECE_NUM];
extern const bool PIECE_BISHOP_QUEEN[PIECE_NUM];
extern const bool PIECE_SLIDES[PIECE_NUM];

extern const int SQ_TO_FILE[BRD_SQ_NUM];
extern const int SQ_TO_RANK[BRD_SQ_NUM];


/* ==========================================================================
 * ATTACK: attack.c
 * ========================================================================== */


/***FUNCTIONS***/
bool sq_attacked(const int sq120, const int side, const BoardState* state);

/* ==========================================================================
 * IO: io.c
 * ========================================================================== */

/***FUNCTIONS***/
char* print_sq(const int sq120);
char* print_move(const unsigned int move);
int print_move_list(const MoveList* list);

/* ==========================================================================
 * VALIDATE: validate.c
 * ========================================================================== */

/***FUNCTIONS***/
bool sq_on_board(const int sq);
bool side_valid(const int side);
bool file_rank_valid(const int fr);
bool piece_valid_empty(const int p); 

/* ==========================================================================
 * MOVEGEN: movegen.c
 * ========================================================================== */

/***FUNTIONS***/
int generate_all_moves(const BoardState* state, MoveList* list);
#endif
