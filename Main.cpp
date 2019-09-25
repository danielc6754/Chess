/*
	CHESS GAME

	PLAYER 1 is White (Top side)
	PLAYER 2 is Black (Bot side)

	=================== Game Design ===================

	1. Start up/ Setup

	2. Player Phase:
		2.1) Select Unit
			// Unit pointer is either nullptr or a sPiece*
		2.2) Find Valid Movement
			// Return values to vector containing all valid locations
		2.3) Confirm Movement
			// Search movement vector to find if movement is valid
		2.4) Update Collisions
			// Update collisions for new positions and open spaces
		2.5) Update Checks
			// Update checks for locations
		2.6) Remove Captured Unit
			// Remove units that are captured
		2.7) Pawn Check
			// If pawn reaches end, give option to replace pawn with unit

	3. Check for Check/Checkmate

	4. AI (if activated)

	5. Send back to Player

	=================== Data Structures ===================

	// Collision is the size of the board (8 x 8)
		// Used to quickly access if a position is occupied or not 
	Collision : Int array[64]
	
	// Check is size of board (8 x 8)
		// Used to quickly access if position is in position of check
	Check : bool array[64]

	// Player Pieces of size 16 (16 pieces total)
	Pieces : list<sPieces*> p1(16)
*/


#define OLC_PGE_APPLICATION
#include "PixelGameEngine.h"
//#include "Pieces.h"

class Chess : public olc::PixelGameEngine {
public:
	Chess() {
		sAppName = "Chess Demo";
	}

private:
	// Member variables
	// Board
	int nWidth = 8;
	int nHeight = 8;

	// Chess Pieces
	enum pieceType {
		PAWN,
		ROOK,
		KNIGHT,
		BISHOP,
		QUEEN,
		KING
	};
	
	struct sPiece {
		int type;
		int x;
		int y;
		bool moved;

		sPiece(int ntype, int nx, int ny, bool firstMove = false) {
			type = ntype;
			x = nx;
			y = ny;
			moved = firstMove;
		}
	};

	// Player Variables
	std::list<sPiece*> player1;
	std::list<sPiece*> player2;
	
	bool p1CaptureArr[64];
	bool p2CaptureArr[64];

	// Collision
	int collisionArr[64] = { 0 };

	// Movement vector
	std::vector<std::pair<int, int>> moveLoc;
	
	// Turn Variables
	int currentPlayer = 0;
	sPiece* selectedPiece = nullptr;

	// Test game over
	sPiece* p1King = nullptr;
	sPiece* p2King = nullptr;
	bool checkmate = false;
	bool endTurn = false;
	bool drawLast = false;

	virtual bool OnUserCreate() {
		player1.clear();
		player2.clear();
		moveLoc.clear();
		/*
		// Initialize player pieces
		for (int i = 0; i < 8; i++) {
			player1.push_back(new sPiece(PAWN, i, 1));
			player2.push_back(new sPiece(PAWN, i, 6));
		}
		
		for (int i = 0; i < 2; i++) {
			player1.push_back(new sPiece(ROOK, i * 7, 0));
			player2.push_back(new sPiece(ROOK, i * 7, 7));

			player1.push_back(new sPiece(KNIGHT, i * 5 + 1, 0));
			player2.push_back(new sPiece(KNIGHT, i * 5 + 1, 7));

			player1.push_back(new sPiece(BISHOP, i * 3 + 2, 0));
			player2.push_back(new sPiece(BISHOP, i * 3 + 2, 7));
		}
		*/
		// Black pieces bot, White pieces top Queens places on same color tile as the player's pieces
		// Board tiles go white, black, white, black
		player1.push_back(new sPiece(QUEEN, 3, 0));
		player2.push_back(new sPiece(QUEEN, 3, 7));

		player1.push_back(new sPiece(KING, 4, 0));
		player2.push_back(new sPiece(KING, 4, 7));
		
		p1King = player1.back();
		p2King = player2.back();

		// Initialize Collision
		// Testing Collision
		collisionArr[3] = 1;
		collisionArr[4] = 1;
		collisionArr[59] = 2;
		collisionArr[60] = 2;

		/* Quick Collision
		for (int i = 0; i < 8; i++) {			
			// Player 1 Pieces
			collisionArr[i] = 1;
			collisionArr[8 + i] = 1;
			// Player 2 Pieces
			collisionArr[48 + i] = 2;
			collisionArr[56 + i] = 2;
		}
		*/

		// Initialize Capture
		// Since Capture should not be possible in the first turn, set all captures to false
		for (int i = 0; i < 64; i++) {
			p1CaptureArr[i] = false;
			p2CaptureArr[i] = false;
		}

		updateCaptureZone(player1, p1CaptureArr);
		updateCaptureZone(player2, p2CaptureArr);

		return true;
	}


	// Function to find valid movement location
	// Parameter: take selected unit
	// Output: valid pair location
	void findValidMoves(sPiece* unit, bool *checkZone) {
		// Collision Location
		int curPos = unit->y * nWidth + unit->x;
		int enemy = currentPlayer == 0 ? 2 : 1;
		int ally = currentPlayer + 1;

		if (unit->type == PAWN) {
			int moveUp = currentPlayer == 0 ? 1 : -1;
			int colMoveUp = (moveUp * 8) + curPos;

			// Normal Movement
			if (collisionArr[colMoveUp] == 0) {
				moveLoc.push_back(std::make_pair(unit->x, unit->y + moveUp));

				// Starting Movement (move up 2 spaces if no pieces in front)
				if (!unit->moved && collisionArr[colMoveUp + (8 * moveUp)] == 0)
					moveLoc.push_back(std::make_pair(unit->x, unit->y + (2 * moveUp)));
			}
			// Enemy Captures
			if (collisionArr[colMoveUp - 1] == enemy)
				moveLoc.push_back(std::make_pair(unit->x - 1, unit->y + moveUp));
			if (collisionArr[colMoveUp + 1] == enemy)
				moveLoc.push_back(std::make_pair(unit->x + 1, unit->y + moveUp));

		}

		if (unit->type == ROOK) {
			// Right
			for (int i = 1; i < 8 - unit->x; i++) {
				// If free space
				if (collisionArr[curPos + i] == 0)
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y));
				// If player piece
				if (collisionArr[curPos + i] == ally)
					break;
				// If enemy piece
				if (collisionArr[curPos + i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y));
					break;
				}
			}

			// Left
			for (int i = 1; i <= unit->x; i++) {
				if (collisionArr[curPos - i] == 0)
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y));
				if (collisionArr[curPos - i] == ally)
					break;
				if (collisionArr[curPos - i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y));
					break;
				}
			}
			
			// Down
			for (int i = 1; i < 8 - unit->y; i++) {
				if (collisionArr[curPos + (i * nWidth)] == 0)
					moveLoc.push_back(std::make_pair(unit->x, unit->y + i));
				if (collisionArr[curPos + (i * nWidth)] == ally)
					break;
				if (collisionArr[curPos + (i * nWidth)] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x, unit->y + i));
					break;
				}
			}
			
			// Up
			for (int i = 1; i <= unit->y; i++) {
				if (collisionArr[curPos - (i * nWidth)] == 0)
					moveLoc.push_back(std::make_pair(unit->x, unit->y - i));
				if (collisionArr[curPos - (i * nWidth)] == ally)
					break;
				if (collisionArr[curPos - (i * nWidth)] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x, unit->y - i));
					break;
				}
			}
			
		}

		if (unit->type == KNIGHT) {
			// Up Left // Up 2 spaces (- (2 * 8)), Left 1 space (- 1)
			int boundCheck = curPos - (2 * 8) - 1;
			// Check if in bound first
			if (boundCheck >= 0 && boundCheck < 64)
				if (collisionArr[boundCheck] == 0 || collisionArr[boundCheck] == enemy)
					moveLoc.push_back(std::make_pair(unit->x - 1, unit->y - 2));

			// Up Right // Up 2 spaces (- (2 * 8)), Right 1 space (+ 1)
			boundCheck = curPos - (2 * 8) + 1;
			// Check if in bound first
			if (boundCheck >= 0 && boundCheck < 64)
				if (collisionArr[boundCheck] == 0 || collisionArr[boundCheck] == enemy)
					moveLoc.push_back(std::make_pair(unit->x + 1, unit->y - 2));
			
			// Right Up // Right 2 spaces ( + 2), Up 1 space (- 8)
			boundCheck = curPos + 2 - 8;
			// Check if in bound first
			if (boundCheck >= 0 && boundCheck < 64)
				if (collisionArr[boundCheck] == 0 || collisionArr[boundCheck] == enemy)
					moveLoc.push_back(std::make_pair(unit->x + 2, unit->y - 1));
			
			// Right Down // Right 2 space (+ 2), Down 1 space (+ 8)
			boundCheck = curPos + 2 + 8;
			// Check if in bound first
			if (boundCheck >= 0 && boundCheck < 64)
				if (collisionArr[boundCheck] == 0 || collisionArr[boundCheck] == enemy)
					moveLoc.push_back(std::make_pair(unit->x + 2, unit->y + 1));

			// Down Right // Down 2 spaces (+ (2 * 8)), Right 1 space (+ 1)
			boundCheck = curPos + (2 * 8) + 1;
			// Check if in bound first
			if (boundCheck >= 0 && boundCheck < 64)
				if (collisionArr[boundCheck] == 0 || collisionArr[boundCheck] == enemy)
					moveLoc.push_back(std::make_pair(unit->x + 1, unit->y + 2));

			// Down Left // Down 2 spaces (+ (2 * 8)), Left 1 space (- 1)
			boundCheck = curPos + (2 * 8) + 1;
			// Check if in bound first
			if (boundCheck >= 0 && boundCheck < 64)
				if (collisionArr[boundCheck] == 0 || collisionArr[boundCheck] == enemy)
					moveLoc.push_back(std::make_pair(unit->x - 1, unit->y + 2));
			
			// Left Down // Left 2 spaces ( - 2), Down 1 space ( + 8)
			boundCheck = curPos - 2 + 8;
			// Check if in bound first
			if (boundCheck >= 0 && boundCheck < 64)
				if (collisionArr[boundCheck] == 0 || collisionArr[boundCheck] == enemy)
					moveLoc.push_back(std::make_pair(unit->x - 2, unit->y + 1));
			
			// Left Up // Left 2 spaces ( - 2), Up 1 spaces( - 8)
			boundCheck = curPos - 2 - 8;
			// Check if in bound first
			if (boundCheck >= 0 && boundCheck < 64)
				if (collisionArr[boundCheck] == 0 || collisionArr[boundCheck] == enemy)
					moveLoc.push_back(std::make_pair(unit->x - 2, unit->y - 1));
		}

		if (unit->type == BISHOP) {
			// Top Left Diagonal
			for (int i = 1; (i <= unit->x) && (i <= unit->y); i++) {
				if (collisionArr[curPos - (i * nWidth) - i] == 0)
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y - i));
				if (collisionArr[curPos - (i * nWidth) - i] == ally)
					break;
				if (collisionArr[curPos - (i * nWidth) - i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y - i));
					break;
				}
			}

			// Top Right Diagonal
			for (int i = 1; (i < 8 - unit->x) && (i <= unit->y); i++) {
				if (collisionArr[curPos - (i * nWidth) + i] == 0)
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y - i));
				if (collisionArr[curPos - (i * nWidth) + i] == ally)
					break;
				if (collisionArr[curPos - (i * nWidth) + i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y - i));
					break;
				}
			}

			// Bottom Left Diagonal
			for (int i = 1; (i <= unit->x) && (i < 8 - unit->y); i++) {
				if (collisionArr[curPos + (i * nWidth) - i] == 0)
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y + i));
				if (collisionArr[curPos + (i * nWidth) - i] == ally)
					break;
				if (collisionArr[curPos + (i * nWidth) - i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y + i));
					break;
				}
			}

			// Bottom Right Diagonal
			for (int i = 1; (i < 8 - unit->x) && (i < 8 - unit->y); i++) {
				if (collisionArr[curPos + (i * nWidth) + i] == 0)
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y + i));
				if (collisionArr[curPos + (i * nWidth) + i] == ally)
					break;
				if (collisionArr[curPos + (i * nWidth) + i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y + i));
					break;
				}
			}
		}

		if (unit->type == QUEEN) {
			// Up Left Diagonal
			for (int i = 1; (i <= unit->x) && (i <= unit->y); i++) {
				if (collisionArr[curPos - (i * nWidth) - i] == 0)
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y - i));
				if (collisionArr[curPos - (i * nWidth) - i] == ally)
					break;
				if (collisionArr[curPos - (i * nWidth) - i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y - i));
					break;
				}
			}

			// Up
			for (int i = 1; i <= unit->y; i++) {
				if (collisionArr[curPos - (i * nWidth)] == 0)
					moveLoc.push_back(std::make_pair(unit->x, unit->y - i));
				if (collisionArr[curPos - (i * nWidth)] == ally)
					break;
				if (collisionArr[curPos - (i * nWidth)] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x, unit->y - i));
					break;
				}
			}

			// Up Right Diagonal
			for (int i = 1; (i < 8 - unit->x) && (i <= unit->y); i++) {
				if (collisionArr[curPos - (i * nWidth) + i] == 0)
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y - i));
				if (collisionArr[curPos - (i * nWidth) + i] == ally)
					break;
				if (collisionArr[curPos - (i * nWidth) + i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y - i));
					break;
				}
			}

			// Right
			for (int i = 1; i < 8 - unit->x; i++) {
				// If free space
				if (collisionArr[curPos + i] == 0)
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y));
				// If player piece
				if (collisionArr[curPos + i] == ally)
					break;
				// If enemy piece
				if (collisionArr[curPos + i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y));
					break;
				}
			}

			// Bottom Right Diagonal
			for (int i = 1; (i < 8 - unit->x) && (i < 8 - unit->y); i++) {
				if (collisionArr[curPos + (i * nWidth) + i] == 0)
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y + i));
				if (collisionArr[curPos + (i * nWidth) + i] == ally)
					break;
				if (collisionArr[curPos + (i * nWidth) + i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y + i));
					break;
				}
			}

			// Down
			for (int i = 1; i < 8 - unit->y; i++) {
				if (collisionArr[curPos + (i * nWidth)] == 0)
					moveLoc.push_back(std::make_pair(unit->x, unit->y + i));
				if (collisionArr[curPos + (i * nWidth)] == ally)
					break;
				if (collisionArr[curPos + (i * nWidth)] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x, unit->y + i));
					break;
				}
			}

			// Bottom Left Diagonal
			for (int i = 1; (i <= unit->x) && (i < 8 - unit->y); i++) {
				if (collisionArr[curPos + (i * nWidth) - i] == 0)
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y + i));
				if (collisionArr[curPos + (i * nWidth) - i] == ally)
					break;
				if (collisionArr[curPos + (i * nWidth) - i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y + i));
					break;
				}
			}

			// Left
			for (int i = 1; i <= unit->x; i++) {
				if (collisionArr[curPos - i] == 0)
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y));
				if (collisionArr[curPos - i] == ally)
					break;
				if (collisionArr[curPos - i] == enemy) {
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y));
					break;
				}
			}
		}

		if (unit->type == KING) {
			int checkPos = 0;

			// Check Up
			checkPos = curPos - 8;
			if (checkPos >= 0 && checkPos < 64)
				if (collisionArr[checkPos] != ally && !checkZone[checkPos])
					moveLoc.push_back(std::make_pair(unit->x, unit->y - 1));

			// Check Up Right
			checkPos = checkPos + 1;
			if (checkPos >= 0 && checkPos < 64)
				if (collisionArr[checkPos] != ally && !checkZone[checkPos])
					moveLoc.push_back(std::make_pair(unit->x + 1, unit->y - 1));

			// Check Right
			checkPos = checkPos + 8;
			if (checkPos >= 0 && checkPos < 64)
				if (collisionArr[checkPos] != ally && !checkZone[checkPos])
					moveLoc.push_back(std::make_pair(unit->x + 1, unit->y));

			// Check Down Right
			checkPos = checkPos + 8;
			if (checkPos >= 0 && checkPos < 64)
				if (collisionArr[checkPos] != ally && !checkZone[checkPos])
					moveLoc.push_back(std::make_pair(unit->x + 1, unit->y + 1));

			// Check Down
			checkPos = checkPos - 1;
			if (checkPos >= 0 && checkPos < 64)
				if (collisionArr[checkPos] != ally && !checkZone[checkPos])
					moveLoc.push_back(std::make_pair(unit->x, unit->y + 1));

			// Check Down Left
			checkPos = checkPos - 1;
			if (checkPos >= 0 && checkPos < 64)
				if (collisionArr[checkPos] != ally && !checkZone[checkPos])
					moveLoc.push_back(std::make_pair(unit->x - 1, unit->y + 1));

			// Check Left
			checkPos = checkPos - 8;
			if (checkPos >= 0 && checkPos < 64)
				if (collisionArr[checkPos] != ally && !checkZone[checkPos])
					moveLoc.push_back(std::make_pair(unit->x - 1, unit->y));

			// Check Up Left
			checkPos = checkPos - 8;
			if (checkPos >= 0 && checkPos < 64)
				if (collisionArr[checkPos] != ally && !checkZone[checkPos])
					moveLoc.push_back(std::make_pair(unit->x - 1, unit->y - 1));
		}
	}

	// Function to update capture
	void updateCaptureZone(std::list<sPiece*> player, bool *updateArr) {
		// Enemy and Ally
		int enemy = currentPlayer == 0 ? 2 : 1;
		int ally = currentPlayer + 1;

		// Clear update Arr
		for (int i = 0; i < 64; i++)
			updateArr[i] = false;

		for (auto& p : player) {
			int pPos = p->y * nWidth + p->x;
			
			if (p->type == PAWN) {
				// Adjust Pawn direction depending on player
				int direction = currentPlayer == 0 ? 1 : -1;

				// Left Diagonal
				// Check Collision and Position
				if (collisionArr[pPos + (8 * direction) - 1] != ally && p->x != 0) {
					// Check if already true
					if (updateArr[pPos + (8 * direction) - 1])
						updateArr[pPos + (8 * direction) - 1] = true;
				}

				// Right Diagonal
				// Check Collision and Position
				if (collisionArr[pPos + (8 * direction) + 1] != ally && p->x != 7) {
					// Check if already true
					if (updateArr[pPos + (8 * direction) - 1])
						updateArr[pPos + (8 * direction) - 1] = true;
				}
			}

			if (p->type == ROOK) {
				// Used to calculate position to check
				int checkPos = 0;

				// Check UP
				for (int i = 1; i <= p->y; i++) {
					checkPos = pPos - (8 * i);

					// Check Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Check if Enemy
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Check DOWN
				for (int i = 1; i < 8 - p->y; i++) {
					checkPos = pPos + (8 * i);

					// Check Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Check if Enemy
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Check LEFT
				for (int i = 1; i <= p->x; i++) {
					checkPos = pPos - i;

					// Check Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Check if Enemy
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Check RIGHT
				for (int i = 1; i < 8 - p->x; i++) {
					checkPos = pPos + i;

					// Check Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Check if Enemy
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}
			}

			if (p->type == KNIGHT) {
				if (p->y - 2 >= 0) {
					// Top Left (2 Up, 1 Left == - 16 - 1)
					if (p->x - 1 >= 0)
						if (collisionArr[pPos - 16 - 1] != ally && !updateArr[pPos - 16 - 1])
							updateArr[pPos - 16 - 1] = true;
					// Top Right (2 Up, 1 Right == - 16 + 1)
					if (p->x + 1 < 8)
						if (collisionArr[pPos - 16 + 1] != ally && !updateArr[pPos - 16 + 1])
							updateArr[pPos - 16 + 1] = true;
				}

				if (p->x + 2 < 8) {
					// Right Up (2 Right, 1 Up == + 2 - 8)
					if (p->y - 1 >= 0)
						if (collisionArr[pPos + 2 - 8] != ally && !updateArr[pPos + 2 - 8])
							updateArr[pPos + 2 - 8] = true;
					// Right Down (2 Right, 1 Down == + 2 + 8)
					if (p->y + 1 < 8)
						if (collisionArr[pPos + 2 + 8] != ally && !updateArr[pPos + 2 + 8])
							updateArr[pPos + 2 + 8] = true;
				}

				if (p->y + 2 < 8) {
					// Bottom Left (2 Down, 1 Left == + 16 - 1)
					if (p->x - 1 >= 0)
						if (collisionArr[pPos + 16 - 1] != ally && !updateArr[pPos + 16 - 1])
							updateArr[pPos + 16 - 1] = true;
					// Top Right (2 Down, 1 Right == + 16 + 1)
					if (p->x + 1 < 8)
						if (collisionArr[pPos + 16 + 1] != ally && !updateArr[pPos + 16 + 1])
							updateArr[pPos + 16 + 1] = true;
				}

				if (p->x - 2 >= 0) {
					// Left Up (2 Left, 1 Up == - 2 - 8)
					if (p->y - 1 >= 0)
						if (collisionArr[pPos - 2 - 8] != ally && !updateArr[pPos - 2 - 8])
							updateArr[pPos - 2 - 8] = true;
					// Left Down (2 Left, 1 Down == - 2 + 8)
					if (p->y + 1 < 8)
						if (collisionArr[pPos - 2 + 8] != ally && !updateArr[pPos - 2 + 8])
							updateArr[pPos - 2 + 8] = true;
				}
			}

			if (p->type == BISHOP) {
				// Used to calculate position to check
				int checkPos = 0;

				// Top Left Diagonal
				for (int i = 1; i <= p->x && i <= p->y; i++) {
					checkPos = pPos - (i * (8 + 1));
					//Check for Ally Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Enemy Collision
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Top Right Diagonal
				for (int i = 1; i < 8 - p->x && i <= p->y; i++) {
					checkPos = pPos - (i * (8 - 1));
					//Check for Ally Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Enemy Collision
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Bottom Left Diagonal
				for (int i = 1; i <= p->x && i < 8 - p->y; i++) {
					checkPos = pPos + (i * (8 - 1));
					//Check for Ally Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Enemy Collision
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Bottom Right Diagonal
				for (int i = 1; i < 8 - p->x && i < 8 - p->y; i++) {
					checkPos = pPos + (i * (8 + 1));
					//Check for Ally Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Enemy Collision
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}
			}
			
			if (p->type == QUEEN) {
				// Used to calculate position to check
				int checkPos = 0;
				
				// Check UP
				for (int i = 1; i <= p->y; i++) {
					checkPos = pPos - (8 * i);

					// Check Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Check if Enemy
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Top Right Diagonal
				for (int i = 1; i < 8 - p->x && i <= p->y; i++) {
					checkPos = pPos - (i * (8 - 1));

					//Check for Ally Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Enemy Collision
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Check RIGHT
				for (int i = 1; i < 8 - p->x; i++) {
					checkPos = pPos + i;

					// Check Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Check if Enemy
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Bottom Right Diagonal
				for (int i = 1; i < 8 - p->x && i < 8 - p->y; i++) {
					checkPos = pPos + (i * (8 + 1));
					//Check for Ally Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Enemy Collision
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Check DOWN
				for (int i = 1; i < 8 - p->y; i++) {
					checkPos = pPos + (8 * i);

					// Check Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Check if Enemy
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Bottom Left Diagonal
				for (int i = 1; i <= p->x && i < 8 - p->y; i++) {
					checkPos = pPos + (i * (8 - 1));
					//Check for Ally Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Enemy Collision
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}

				// Check LEFT
				for (int i = 1; i <= p->x; i++) {
					checkPos = pPos - i;

					// Check Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Check if Enemy
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}
				
				// Top Left Diagonal
				for (int i = 1; i <= p->x && i <= p->y; i++) {
					checkPos = pPos - (i * (8 + 1));
					//Check for Ally Collision
					if (collisionArr[checkPos] == ally)
						break;

					else {
						// Check if already true
						if (!updateArr[checkPos])
							updateArr[checkPos] = true;
						// Enemy Collision
						if (collisionArr[checkPos] == enemy)
							break;
					}
				}
			}
			
			if (p->type == KING) {
				int checkPos = 0;
				
				// Check Up
				checkPos = pPos - 8;
				if (checkPos >= 0 && checkPos < 64)
					if (collisionArr[checkPos] != ally && !updateArr[checkPos])
						updateArr[checkPos] = true;
				
				// Check Up Right
				checkPos = checkPos + 1;
				if (checkPos >= 0 && checkPos < 64)
					if (collisionArr[checkPos] != ally && !updateArr[checkPos])
						updateArr[checkPos] = true;

				// Check Right
				checkPos = checkPos + 8;
				if (checkPos >= 0 && checkPos < 64)
					if (collisionArr[checkPos] != ally && !updateArr[checkPos])
						updateArr[checkPos] = true;

				// Check Down Right
				checkPos = checkPos + 8;
				if (checkPos >= 0 && checkPos < 64)
					if (collisionArr[checkPos] != ally && !updateArr[checkPos])
						updateArr[checkPos] = true;

				// Check Down
				checkPos = checkPos - 1;
				if (checkPos >= 0 && checkPos < 64)
					if (collisionArr[checkPos] != ally && !updateArr[checkPos])
						updateArr[checkPos] = true;
				
				// Check Down Left
				checkPos = checkPos - 1;
				if (checkPos >= 0 && checkPos < 64)
					if (collisionArr[checkPos] != ally && !updateArr[checkPos])
						updateArr[checkPos] = true;

				// Check Left
				checkPos = checkPos - 8;
				if (checkPos >= 0 && checkPos < 64)
					if (collisionArr[checkPos] != ally && !updateArr[checkPos])
						updateArr[checkPos] = true;

				// Check Up Left
				checkPos = checkPos - 8;
				if (checkPos >= 0 && checkPos < 64)
					if (collisionArr[checkPos] != ally && !updateArr[checkPos])
						updateArr[checkPos] = true;
			}
			
		}
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		// Inputs
		if (IsFocused()) {
			if (!checkmate) {
				if (GetMouse(0).bPressed) {
					int posX = (GetMouseX() >> 4);
					int posY = (GetMouseY() >> 4);

					// Logic
					if (selectedPiece != nullptr) {
						// Check if same unit
						if (posX == selectedPiece->x && posY == selectedPiece->y) {
							selectedPiece = nullptr;

							// Clear previous valid unit movement locations
							moveLoc.clear();
						}

						else {

							bool captured = false;

							// Check if move is valid
							for (auto change : moveLoc) {
								if (change.first == posX && change.second == posY) {
									// Captured Enemy?
									if (collisionArr[posY * nWidth + posX] == (currentPlayer == 0 ? 2 : 1))
										captured = true;

									// Update collisions
									collisionArr[selectedPiece->y * nWidth + selectedPiece->x] = 0;
									collisionArr[posY * nWidth + posX] = currentPlayer + 1;
									// Update Unit position
									selectedPiece->x = posX;
									selectedPiece->y = posY;

									// If first movement, set moved to true
									if (selectedPiece->moved == false)
										selectedPiece->moved = true;

									// Clear selection
									selectedPiece = nullptr;
									// clear movement tiles
									moveLoc.clear();

									endTurn = true;

								}
							}

							// Captured an enemy
							if (captured) {
								// Find Enemy piece and get rid of it
								auto it = (currentPlayer == 1) ? player1.begin() : player2.begin();
								for (auto& p : (currentPlayer == 1) ? player1 : player2) {
									if (p->x == posX && p->y == posY) {
										(currentPlayer == 1) ? player1.erase(it) : player2.erase(it);
										break;
									}
									it++;
								}

								endTurn = true;
							}
							/*
							// Check if Pawn reached end
							if (selectedPiece->type == PAWN) {
								// Player 1 reach end
								if (selectedPiece->y == 7 && currentPlayer == 0) {
									// Give option for chess piece
								}
								// Player 2 reach end
								if (selectedPiece->y == 0 && currentPlayer == 1) {
									// Give  option for chess piece
								}
							}*/

						}

					}

					// No Piece selected
					else {
						/*
						// Player 2
						if (currentPlayer) {
							for (auto& p : player2) {
								if (p->x == posX && p->y == posY) {
									selectedPiece = p;
									break;
								}
							}
						}

						// Player 1
						else {
							for (auto& p : player1) {
								if (p->x == posX && p->y == posY) {
									selectedPiece = p;
									break;
								}
							}
						}
						*/
						// Condensed form of above statement less lines, but a bit more complex
						for (auto& p : (currentPlayer == 1 ? player2 : player1)) {
							if (p->x == posX && p->y == posY) {
								selectedPiece = p;

								// Find valid Movement zones
								findValidMoves(selectedPiece, currentPlayer == 0 ? p2CaptureArr : p1CaptureArr);
								break;
							}
						}
					}
				}
			}
		}


		// Game Logic turn stuff
		if (endTurn) {
			// Checkmate?
			moveLoc.clear();

			if (currentPlayer == 0)
				findValidMoves(player1.back(), p1CaptureArr);
			if (currentPlayer == 1)
				findValidMoves(player2.back(), p2CaptureArr);

			if (moveLoc.empty()) {
				checkmate = true;
				drawLast = true;
			}
			if (!checkmate) {
				// Next Player
				currentPlayer = (currentPlayer == 0 ? 1 : 0);
				// Update Capture arrays
				updateCaptureZone(player1, p1CaptureArr);
				updateCaptureZone(player2, p2CaptureArr);

				moveLoc.clear();

				endTurn = false;
			}
		}

		// Draw
		// Draw Board
		// While not checkmated
		if (!checkmate) {
			for (int i = 0; i < nWidth; i++) {
				for (int j = 0; j < nHeight; j++) {
					// Chessboard color coding is achieved by logical NOR where either the mod of 2 of x and y is 1 or 0
					FillRect(i << 4, j << 4, 16, 16, (i % 2 == j % 2) ? olc::BLACK : olc::WHITE);
				}
			}

			// Draw Valid Spaces
			for (auto& sp : moveLoc)
				FillRect((sp.first << 4) + 1, (sp.second << 4) + 1, 14, 14, olc::GREEN);

			if (selectedPiece != nullptr)
				FillRect((selectedPiece->x << 4) + 1, (selectedPiece->y << 4) + 1, 14, 14, olc::YELLOW);

			// Draw Pieces
			for (auto& piece : player1) {
				FillRect(piece->x * 16 + 4, piece->y * 16 + 4, 8, 8, olc::BLUE);
			}

			for (auto& piece : player2) {
				FillRect(piece->x * 16 + 4, piece->y * 16 + 4, 8, 8, olc::RED);
			}
		}

		else {
			if (drawLast) {
				for (int i = 0; i < nWidth; i++) {
					for (int j = 0; j < nHeight; j++) {
						// Chessboard color coding is achieved by logical NOR where either the mod of 2 of x and y is 1 or 0
						FillRect(i << 4, j << 4, 16, 16, (i % 2 == j % 2) ? olc::BLACK : olc::WHITE);
					}
				}

				// Draw Valid Spaces
				for (auto& sp : moveLoc)
					FillRect((sp.first << 4) + 1, (sp.second << 4) + 1, 14, 14, olc::GREEN);

				if (selectedPiece != nullptr)
					FillRect((selectedPiece->x << 4) + 1, (selectedPiece->y << 4) + 1, 14, 14, olc::YELLOW);

				// Draw Pieces
				for (auto& piece : player1) {
					FillRect(piece->x * 16 + 4, piece->y * 16 + 4, 8, 8, olc::BLUE);
				}

				for (auto& piece : player2) {
					FillRect(piece->x * 16 + 4, piece->y * 16 + 4, 8, 8, olc::RED);
				}

				drawLast = false;
			}

			FillRect(21, 48, 87, 16, olc::DARK_GREY);
			FillRect(23, 50, 83, 12, olc::Pixel(100, 100, 200));

			DrawString(25, 52, "CHECKMATE!", olc::WHITE, 1);
		}
		return true;
	}


};

int main() {
	Chess game;
	if (game.Construct(128, 128, 4, 4))
		game.Start();
	
	return 0;
}