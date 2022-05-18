#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;
    bool isShipChar(char chr) const; // check if a character is a current ship character

  private:
    char m_board[MAXROWS][MAXCOLS];
    const Game& m_game;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    clear(); // initializes m_Board to array of '.'
    unblock(); // initializes m_blocked to array of false
}

void BoardImpl::clear()
{
    for(int i = 0; i < m_game.rows(); i++){
        for(int j = 0; j < m_game.cols(); j++){
            m_board[i][j] = '.';
        }
    }
    // This compiles, but may not be correct
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                m_board[r][c] = '#';
            }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if(m_board[r][c] == '#'){
                m_board[r][c] = '.';
            }
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if(shipId < 0 || shipId >= m_game.nShips()) return false; // check if shipId is valid
    if(!m_game.isValid(topOrLeft)) return false; // check if insert point is valid
    
    for (int r = 0; r < m_game.rows(); r++) { // check if shipId has been placed already
        for (int c = 0; c < m_game.cols(); c++)
        {
            if(m_board[r][c] == m_game.shipSymbol(shipId)) return false;
        }
    }
    
    switch (dir) {
        case 0: // horizontal case
            if(topOrLeft.c + m_game.shipLength(shipId) > m_game.cols()) return false; // check if ship fits horizontally
            
            for(int c = 0; c < m_game.shipLength(shipId); c++){
                if(m_board[topOrLeft.r][topOrLeft.c + c] != '.') return false; // check if position is blocked or already has ship
            }
            
            for(int c = 0; c < m_game.shipLength(shipId); c++){
                    m_board[topOrLeft.r][topOrLeft.c + c] = m_game.shipSymbol(shipId); // add ship to board
            }
            
            break;
        case 1: // vertical case
            if(topOrLeft.r + m_game.shipLength(shipId) > m_game.rows()) return false; // check if ship fits vertically
            
            for(int r = 0; r < m_game.shipLength(shipId); r++){
                if(m_board[topOrLeft.r + r][topOrLeft.c] != '.') return false; // check if position is blocked or already has ship
            }
            
            for(int r = 0; r < m_game.shipLength(shipId); r++){
                m_board[topOrLeft.r + r][topOrLeft.c] = m_game.shipSymbol(shipId); // add ship to board
            }
            
            break;
        default:
            assert(false); // catch in dir
            break;
    }
    
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if(shipId < 0 || shipId >= m_game.nShips()) return false; // check if shipId is valid
    
    switch (dir) {
        case 0: // horizontal case
            
            for(int c = topOrLeft.c; c < m_game.shipLength(shipId); c++){
                if(m_board[topOrLeft.r][c] == m_game.shipSymbol(shipId)){
                    m_board[topOrLeft.r][c] = '.';
                }
                else {
                    return false;
                }
            }
            break;
        case 1: // vertical case
            
            for(int r = topOrLeft.r; r < m_game.shipLength(shipId); r++){
                if(m_board[r][topOrLeft.c] == m_game.shipSymbol(shipId)){
                    m_board[r][topOrLeft.c] = '.';
                }
                else {
                    return false;
                }
            }
            break;
        default:
            assert(false); // catch in dir
            break;
    }
    
    return true; // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const
{
    cout << "  ";
    for(int c = 0; c < m_game.cols(); c++){ // print coloumn numbers
        cout << c;
    }
    cout << endl;

    for(int r = 0; r < m_game.rows(); r++){
        cout << r << " "; // print row numbers
        for(int c = 0; c < m_game.cols(); c++){
            if(shotsOnly && !(m_board[r][c] == '.' || m_board[r][c] == 'o' || m_board[r][c] == 'X')){
                cout << '.';
            }
            else {
                cout << m_board[r][c];
            }
        }
        cout << endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    // reset values
    shotHit = false;
    shipDestroyed = false;
    
    if(!m_game.isValid(p)) return false; // check if attack point is invalid
    
    if(m_board[p.r][p.c] == 'o' || m_board[p.r][p.c] == 'X') return false; // check if attack is called on previously attacked spot
    
    if(isShipChar(m_board[p.r][p.c])){
        
        shotHit = true;
        
        for (int i = 0; i < m_game.nShips(); i++) { // find which ship was attacked
            if(m_board[p.r][p.c] == m_game.shipSymbol(i)){
                shipId = i;
            }
        }
        
        m_board[p.r][p.c] = 'X';
        
        bool destroyed = true;
        
        for (int r = 0; r < m_game.rows(); r++){ // check if ship is destroyed
            for (int c = 0; c < m_game.cols(); c++){
                if (m_board[r][c] == m_game.shipSymbol(shipId))
                {
                    destroyed = false;
                    break;
                }
            }
            if(!destroyed) break;
        }
        
        shipDestroyed = destroyed;
        
    }
    
    else if(m_board[p.r][p.c] == '.'){
        m_board[p.r][p.c] = 'o';
    }
    
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    for (int r = 0; r < m_game.rows(); r++){
        for (int c = 0; c < m_game.cols(); c++){
            if (isShipChar(m_board[r][c]))
            {
                return false;
            }
        }
    }
    return true;
}

bool BoardImpl::isShipChar(char chr) const {
    for(int i = 0; i < m_game.nShips(); i++){
        if(chr == m_game.shipSymbol(i)){
            return true;
        }
    }
    return false;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
