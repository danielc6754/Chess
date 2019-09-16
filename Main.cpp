/*
	CHESS GAME

	PLAYER 1 is White (Top side)
	PLAYER 2 is Black (Bot side)

*/


#define OLC_PGE_APPLICATION
#include "PixelGameEngine.h"

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
		bool dead = false;

		sPiece(int ntype, int nx, int ny) {
			type = ntype;
			x = nx;
			y = ny;
		}
	};

	std::list<sPiece*> player1;
	std::list<sPiece*> player2;
	std::vector<std::pair<int, int>> moveLoc;
	int collisionArr[64] = { 0 };

	// Turn variables
	int posX;
	int posY;

	int currentPlayer = 0;
	sPiece* selectedPiece = nullptr;

	virtual bool OnUserCreate() {
		player1.clear();
		player2.clear();
		moveLoc.clear();

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
		
		// Black pieces bot, White pieces top Queens places on same color tile as the player's pieces
		// Board tiles go white, black, white, black
		player1.push_back(new sPiece(QUEEN, 3, 0));
		player2.push_back(new sPiece(QUEEN, 3, 7));

		player1.push_back(new sPiece(KING, 4, 0));
		player2.push_back(new sPiece(KING, 4, 7));
		
		// Initialize Collision
		for (int i = 0; i < 8; i++) {
			// Player 1
			collisionArr[i] = 1;
			collisionArr[8 + i] = 1;
			collisionArr[48 + i] = 2;
			collisionArr[56 + i] = 2;
		}

		return true;
	}


	// Function to find valid movement location
	// Parameter: take selected unit
	// Output: valid pair location
	void findValidMoves(sPiece* unit) {
		// Collision Location
		int curPos = unit->y * nWidth + unit->x;
		int enemy = currentPlayer == 0 ? 2 : 1;

		if (unit->type == PAWN) {
			int moveUp = currentPlayer == 0 ? 1 : -1;
			int colMoveUp = (moveUp * 8) + curPos;

			// Normal Movement
			if (collisionArr[colMoveUp] == 0)
				moveLoc.push_back(std::make_pair(unit->x, unit->y + moveUp));
			// Enemy Captures
			if (collisionArr[colMoveUp - 1] == enemy)
				moveLoc.push_back(std::make_pair(unit->x - 1, unit->y + moveUp));
			if (collisionArr[colMoveUp + 1] == enemy)
				moveLoc.push_back(std::make_pair(unit->x + 1, unit->y + moveUp));

		}

		if (unit->type == ROOK) {
			// Right
			for (int i = 1; i < 8 - unit->x; i++) {
				if (collisionArr[curPos + i] == 0)
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y));
				if (collisionArr[curPos + i] == 1) {
					break;
				}
				if (collisionArr[curPos + i] == 2) {
					moveLoc.push_back(std::make_pair(unit->x + i, unit->y));
					break;
				}
			}

			// Left
			for (int i = 1; i <= unit->x; i++) {
				if (collisionArr[curPos - i] == 0)
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y));
				if (collisionArr[curPos - i] == 1)
					break;
				if (collisionArr[curPos - i] == 2) {
					moveLoc.push_back(std::make_pair(unit->x - i, unit->y));
					break;
				}
			}
			
			// Down
			for (int i = 1; i < 8 - unit->y; i++) {
				if (collisionArr[curPos + (i * nWidth)] == 0)
					moveLoc.push_back(std::make_pair(unit->x, unit->y + i));
				if (collisionArr[curPos + (i * nWidth)] == 1)
					break;
				if (collisionArr[curPos + (i * nWidth)] == 2) {
					moveLoc.push_back(std::make_pair(unit->x, unit->y + i));
					break;
				}
			}
			
			// Up
			for (int i = 1; i <= unit->y; i++) {
				if (collisionArr[curPos - (i * nWidth)] == 0)
					moveLoc.push_back(std::make_pair(unit->x, unit->y - i));
				if (collisionArr[curPos - (i * nWidth)] == 1)
					break;
				if (collisionArr[curPos - (i * nWidth)] == 2) {
					moveLoc.push_back(std::make_pair(unit->x, unit->y - i));
					break;
				}
			}
			
		}

		if (unit->type == KNIGHT) {

		}

		if (unit->type == BISHOP) {

		}

		if (unit->type == QUEEN) {

		}

		if (unit->type == KING) {
		
		}
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		// Inputs
		if (IsFocused()) {
			if (GetMouse(0).bPressed) {
				posX = (GetMouseX() >> 4);
				posY = (GetMouseY() >> 4);
				
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

							}
						}

						// Captured an enemy
						if (captured) {
							// Enemy piece?
							auto it = (currentPlayer == 1) ? player1.begin() : player2.begin();
							for (auto& p : (currentPlayer == 1) ? player1 : player2) {
								if (p->x == posX && p->y == posY) {
									(currentPlayer == 1) ? player1.erase(it) : player2.erase(it);
									break;
								}
								it++;
							}
						}

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
							findValidMoves(selectedPiece);
							break;
						}
					}
				}
			}
		}

		// Draw
		// Draw Board
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

		return true;
	}


};

int main() {
	Chess game;
	if (game.Construct(128, 128, 4, 4))
		game.Start();
	
	return 0;
}