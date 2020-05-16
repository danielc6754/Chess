#include "Pieces.h"

cPiece::cPiece(int nx, int ny, int ntype) {
	x = nx;
	y = ny;
	type = ntype;
}

 // ######################### PAWN ##################################### //

// Call base constructor to initialize first varibles
// Additional Setup afterwards
Pawn::Pawn(int nx, int ny, int ntype, bool fm) : cPiece(nx, ny, ntype) {
	firstMove = fm;
}

std::vector<std::pair<int, int>> Pawn::movement(int curPlayer, int* collision, bool* checkZones = nullptr) {
	// return movement vector
	std::vector<std::pair<int, int>> moveLoc;

	// Collision Location
	int curPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Some setup Variables
	int moveUp = curPlayer == 0 ? 1 : -1;
	int colMoveUp = (moveUp * 8) + curPos;

	// Normal Movement
	if (collision[colMoveUp] == 0) {
		moveLoc.push_back(std::make_pair(x, y + moveUp));

		// Starting Movement (move up 2 spaces if no pieces in front)
		if (firstMove && collision[colMoveUp + (8 * moveUp)] == 0)
			moveLoc.push_back(std::make_pair(x, y + (2 * moveUp)));
	}
	// Valid capture zones
	if (collision[colMoveUp - 1] == enemy || collision[colMoveUp - 1] == enemy + 2)
		moveLoc.push_back(std::make_pair(x - 1, y + moveUp));
	if (collision[colMoveUp + 1] == enemy || collision[colMoveUp - 1] == enemy + 2)
		moveLoc.push_back(std::make_pair(x + 1, y + moveUp));

	return moveLoc;
}

void Pawn::updateZones(int curPlayer, int* collision, bool* oldZone) {

	// Quick position calc
	int pPos = y * 8 + x;

	// Adjust Pawn direction depending on player
	int direction = curPlayer == 0 ? 1 : -1;

	// Left Diagonal
	// Check Collision and Position
	if (x != 0) {
		if (collision[pPos + (8 * direction) - 1] != curPlayer + 1 || collision[pPos + (8 * direction) - 1] != curPlayer + 3) {
			// Check if already true
			if (!oldZone[pPos + (8 * direction) - 1])
				collision[pPos + (8 * direction) - 1] = true;
		}
	}

	// Right Diagonal
	// Check Collision and Position
	if (x != 7) {
		if (collision[pPos + (8 * direction) + 1] != curPlayer + 1 || collision[pPos + (8 * direction) + 1] != curPlayer + 3) {
			// Check if already true
			if (!collision[pPos + (8 * direction) - 1])
				collision[pPos + (8 * direction) - 1] = true;
		}
	}
}



// ######################### ROOK ##################################### //
// Inherits orignal constructor
// If additional setup is needed for this piece add in
Rook::Rook(int nx, int ny, int ntype, bool fm) : cPiece(nx, ny, ntype) {
	firstMove = fm;
}

std::vector<std::pair<int, int>> Rook::movement(int curPlayer, int* collision, bool* checkZones = nullptr) {
	// return movement vector
	std::vector<std::pair<int, int>> moveLoc;

	// Collision Location
	int curPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Check UP
	for (int i = 1; i <= y; i++) {
		int checkLoc = curPos - (8 * i);

		// Check Collision
		if (collision[checkLoc] == ally || collision[checkLoc] == ally + 2)
			break;

		else {
			// Valid movement
			moveLoc.push_back(std::make_pair(x, y - i));

			// Enemy Collision
			if (collision[checkLoc] == enemy || collision[checkLoc] == enemy + 2)
				break;
		}
	}

	// Check RIGHT
	for (int i = 1; i < 8 - x; i++) {
		int checkLoc = curPos + i;

		// Check ally Collision
		if (collision[checkLoc] == ally || collision[checkLoc] == ally + 2)
			break;

		else {
			// Valid movement
			moveLoc.push_back(std::make_pair(x + i, y));

			// Enemy Collision
			if (collision[checkLoc] == enemy || collision[checkLoc] == enemy + 2)
				break;
		}
	}

	// Check DOWN
	for (int i = 1; i < 8 - y; i++) {
		int checkLoc = curPos + (8 * i);

		// Check Collision
		if (collision[checkLoc] == ally || collision[checkLoc] == ally + 2)
			break;

		else {
			// Valid movement
			moveLoc.push_back(std::make_pair(x, y + i));

			// Enemy Collision
			if (collision[checkLoc] == enemy || collision[checkLoc] == enemy + 2)
				break;
		}
	}

	// Check LEFT
	for (int i = 1; i <= x; i++) {
		int checkLoc = curPos - i;

		// Check Collision
		if (collision[checkLoc] == ally || collision[checkLoc] == ally + 2)
			break;

		else {
			// Valid Movement
			moveLoc.push_back(std::make_pair(x - i, y));

			// Enemy Collision
			if (collision[checkLoc] == enemy || collision[checkLoc] == enemy + 2)
				break;
		}
	}

	return moveLoc;
}

void Rook::updateZones(int curPlayer, int* collision, bool* oldZone) {
	// Quick position calc
	int pPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Used to calculate position to check
	int checkPos = 0;

	// Check UP
	for (int i = 1; i <= y; i++) {
		checkPos = pPos - (8 * i);

		// Check Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Check if Enemy
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Check DOWN
	for (int i = 1; i < 8 - y; i++) {
		checkPos = pPos + (8 * i);

		// Check Collision
		if (collision[checkPos] == ally)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Check if Enemy
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Check LEFT
	for (int i = 1; i <= x; i++) {
		checkPos = pPos - i;

		// Check Collision
		if (collision[checkPos] == ally)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Check if Enemy
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Check RIGHT
	for (int i = 1; i < 8 - x; i++) {
		checkPos = pPos + i;

		// Check Collision
		if (collision[checkPos] == ally)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Check if Enemy
			if (collision[checkPos] == enemy)
				break;
		}
	}
	

}



// ######################### KNIGHT ################################### //

// Inherits orignal constructor
// If additional setup is needed for this piece add in
Knight::Knight(int nx, int ny, int ntype) : cPiece(nx, ny, ntype) {};

std::vector<std::pair<int, int>> Knight::movement(int curPlayer, int* collision, bool* checkZones = nullptr) {
	// return movement vector
	std::vector<std::pair<int, int>> moveLoc;

	// Quick position calc
	int curPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// TOP
	if (y - 2 >= 0) {
		// Top Left (2 Up, 1 Left == - 16 - 1)
		if (x - 1 >= 0)
			if (collision[curPos - 16 - 1] != ally || collision[curPos - 16 - 1] != ally + 2)
				moveLoc.push_back(std::make_pair(x - 1, y - 2));
		// Top Right (2 Up, 1 Right == - 16 + 1)
		if (x + 1 < 8)
			if (collision[curPos - 16 + 1] != ally || collision[curPos - 16 - 1] != ally + 2)
				moveLoc.push_back(std::make_pair(x + 1, y - 2));
	}

	// RIGHT
	if (x + 2 < 8) {
		// Right Up (2 Right, 1 Up == + 2 - 8)
		if (y - 1 >= 0)
			if (collision[curPos + 2 - 8] != ally || collision[curPos - 16 - 1] != ally + 2)
				moveLoc.push_back(std::make_pair(x + 2, y - 1));
		// Right Down (2 Right, 1 Down == + 2 + 8)
		if (y + 1 < 8)
			if (collision[curPos + 2 + 8] != ally || collision[curPos - 16 - 1] != ally + 2)
				moveLoc.push_back(std::make_pair(x + 2, y + 1));
	}

	// DOWN
	if (y + 2 < 8) {
		// Bottom Left (2 Down, 1 Left == + 16 - 1)
		if (x - 1 >= 0)
			if (collision[curPos + 16 - 1] != ally || collision[curPos - 16 - 1] != ally + 2)
				moveLoc.push_back(std::make_pair(x - 1, y + 2));
		// Bottom Right (2 Down, 1 Right == + 16 + 1)
		if (x + 1 < 8)
			if (collision[curPos + 16 + 1] != ally || collision[curPos - 16 - 1] != ally + 2)
				moveLoc.push_back(std::make_pair(x + 1, y + 2));
	}

	// LEFT
	if (x - 2 >= 0) {
		// Left Up (2 Left, 1 Up == - 2 - 8)
		if (y - 1 >= 0)
			if (collision[curPos - 2 - 8] != ally || collision[curPos - 16 - 1] != ally + 2)
				moveLoc.push_back(std::make_pair(x - 2, y - 1));
		// Left Down (2 Left, 1 Down == - 2 + 8)
		if (y + 1 < 8)
			if (collision[curPos - 2 + 8] != ally || collision[curPos - 16 - 1] != ally + 2)
				moveLoc.push_back(std::make_pair(x - 2, y + 1));
	}

	return moveLoc;
}

void Knight::updateZones(int curPlayer, int* collision, bool* oldZone) {
	// Collision Location
	int pPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	//Top
	if (y - 2 >= 0) {
		// Top Left (2 Up, 1 Left == - 16 - 1)
		if (x - 1 >= 0)
			if (collision[pPos - 16 - 1] != ally || collision[pPos - 16 - 1] != ally + 2)
				if (!oldZone[pPos - 16 - 1])
					oldZone[pPos - 16 - 1] = true;
		// Top Right (2 Up, 1 Right == - 16 + 1)
		if (x + 1 < 8)
			if (collision[pPos - 16 + 1] != ally || collision[pPos - 16 + 1] != ally + 2)
				if (!oldZone[pPos - 16 + 1])
					oldZone[pPos - 16 + 1] = true;
	}
	// Right
	if (x + 2 < 8) {
		// Right Up (2 Right, 1 Up == + 2 - 8)
		if (y - 1 >= 0)
			if (collision[pPos + 2 - 8] != ally || collision[pPos + 2 - 8] != ally + 2)
				if (!oldZone[pPos + 2 - 8])
					oldZone[pPos + 2 - 8] = true;
		// Right Down (2 Right, 1 Down == + 2 + 8)
		if (y + 1 < 8)
			if (collision[pPos + 2 + 8] != ally || collision[pPos + 2 + 8] != ally + 2)
				if (!oldZone[pPos + 2 + 8])
					oldZone[pPos + 2 + 8] = true;
	}
	// Bot
	if (y + 2 < 8) {
		// Bottom Left (2 Down, 1 Left == + 16 - 1)
		if (x - 1 >= 0)
			if (collision[pPos + 16 - 1] != ally || collision[pPos + 16 - 1] != ally + 2)
				if (!oldZone[pPos + 16 - 1])
					oldZone[pPos + 16 - 1] = true;
		// Top Right (2 Down, 1 Right == + 16 + 1)
		if (x + 1 < 8)
			if (collision[pPos + 16 + 1] != ally || collision[pPos + 16 + 1] != ally + 2)
				if (!oldZone[pPos + 16 + 1])
					oldZone[pPos + 16 + 1] = true;
	}
	// Left
	if (x - 2 >= 0) {
		// Left Up (2 Left, 1 Up == - 2 - 8)
		if (y - 1 >= 0)
			if (collision[pPos - 2 - 8] != ally || collision[pPos - 2 - 8] != ally + 2)
				if (!oldZone[pPos - 2 - 8])
					oldZone[pPos - 2 - 8] = true;
		// Left Down (2 Left, 1 Down == - 2 + 8)
		if (y + 1 < 8)
			if (collision[pPos - 2 + 8] != ally || collision[pPos - 2 + 8] != ally + 2)
				if (!oldZone[pPos - 2 + 8])
					oldZone[pPos - 2 + 8] = true;
	}
}



// ######################### BISHOP ################################### //

// Inherits orignal constructor
// If additional setup is needed for this piece add in
Bishop::Bishop(int nx, int ny, int ntype) : cPiece(nx, ny, ntype) {};

std::vector<std::pair<int, int>> Bishop::movement(int curPlayer, int* collision, bool* checkZones = nullptr) {
	// return movement vector
	std::vector<std::pair<int, int>> moveLoc;

	// Quick position calc
	int curPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Temp hold new position value
	int checkPos = 0;
	// Top Left Diagonal
	for (int i = 1; i <= x && i <= y; i++) {
		checkPos = curPos - (i * (8 + 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Space is valid
			moveLoc.push_back(std::make_pair(x - i, y - i));
			// Enemy Collision
			if (collision[checkPos] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Top Right Diagonal
	for (int i = 1; i < 8 - x && i <= y; i++) {
		checkPos = curPos - (i * (8 - 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Space is valid
			moveLoc.push_back(std::make_pair(x + i, y - i));
			// Enemy Collision
			if (collision[checkPos] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Bottom Left Diagonal
	for (int i = 1; i <= x && i < 8 - y; i++) {
		checkPos = curPos + (i * (8 - 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Space is valid
			moveLoc.push_back(std::make_pair(x - i, y + i));
			// Enemy Collision
			if (collision[checkPos] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Bottom Right Diagonal
	for (int i = 1; i < 8 - x && i < 8 - y; i++) {
		checkPos = curPos + (i * (8 + 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Space is valid
			moveLoc.push_back(std::make_pair(x + i, y + i));
			// Enemy Collision
			if (collision[checkPos] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	return moveLoc;
}

void Bishop::updateZones(int curPlayer, int* collision, bool* oldZone) {
	// Collision Location
	int pPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Used to calculate position to check
	int checkPos = 0;

	// Top Left Diagonal
	for (int i = 1; i <= x && i <= y; i++) {
		checkPos = pPos - (i * (8 + 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Enemy Collision
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Top Right Diagonal
	for (int i = 1; i < 8 - x && i <= y; i++) {
		checkPos = pPos - (i * (8 - 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Enemy Collision
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Bottom Left Diagonal
	for (int i = 1; i <= x && i < 8 - y; i++) {
		checkPos = pPos + (i * (8 - 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Enemy Collision
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Bottom Right Diagonal
	for (int i = 1; i < 8 - x && i < 8 - y; i++) {
		checkPos = pPos + (i * (8 + 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Enemy Collision
			if (collision[checkPos] == enemy)
				break;
		}
	}
}



// ######################### QUEEN ################################### //

// Inherits orignal constructor
// If additional setup is needed for this piece add in
Queen::Queen(int nx, int ny, int ntype) : cPiece(nx, ny, ntype) {};

std::vector<std::pair<int, int>> Queen::movement(int curPlayer, int* collision, bool* checkZones = nullptr) {
	// return movement vector
	std::vector<std::pair<int, int>> moveLoc;

	// Collision Location
	int curPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Temp hold new position value
	int checkPos = 0;

	// Check UP
	for (int i = 1; i <= y; i++) {
		int checkLoc = curPos - (8 * i);

		// Check Collision
		if (collision[checkLoc] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Valid movement
			moveLoc.push_back(std::make_pair(x, y - i));

			// Enemy Collision
			if (collision[checkLoc] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Top Right Diagonal
	for (int i = 1; i < 8 - x && i <= y; i++) {
		checkPos = curPos - (i * (8 - 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Space is valid
			moveLoc.push_back(std::make_pair(x + i, y - i));
			// Enemy Collision
			if (collision[checkPos] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Check RIGHT
	for (int i = 1; i < 8 - x; i++) {
		int checkLoc = curPos + i;

		// Check ally Collision
		if (collision[checkLoc] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Valid movement
			moveLoc.push_back(std::make_pair(x + i, y));

			// Enemy Collision
			if (collision[checkLoc] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Bottom Right Diagonal
	for (int i = 1; i < 8 - x && i < 8 - y; i++) {
		checkPos = curPos + (i * (8 + 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Space is valid
			moveLoc.push_back(std::make_pair(x + i, y + i));
			// Enemy Collision
			if (collision[checkPos] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Check DOWN
	for (int i = 1; i < 8 - y; i++) {
		int checkLoc = curPos + (8 * i);

		// Check Collision
		if (collision[checkLoc] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Valid movement
			moveLoc.push_back(std::make_pair(x, y + i));

			// Enemy Collision
			if (collision[checkLoc] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Bottom Left Diagonal
	for (int i = 1; i <= x && i < 8 - y; i++) {
		checkPos = curPos + (i * (8 - 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Space is valid
			moveLoc.push_back(std::make_pair(x - i, y + i));
			// Enemy Collision
			if (collision[checkPos] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Check LEFT
	for (int i = 1; i <= x; i++) {
		int checkLoc = curPos - i;

		// Check Collision
		if (collision[checkLoc] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Valid Movement
			moveLoc.push_back(std::make_pair(x - i, y));

			// Enemy Collision
			if (collision[checkLoc] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	// Top Left Diagonal
	for (int i = 1; i <= x && i <= y; i++) {
		checkPos = curPos - (i * (8 + 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Space is valid
			moveLoc.push_back(std::make_pair(x - i, y - i));
			// Enemy Collision
			if (collision[checkPos] == enemy || collision[checkPos] == enemy + 2)
				break;
		}
	}

	return moveLoc;
}

void Queen::updateZones(int curPlayer, int* collision, bool* oldZone) {
	// Collision Location
	int pPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Used to calculate position to check
	int checkPos = 0;

	// Check UP
	for (int i = 1; i <= y; i++) {
		checkPos = pPos - (8 * i);

		// Check Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Check if Enemy
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Top Right Diagonal
	for (int i = 1; i < 8 - x && i <= y; i++) {
		checkPos = pPos - (i * (8 - 1));

		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Enemy Collision
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Check RIGHT
	for (int i = 1; i < 8 - x; i++) {
		checkPos = pPos + i;

		// Check Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Check if Enemy
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Bottom Right Diagonal
	for (int i = 1; i < 8 - x && i < 8 - y; i++) {
		checkPos = pPos + (i * (8 + 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Enemy Collision
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Check DOWN
	for (int i = 1; i < 8 - y; i++) {
		checkPos = pPos + (8 * i);

		// Check Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Check if Enemy
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Bottom Left Diagonal
	for (int i = 1; i <= x && i < 8 - y; i++) {
		checkPos = pPos + (i * (8 - 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Enemy Collision
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Check LEFT
	for (int i = 1; i <= x; i++) {
		checkPos = pPos - i;

		// Check Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Check if Enemy
			if (collision[checkPos] == enemy)
				break;
		}
	}

	// Top Left Diagonal
	for (int i = 1; i <= x && i <= y; i++) {
		checkPos = pPos - (i * (8 + 1));
		//Check for Ally Collision
		if (collision[checkPos] == ally || collision[checkPos] == ally + 2)
			break;

		else {
			// Check if already true
			if (!oldZone[checkPos])
				oldZone[checkPos] = true;
			// Enemy Collision
			if (collision[checkPos] == enemy)
				break;
		}
	}
}



// ######################### KING ################################### //
// Call base constructor to initialize first varibles
// Additional Setup afterwards
King::King(int nx, int ny, int ntype, bool fm, bool cL, bool cR) : cPiece(nx, ny, ntype) {
	firstMove = fm;
	castleL = cL;
	castleR = cR;
}

std::vector<std::pair<int, int>> King::movement(int curPlayer, int* collision, bool* checkZones) {
	// return movement vector
	std::vector<std::pair<int, int>> moveLoc;

	// Collision Location
	int pPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Used to calculate position to check
	int checkPos = 0;

	// Check Up
	checkPos = pPos - 8;
	if (y - 1 >= 0)
		if (collision[checkPos] != ally && !checkZones[checkPos])
			moveLoc.push_back(std::make_pair(x, y - 1));

	// Check Up Right
	checkPos = checkPos + 1;
	if (x + 1 < 8 && y - 1 >= 0)
		if (collision[checkPos] != ally && !checkZones[checkPos])
			moveLoc.push_back(std::make_pair(x + 1, y - 1));

	// Check Right
	checkPos = checkPos + 8;
	if (x + 1 < 8)
		if (collision[checkPos] != ally && !checkZones[checkPos])
			moveLoc.push_back(std::make_pair(x + 1, y));

	// Check Down Right
	checkPos = checkPos + 8;
	if (x + 1 < 8 && y + 1 < 8)
		if (collision[checkPos] != ally && !checkZones[checkPos])
			moveLoc.push_back(std::make_pair(x + 1, y + 1));

	// Check Down
	checkPos = checkPos - 1;
	if (y + 1 < 8)
		if (collision[checkPos] != ally && !checkZones[checkPos])
			moveLoc.push_back(std::make_pair(x, y + 1));

	// Check Down Left
	checkPos = checkPos - 1;
	if (x - 1 >= 0 && y + 1 < 8)
		if (collision[checkPos] != ally && !checkZones[checkPos])
			moveLoc.push_back(std::make_pair(x - 1, y + 1));

	// Check Left
	checkPos = checkPos - 8;
	if (x - 1 >= 0)
		if (collision[checkPos] != ally && !checkZones[checkPos])
			moveLoc.push_back(std::make_pair(x - 1, y));

	// Check Up Left
	checkPos = checkPos - 8;
	if (x - 1 >= 0 && y - 1 >= 0)
		if (collision[checkPos] != ally && !checkZones[checkPos])
			moveLoc.push_back(std::make_pair(x - 1, y - 1));


	// Castling Movement
	if (firstMove) {
		if (castleL) {
			
		}

		if (castleR) {
			
		}
	}

	return moveLoc;
}

void King::updateZones(int curPlayer, int* collision, bool* oldZone) {
	// Collision Location
	int pPos = y * 8 + x;
	int enemy = curPlayer == 0 ? 2 : 1;
	int ally = curPlayer + 1;

	// Used to calculate position to check
	int checkPos = 0;

	// Check Up
	checkPos = pPos - 8;
	if (y - 1 >= 0)
		if (collision[checkPos] != ally && !oldZone[checkPos])
			oldZone[checkPos] = true;

	// Check Up Right
	checkPos = checkPos + 1;
	if (x + 1 < 8 && y - 1 >= 0)
		if (collision[checkPos] != ally && !oldZone[checkPos])
			oldZone[checkPos] = true;

	// Check Right
	checkPos = checkPos + 8;
	if (x + 1 < 8)
		if (collision[checkPos] != ally && !oldZone[checkPos])
			oldZone[checkPos] = true;

	// Check Down Right
	checkPos = checkPos + 8;
	if (x + 1 < 8 && y + 1 < 8)
		if (collision[checkPos] != ally && !oldZone[checkPos])
			oldZone[checkPos] = true;

	// Check Down
	checkPos = checkPos - 1;
	if (y + 1 < 8)
		if (collision[checkPos] != ally && !oldZone[checkPos])
			oldZone[checkPos] = true;

	// Check Down Left
	checkPos = checkPos - 1;
	if (x - 1 >= 0 && y + 1 < 8)
		if (collision[checkPos] != ally && !oldZone[checkPos])
			oldZone[checkPos] = true;

	// Check Left
	checkPos = checkPos - 8;
	if (x - 1 >= 0)
		if (collision[checkPos] != ally && !oldZone[checkPos])
			oldZone[checkPos] = true;

	// Check Up Left
	checkPos = checkPos - 8;
	if (x - 1 >= 0 && y - 1 >= 0)
		if (collision[checkPos] != ally && !oldZone[checkPos])
			oldZone[checkPos] = true;
}
