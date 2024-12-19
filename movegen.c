#include "defs.h"
#include "stdio.h"

// used to control looping through all non pawn pieces
static const int LOOP_SLIDE_PIECE[8] = {wB, wR, wQ, 0, bB, bR, bQ, 0};
static const int LOOP_SLIDE_INDEX[2] = {0, 4};

static const int LOOP_NON_SLIDE_PIECE[6] = {wN, wK, 0, bN, bK, 0};
static const int NON_SLIDE_INDEX[2] = {0, 3};

// offsets describing movement directions for non pawns
static const int PIECE_DIRECTIONS[PIECE_NUM][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{-8, -19, -21, -12, 8, 19, 21, 12},
	{-9, -11, 11, 9, 0, 0, 0, 0},
	{-1, -10, 1, 10, 0, 0, 0, 0},
	{-1, -10, 1, 10, -9, -11, 11, 9},
	{-1, -10, 1, 10, -9, -11, 11, 9},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{-8, -19, -21, -12, 8, 19, 21, 12},
	{-9, -11, 11, 9, 0, 0, 0, 0},
	{-1, -10, 1, 10, 0, 0, 0, 0},
	{-1, -10, 1, 10, -9, -11, 11, 9},
	{-1, -10, 1, 10, -9, -11, 11, 9} 
};
static const int NUM_DIRECTIONS[PIECE_NUM] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

static inline unsigned int build_move(unsigned int from, unsigned int to, unsigned int capture, 
				      unsigned int promote, unsigned int flags) {
	return from | (to << 7) | (capture << 14) | (promote << 18) | flags;
}

static inline bool sq_offboard(const int sq120) {return SQ_TO_FILE[sq120] == FILE_NONE;}

int add_quiet_move(const BoardState* state, unsigned int move, MoveList* list) {
	// list->moves is an array of structs of type Move
	// struct Move contains int move and int score 
	list->moves[list->count].move = move;
	list->moves[list->count].score = 0;
	list->count++;
	return 0;
}

int add_capture_move(const BoardState* state, unsigned int move, MoveList* list) {
	// list->moves is an array of structs of type Move
	// struct Move contains int move and int score 
	list->moves[list->count].move = move;
	list->moves[list->count].score = 0;
	list->count++;
	return 0;
}

int add_en_passant_move(const BoardState* state, unsigned int move, MoveList* list) {
	// list->moves is an array of structs of type Move
	// struct Move contains int move and int score 
	list->moves[list->count].move = move;
	list->moves[list->count].score = 0;
	list->count++;
	return 0;
}

// in between function to handle pawn promotions
int add_wP_cap_move(const BoardState* state, const unsigned int from,
			const unsigned int to, const unsigned int capture,
			MoveList* list) {

	assert(piece_valid_empty(capture));
	assert(sq_on_board(from));
	assert(sq_on_board(to));

	if(SQ_TO_RANK[from] == RANK_7) {
		add_capture_move(state, build_move(from, to, capture, wQ, 0), list);
		add_capture_move(state, build_move(from, to, capture, wR, 0), list);
		add_capture_move(state, build_move(from, to, capture, wB, 0), list);
		add_capture_move(state, build_move(from, to, capture, wN, 0), list);
	} else {
		add_capture_move(state, build_move(from, to, capture, EMPTY, 0), list);
	}
	return 0;
}

int add_wP_move(const BoardState* state, const unsigned int from,
			const unsigned int to,
			MoveList* list) {

	assert(sq_on_board(from));
	assert(sq_on_board(to));

	if(SQ_TO_RANK[from] == RANK_7) {
		add_quiet_move(state, build_move(from, to, EMPTY, wQ, 0), list);
		add_quiet_move(state, build_move(from, to, EMPTY, wR, 0), list);
		add_quiet_move(state, build_move(from, to, EMPTY, wB, 0), list);
		add_quiet_move(state, build_move(from, to, EMPTY, wN, 0), list);
	} else {
		add_quiet_move(state, build_move(from, to, EMPTY, EMPTY, 0), list);
	}
	return 0;
}
// in between function to handle pawn promotions
int add_bP_cap_move(const BoardState* state, const unsigned int from,
			const unsigned int to, const unsigned int capture,
			MoveList* list) {

	assert(piece_valid_empty(capture));
	assert(sq_on_board(from));
	assert(sq_on_board(to));

	if(SQ_TO_RANK[from] == RANK_2) {
		add_capture_move(state, build_move(from, to, capture, bQ, 0), list);
		add_capture_move(state, build_move(from, to, capture, bR, 0), list);
		add_capture_move(state, build_move(from, to, capture, bB, 0), list);
		add_capture_move(state, build_move(from, to, capture, bN, 0), list);
	} else {
		add_capture_move(state, build_move(from, to, capture, EMPTY, 0), list);
	}
	return 0;
}

int add_bP_move(const BoardState* state, const unsigned int from,
			const unsigned int to,
			MoveList* list) {

	assert(sq_on_board(from));
	assert(sq_on_board(to));

	if(SQ_TO_RANK[from] == RANK_2) {
		add_quiet_move(state, build_move(from, to, EMPTY, bQ, 0), list);
		add_quiet_move(state, build_move(from, to, EMPTY, bR, 0), list);
		add_quiet_move(state, build_move(from, to, EMPTY, bB, 0), list);
		add_quiet_move(state, build_move(from, to, EMPTY, bN, 0), list);
	} else {
		add_quiet_move(state, build_move(from, to, EMPTY, EMPTY, 0), list);
	}
	return 0;
}


int generate_all_moves(const BoardState* state, MoveList* list) {
	assert(check_board(state));
	
	const int side = state->sideToMove;
	list->count = 0;

	printf("\n\nSide to move: %d\n", side);

	if (side == WHITE) {
		for (int pieceCount = 0; pieceCount < state->pieceCounts[wP]; pieceCount++) {
			const int sq120 = state->pieceList[wP][pieceCount];
			assert(!sq_offboard(sq120));	
			assert(sq120 >= 0);
			// + 10 adds 1 rank
			if (state->pieces[sq120 + 10] == EMPTY) {
				add_wP_move(state, sq120, sq120 + 10, list);
				// + 20 adds to ranks	
				if (SQ_TO_RANK[sq120] == RANK_2 && state->pieces[sq120 + 20] == EMPTY) {
					add_quiet_move(
							state, 
							build_move(
								sq120, 
								sq120 + 20, 
								EMPTY, 
								EMPTY, F_PAWN_START),
							list);
				}
			}
			// check for possible captures
			if (!sq_offboard(sq120 + 9) && PIECE_COLOR[state->pieces[sq120 + 9]] == BLACK) {
					add_wP_cap_move(state, sq120, sq120 + 9, state->pieces[sq120 + 9], list);
			}
			if (!sq_offboard(sq120 + 11) && PIECE_COLOR[state->pieces[sq120 + 11]] == BLACK) {
					add_wP_cap_move(state, sq120, sq120 + 11, state->pieces[sq120 + 11], list);
			}
			// handle en passant captures
			if (sq120 + 9 == state->enPassantSquare) {
				add_capture_move(
						state, 
						build_move(
							sq120, 
							sq120 + 9, 
							EMPTY, EMPTY, 
							F_EP_CAPTURE),
						list);
			}
			if (sq120 + 11 == state->enPassantSquare) {
				add_capture_move(
						state, 
						build_move(
							sq120, 
							sq120 + 11, 
							EMPTY, EMPTY, 
							F_EP_CAPTURE),
						list);
			}
		}
	} else {
		for (int pieceCount = 0; pieceCount < state->pieceCounts[bP]; pieceCount++) {
			const int sq120 = state->pieceList[bP][pieceCount];
			assert(!sq_offboard(sq120));	
			assert(sq120 >= 0);
			// + 10 adds 1 rank
			if (state->pieces[sq120 - 10] == EMPTY) {
				add_bP_move(state, sq120, sq120 - 10, list);
				// + 20 adds to ranks	
				if (SQ_TO_RANK[sq120] == RANK_7 && state->pieces[sq120 - 20] == EMPTY) {
					add_quiet_move(
							state, 
							build_move(
								sq120, 
								sq120 - 20, 
								EMPTY, 
								EMPTY, F_PAWN_START),
							list);
				}
			}
			// check for possible captures
			if (!sq_offboard(sq120 - 9) && PIECE_COLOR[state->pieces[sq120 - 9]] == WHITE) {
					add_bP_cap_move(state, sq120, sq120 - 9, state->pieces[sq120 - 9], list);
			}
			if (!sq_offboard(sq120 - 11) && PIECE_COLOR[state->pieces[sq120 - 11]] == WHITE) {
					add_bP_cap_move(state, sq120, sq120 - 11, state->pieces[sq120 - 11], list);
			}
			// handle en passant captures
			if (sq120 - 9 == state->enPassantSquare) {
				add_capture_move(
						state, 
						build_move(
							sq120, 
							sq120 - 9, 
							EMPTY, EMPTY, 
							F_EP_CAPTURE),
						list);
			}
			if (sq120 - 11 == state->enPassantSquare) {
				add_capture_move(
						state, 
						build_move(
							sq120, 
							sq120 - 11, 
							EMPTY, EMPTY, 
							F_EP_CAPTURE),
						list);
			}
		}

	}

	// loop for slide pieces
	int pieceIndex = LOOP_SLIDE_INDEX[side];
	int piece = LOOP_SLIDE_PIECE[pieceIndex++];
	while (piece != 0) {
		assert(piece_valid_empty(piece));
		printf("sliders pieceIndex: %d piece: %d\n", pieceIndex, piece);

		for (int pieceCount = 0; pieceCount < state->pieceCounts[piece]; pieceCount++) {
			const int sq120 = state->pieceList[piece][pieceCount];
			assert(!sq_offboard(sq120));
			printf("Piece: %c on %s\n", PIECE_CHAR[piece], print_sq(sq120));

			for (int i = 0; i < NUM_DIRECTIONS[piece]; i++) {
				
				const int dir = PIECE_DIRECTIONS[piece][i];
				int targetSq = sq120 + dir;

				while (!sq_offboard(targetSq)) {
					if (state->pieces[targetSq] != EMPTY) {
						if (PIECE_COLOR[state->pieces[targetSq]] == side ^ 1) {
							printf("\t\tCapture on %s\n", print_sq(targetSq));
						}
						break;
					}
					printf("\t\tNormal on %s\n", print_sq(targetSq));
					targetSq += dir;
				}
			}
		}
		piece = LOOP_SLIDE_PIECE[pieceIndex++];
	}

	// loop for non slides
	pieceIndex = NON_SLIDE_INDEX[side];
	piece = LOOP_NON_SLIDE_PIECE[pieceIndex++];
	while (piece != 0) {
		assert(piece_valid_empty(piece));
		printf("non-sliders pieceIndex: %d piece: %d\n", pieceIndex, piece);

		for (int pieceCount = 0; pieceCount < state->pieceCounts[piece]; pieceCount++) {
			const int sq120 = state->pieceList[piece][pieceCount];
			assert(!sq_offboard(sq120));
			printf("Piece: %c on %s\n", PIECE_CHAR[piece], print_sq(sq120));

			for (int i = 0; i < NUM_DIRECTIONS[piece]; i++) {
				const int dir = PIECE_DIRECTIONS[piece][i];
				const int targetSq = sq120 + dir;

				if (sq_offboard(targetSq)) { continue; }

				if (state->pieces[targetSq] != EMPTY) {
					if (PIECE_COLOR[state->pieces[targetSq]] == side ^ 1) {
						printf("\t\tCapture on %s\n", print_sq(targetSq));
					}
					continue;
				}
				printf("\t\tNormal on %s\n", print_sq(targetSq));
			}
		}

		piece = LOOP_NON_SLIDE_PIECE[pieceIndex++];
	}
	return 0;
}

