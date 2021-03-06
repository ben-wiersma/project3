#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
  private:
    int m_rows;
    int m_cols;
    struct Ship
    {
        int m_len;
        char m_sym;
        string m_name;
    };
    vector<Ship> m_ships;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
  : m_rows(nRows), m_cols(nCols)
{
    // This compiles but may not be correct
}

int GameImpl::rows() const
{
    return m_rows;  // This compiles but may not be correct
}

int GameImpl::cols() const
{
    return m_cols;  // This compiles but may not be correct
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    Ship ss;
    ss.m_len = length;
    ss.m_sym = symbol;
    ss.m_name = name;
    m_ships.push_back(ss);
    return true;
}

int GameImpl::nShips() const
{
    return m_ships.size();  // This compiles but may not be correct
}

int GameImpl::shipLength(int shipId) const
{
    return m_ships.at(shipId).m_len;  // This compiles but may not be correct
}

char GameImpl::shipSymbol(int shipId) const
{
    return m_ships.at(shipId).m_sym;  // This compiles but may not be correct
}

string GameImpl::shipName(int shipId) const
{
    return m_ships.at(shipId).m_name;  // This compiles but may not be correct
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    
    if(!p1->placeShips(b1) || !p2->placeShips(b2)){
        cerr << "Error in placing ships" << endl;
        return nullptr;
    }
    
    Player* curPlyr = p1;
    Player* otherPlyr = p2;
    Board* curBord = &b2;
    Board* otherBord = &b1;
    
    while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed()) {
        
        bool hit = false;
        bool destroyed = false;
        int id = -1;
        

        
        cout << curPlyr->name() << "'s turn. Board for " << otherPlyr->name() << ":" << endl;
        curBord->display(curPlyr->isHuman());
        
        Point pointAttack = curPlyr->recommendAttack();
        bool ValidAttack = curBord->attack(pointAttack, hit, destroyed, id);
        
        curPlyr->recordAttackResult(pointAttack, ValidAttack, hit, destroyed, id);
        
        if(!ValidAttack){
            cout << curPlyr->name() << " wasted a shot at (" << pointAttack.r << "," << pointAttack.c << ")." << endl;
        }
        else {
            if(hit == true){
                if(destroyed){
                    cout << curPlyr->name() << " attacked at (" << pointAttack.r << "," << pointAttack.c << ") and destroyed the " << shipName(id) << ", resulting in:" << endl;
                }
                else {
                    cout << curPlyr->name() << " attacked at (" << pointAttack.r << "," << pointAttack.c << ") and hit something, resulting in:" << endl;
                }
            }
            else if(hit == false){
                cout << curPlyr->name() << " attacked at (" << pointAttack.r << "," << pointAttack.c << ") and missed, resulting in:" << endl;
            }
            curBord->display(curPlyr->isHuman());
        }
        
        
        if(shouldPause) waitForEnter();
        
        if(curPlyr == p1){
            curPlyr = p2;
            otherPlyr = p1;
            curBord = &b1;
            otherBord = &b2;
        }
        else {
            curPlyr = p1;
            otherPlyr = p2;
            curBord = &b2;
            otherBord = &b1;
        }
        
    }
    
    
    
    cout << otherPlyr->name() << " wins!" << endl;
    
    if(curPlyr->isHuman()) curBord->display(false);
    
    return otherPlyr;  // This compiles but may not be correct
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

