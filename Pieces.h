#pragma once

#include <vector>

// BASE CLASS
// All chess pieces will be inheriting from this class
class cPiece {
public:
	int x;
	int y;

	// Constructor
	cPiece(int nx, int ny);

	// Override functions
	virtual std::vector<std::pair<int, int>> movement(int curPlayer, int* collision, bool* checkZones) { return {std::make_pair(1, 1)}; };
	virtual void updateZones(int curPlayer, int* collision, bool* oldZone) {};
};

class Pawn : public cPiece {

	bool firstMove;
public:
	// Additional constructor setup
	Pawn(int nx, int ny, bool fm);

protected:
	std::vector<std::pair<int, int>> movement(int curPlayer, int* collision, bool* checkZones) override;
	void updateZones(int curPlayer, int* collision, bool* oldZone) override;
};

class Rook : public cPiece {
	bool firstMove;
public:
	// Additional constructor setup
	Rook(int nx, int ny, bool fm);

protected:
	std::vector<std::pair<int, int>> movement(int curPlayer, int* collision, bool* checkZones) override;
	void updateZones(int curPlayer, int* collision, bool* oldZone) override;
};

class Knight : public cPiece {
public:
	// Constructor
	Knight(int nx, int ny);

protected:
	std::vector<std::pair<int, int>> movement(int curPlayer, int* collision, bool* checkZones) override;
	void updateZones(int curPlayer, int* collision, bool* oldZone) override;
};

class Bishop : public cPiece {
public:
	// Constructor
	Bishop(int nx, int ny);

protected:
	std::vector<std::pair<int, int>> movement(int curPlayer, int* collision, bool* checkZones) override;
	void updateZones(int curPlayer, int* collision, bool* oldZone) override;
};

class Queen : public cPiece {
public:
	// Constructor
	Queen(int nx, int ny);

protected:
	std::vector<std::pair<int, int>> movement(int curPlayer, int* collision, bool* checkZones) override;
	void updateZones(int curPlayer, int* collision, bool* oldZone) override;
};

class King : public cPiece {
	bool firstMove;
public:
	// Additional constructor setup
	King(int nx, int ny, bool fm);

protected:
	std::vector<std::pair<int, int>> movement(int curPlayer, int* collision, bool* checkZones) override;
	void updateZones(int curPlayer, int* collision, bool* oldZone) override;
};