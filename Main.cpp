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
	
	// Check is size of board (8 x 8) // 2 Player to have separate check arrays
		// Used to quickly access if position is in position of check
	Check : bool array[64]

	// Player Pieces of size 16 (16 pieces total) // 2 players so 2 groups of 16 (32 total)
	Pieces : list<sPieces*> p1(16)
*/


#define OLC_PGE_APPLICATION
#include "PixelGameEngine.h"
#include "Pieces.h"

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
	
	// Player Variables
	std::list<cPiece*> player1;
	std::list<cPiece*> player2;
	
	bool p1CaptureArr[64];
	bool p2CaptureArr[64];

	// Collision
	int collisionArr[64] = { 0 };

	// Movement vector
	std::vector<std::pair<int, int>> moveLoc;
	
	// Player Turn Variables
	int currentPlayer = 0;
	cPiece* selectedPiece = nullptr;

	// Game Phases
	enum class gamePhase : short {
		Setup = 0x01,
		PlayerPhase = 0x02,
		UpdatePhase = 0x03,
		CheckPhase = 0x04,
		GameOver = 0x05
	} phase;

	enum pieceType {
		PAWN,
		ROOK,
		KNIGHT,
		BISHOP,
		QUEEN,
		KING
	};


	virtual bool OnUserCreate() {
		// Clear just incase
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
		player2.push_back(new Queen(3, 3, QUEEN));
		player1.push_back(new King(3, 1, KING));

		// Initialize Collision
		// Testing Collision
		collisionArr[27] = 2;
		collisionArr[11] = 3;
		//collisionArr[4] = 1;
		//collisionArr[59] = 2;
		//collisionArr[60] = 2;

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
		for (int i = 0; i < 64; i++) {
			p1CaptureArr[i] = false;
			p2CaptureArr[i] = false;
		}

		updateCaptureZone(player1, p1CaptureArr);
		//updateCaptureZone(player2, p2CaptureArr);

		phase = gamePhase::PlayerPhase;

		return true;
	}

	// Function to update capture
	void updateCaptureZone(std::list<cPiece*> player, bool *updateArr) {
		// Enemy and Ally
		int enemy = currentPlayer == 0 ? 2 : 1;
		int ally = currentPlayer + 1;

		// Clear update Arr
		for (int i = 0; i < 64; i++)
			updateArr[i] = false;

		for (auto& p : player) {
			p->updateZones(currentPlayer, collisionArr, updateArr);
			
		}
	}

	virtual bool OnUserUpdate(float fElapsedTime) {

		// Game Phases
		switch (phase) {

		// Game Reset
		/*
		case (Setup) : {
			setupGame();
			phase = PlayerPhase;
		}
	
		break;

		*/

		case (gamePhase::PlayerPhase): {

			// Inputs
			// Is window in focus
			if (IsFocused()) {
				// Left click
				if (GetMouse(0).bPressed) {

					// Mouse position
					// divided by 16 (bit shift 4) to find proper location in data structure space
					int posX = (GetMouseX() >> 4);
					int posY = (GetMouseY() >> 4);

					// =================== Logic ===============================

					// Is piece selected
					if (selectedPiece != nullptr) {
						// Check if same unit
						if (posX == selectedPiece->x && posY == selectedPiece->y) {
							// Deselect unit
							selectedPiece = nullptr;

							// Clear previous valid unit movement locations
							moveLoc.clear();
						}

						// Clicked somewhere else
						else {

							// Check if move is in valid movements
							for (auto change : moveLoc) {

								// Movement is valid
								if (change.first == posX && change.second == posY) {

									// Collision with enemy
									if (collisionArr[posY * nWidth + posX] == (currentPlayer == 0 ? 2 : 1)) {
										// Find Enemy piece and get rid of it
										auto it = (currentPlayer == 1) ? player1.begin() : player2.begin();
										for (auto& p : (currentPlayer == 1) ? player1 : player2) {
											if (p->x == posX && p->y == posY) {
												(currentPlayer == 1) ? player1.erase(it) : player2.erase(it);
												break;
											}
											it++;
										}
									}

									// Update collisions
									collisionArr[selectedPiece->y * nWidth + selectedPiece->x] = 0;
									if (selectedPiece->type == KING)
										collisionArr[posY * nWidth + posX] = currentPlayer + 3;
									else
										collisionArr[posY * nWidth + posX] = currentPlayer + 1;

									// Update Unit position
									selectedPiece->x = posX;
									selectedPiece->y = posY;


									// Clear selection
									//selectedPiece = nullptr;

									// clear movement tiles
									moveLoc.clear();

									phase = gamePhase::UpdatePhase;

									break;
								}
							}
						}
					}

					// No Piece selected
					else {
						// Find if piece exist in click location
						for (auto& p : (currentPlayer == 1 ? player2 : player1)) {
							// Piece found
							if (p->x == posX && p->y == posY) {
								// select piece
								selectedPiece = p;

								// Find valid Movement zones
								moveLoc = p->movement(currentPlayer, collisionArr, currentPlayer == 0 ? p2CaptureArr : p1CaptureArr);
								break;
							}
						}
					}
				}
			}
		}
		break;

		case (gamePhase::UpdatePhase): {
			
			// Update Capture Zone
			updateCaptureZone(player1, p1CaptureArr);
			updateCaptureZone(player2, p2CaptureArr);

			cPiece* checkKing = (currentPlayer == 0) ? player2.front() : player1.front();
			bool* checkCap = (currentPlayer == 0) ? p1CaptureArr : p2CaptureArr;
				
			// Check for check/checkmate
			// if King is in check
			if (checkCap[checkKing->y * nWidth + checkKing->x] == currentPlayer + 1) {
				// Find all of King's moves available
				std::vector<std::pair<int, int>> kingMoves = checkKing->movement(currentPlayer, collisionArr, checkCap);

				// If no more moves
				if (kingMoves.empty()) {
					// Find if another piece can block
				
				}
					


				// If King has moves, Next Turn
				else {
					currentPlayer = (currentPlayer == 0) ? 1 : 0;
					phase = gamePhase::PlayerPhase;
				}
			}

			else {
				// Check for Stalemate
				if (checkKing->type == (currentPlayer == 0) ? player1.back()->type : player2.back()->type) {
					// Find all of King's moves available
					std::vector<std::pair<int, int>> kingMoves = checkKing->movement(currentPlayer, collisionArr, checkCap);

					// If no more moves, Stalemate
					if (kingMoves.empty())
						phase = gamePhase::GameOver;
					
					
					// If King has moves, Next Turn
					else {
						currentPlayer = (currentPlayer == 0) ? 1 : 0;
						phase = gamePhase::PlayerPhase;
					}
				}

				// Next Player's turn
				else {
					currentPlayer = (currentPlayer == 0) ? 1 : 0;
					phase = gamePhase::PlayerPhase;
				}
			}

			selectedPiece = nullptr;
		}
		break;

		case (gamePhase::CheckPhase): {
			phase = gamePhase::PlayerPhase;
		}
		break;

		}

		// Draw
		switch (phase) {
		case (gamePhase::PlayerPhase): {
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

			// Draw Player 1 Pieces
			for (auto& piece : player1) {
				FillRect(piece->x * 16 + 4, piece->y * 16 + 4, 8, 8, olc::BLUE);
			}

			// Player 2 Pieces
			for (auto& piece : player2) {
				FillRect(piece->x * 16 + 4, piece->y * 16 + 4, 8, 8, olc::RED);
			}

		}

		break;

		// Draw the Board 1 more time
		case (gamePhase::UpdatePhase): {
			// Draw Board
			for (int i = 0; i < nWidth; i++) {
				for (int j = 0; j < nHeight; j++) {
					// Chessboard color coding is achieved by logical NOR where either the mod of 2 of x and y is 1 or 0
					FillRect(i << 4, j << 4, 16, 16, (i % 2 == j % 2) ? olc::BLACK : olc::WHITE);
				}
			}

			// Draw Pieces
			for (auto& piece : player1) {
				FillRect(piece->x * 16 + 4, piece->y * 16 + 4, 8, 8, olc::BLUE);
			}

			for (auto& piece : player2) {
				FillRect(piece->x * 16 + 4, piece->y * 16 + 4, 8, 8, olc::RED);
			}

		}
	
		break;

		/*
		case (CheckPhase) :
			// Draw Text Box
			FillRect(21, 48, 87, 16, olc::DARK_GREY);
			FillRect(23, 50, 83, 12, olc::Pixel(100, 100, 200));

			// Draw String
			DrawString(25, 52, "CHECK", olc::WHITE, 1);

			break;
			*/

		// Game Over Screen
		case (gamePhase::GameOver): {
			// Draw Text Box
			FillRect(21, 48, 87, 16, olc::DARK_GREY);
			FillRect(23, 50, 83, 12, olc::Pixel(100, 100, 200));

			// Draw String
			DrawString(25, 52, "CHECKMATE!", olc::WHITE, 1);
		}

		break;

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