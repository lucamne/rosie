#include "defs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


long get_leaf_nodes(struct BoardState* state, int depth) {
	if (depth == 0) { return 1; }

	struct MoveList* mList = malloc(sizeof(struct MoveList));
	generate_all_moves(state, mList);

	long leafNodes = 0;
	for (int i = 0; i < mList->count; i ++) {
		// if move would leave king in check then skip
		if (!make_move(state, mList->moves[i].move)) { continue; }

		leafNodes += get_leaf_nodes(state, depth - 1);

		take_move(state);
	}
	free(mList);
	return leafNodes;
}

long perft(struct BoardState* state, int depth, bool show_output) {
	assert(depth > 0);

	struct MoveList* mList = malloc(sizeof(struct MoveList));
	generate_all_moves(state, mList);

	long leafNodes = 0;

	for (int i = 0; i < mList->count; i++) {

		if (!make_move(state, mList->moves[i].move)) { continue; }
		const int t_leafNodes = get_leaf_nodes(state, depth - 1);
		take_move(state);

		if (show_output) {
			printf("\t\t%s : %d\n", print_move(mList->moves[i].move), t_leafNodes);
		}
		leafNodes += t_leafNodes;
	}
	
	if (show_output) {
		printf("\t\tLeaf nodes : %ld\n", leafNodes);
	}
	return leafNodes;
}

void perft_from_file(struct BoardState* state, char* perft_file, bool verbose) {

	FILE* f = fopen(perft_file, "r");
	if (!f) {
		fprintf(stderr, "Could not open perft file: %s\n", perft_file);
		return;
	}
	
	int testCounter = 0;
	char* pos;
	char* side;
	char* castle;
	char* ep;
	char* fiftyMove;
	char* fullMoves;
	int depth;
	long leafNodes;
	while (fscanf(f, "%ms %ms%ms%ms%ms%ms%d%ld", 
		&pos, 
		&side, 
		&castle, 
		&ep, 
		&fiftyMove, 
		&fullMoves, 
		&depth, 
		&leafNodes) != EOF) {

		// +5 for 4 space chars and a null byte
		const int fenLen = strlen(pos) 
			+ strlen(side) 
			+ strlen(castle) 
			+ strlen(ep) 
			+ strlen(fiftyMove) 
			+ strlen(fullMoves) + 6;

		char* fen = malloc(sizeof(char) * fenLen);

		fen = strcpy(fen, pos);
		fen = strcat(fen, " ");
		fen = strcat(fen, side);
		fen = strcat(fen, " ");
		fen = strcat(fen, castle);
		fen = strcat(fen, " ");
		fen = strcat(fen, ep);
		fen = strcat(fen, " ");
		fen = strcat(fen, fiftyMove);
		fen = strcat(fen, " ");
		fen = strcat(fen, fullMoves);

		free(side);
		free(fullMoves);
		free(fiftyMove);
		free(pos);
		free(castle);
		free(ep);

		// # is used to comment out lines in perft test file
		if (*fen == '#') {
			free(fen);
			continue;
		}

		printf("%d. Depth: %d Expected Nodes: %ld\n", ++testCounter, depth, leafNodes);

		parse_fen(fen, state);
		free(fen);
				const long nodesFound = perft(state, depth, verbose);
		if (nodesFound == leafNodes) {
			printf("\tSuccess!\n");
		} else {
			printf("\tFailed. Expected: %ld Found: %ld\n", leafNodes, nodesFound);
		}
	}

	if (fclose(f)) {
		fprintf(stderr, "Error closing file: %s\n", perft_file);
	}
}

