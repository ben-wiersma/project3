#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

//typedef AwfulPlayer HumanPlayer;


class HumanPlayer : public Player
{
  public:
    HumanPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual bool isHuman() const;
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool HumanPlayer::placeShips(Board& b){
    
    cout << name() << " must place " << game().nShips() << " ships." << endl;
    b.display(false);
    
    for(int i = 0; i < game().nShips(); i++){
        
        Direction direction;
        char dir = '\0';
        while (!(dir == 'v' || dir == 'h')){
            cout << "Enter h or v for " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
            cin >> dir;
            cin.ignore(10000, '\n');
            
            switch (dir) {
                case 'v':
                    direction = VERTICAL;
                    break;
                case 'h':
                    direction = HORIZONTAL;
                    break;
                default:
                    cout << "Direction must be h or v." << endl;
                    break;
            }
        }
        
        
        int userR = -1;
        int userC = -1;
        while (!b.placeShip(Point(userR,userC), i, direction)) {
            if (dir == 'h') {
                cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
            }
            else {
                cout << "Enter row and column of topmost cell (e.g., 3 5): ";
            }
            
            while(!getLineWithTwoIntegers(userR, userC)){
                cout << "You must enter two integers." << endl;
                if (dir == 'h') {
                    cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
                }
                else {
                    cout << "Enter row and column of topmost cell (e.g., 3 5): ";
                }
            }
            
            if (b.placeShip(Point(userR,userC), i, direction)) break;
            else cout << "The ship can not be placed there." << endl;
            
        }
        
        b.display(false);
        
    }
    
    return true;
}

Point HumanPlayer::recommendAttack(){
    int userR = -1;
    int userC = -1;
    cout << "Enter the row and column to attack (e.g, 3 5): ";
    while(!getLineWithTwoIntegers(userR, userC)){
        cout << "You must enter two integers." << endl;
        cout << "Enter the row and column to attack (e.g, 3 5): ";
    }
    
    return Point(userR, userC);
}

bool HumanPlayer::isHuman() const { return true; }

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
}
void HumanPlayer::recordAttackByOpponent(Point p){
}



//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer MediocrePlayer;


class MediocrePlayer : public Player
{
  public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    bool placeRecursive(int ShipN, Board &b);
    int state;
    bool m_attacked[MAXROWS][MAXCOLS];
    Point stateChangePoint;
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
 : Player(nm, g), state(1), stateChangePoint()
{
    for (int i = 0; i < MAXROWS; i++) {
        for (int j = 0; j < MAXCOLS; j++) {
            m_attacked[i][j] = false;
        }
    }
}

bool MediocrePlayer::placeRecursive(int ShipN, Board &b){
    
    if (ShipN == 0) {
        return true;
    }
    
    for(int r = 0; r < game().rows(); r++){
        for(int c = 0; c < game().cols(); c++){
            Point p(r, c);
            if(b.placeShip(p, game().nShips()-ShipN, VERTICAL)){
                if(placeRecursive((ShipN - 1), b)){
                    return true;
                }
                else {
                    b.unplaceShip(p, game().nShips()-ShipN, VERTICAL);
                }
            }
            else if(b.placeShip(p, game().nShips()-ShipN, HORIZONTAL)){
                if(placeRecursive((ShipN - 1), b)){
                    return true;
                }
                else {
                    b.unplaceShip(p, game().nShips()-ShipN, HORIZONTAL);
                }
            }
        }
    }
    
    
    return false;
}

bool MediocrePlayer::placeShips(Board &b){
    
    for(int i = 0; i < 50; i++){
        
        b.block();
        
        bool couldPlace = placeRecursive(game().nShips(), b);
        
        b.unblock();
        
        if(couldPlace) return true;
    }
    
    return false;
}

Point MediocrePlayer::recommendAttack() {
    
    Point p;
    
    
    if(state == 1){
        p.r = randInt(game().rows());
        p.c = randInt(game().cols());
        
        while (m_attacked[p.r][p.c]) {
            p.r = randInt(game().rows());
            p.c = randInt(game().cols());
        }
        return p;
    }
    
    
    
    if(state == 2){
        int distance;
        int dir;
        int count = 0;
        p = stateChangePoint;
        while (!game().isValid(p) || m_attacked[p.r][p.c]) {
            p = stateChangePoint;
            if(count >= 50){
                state = 1;
                return recommendAttack();
            }
            count++;
            distance = randInt(4) + 1;
            dir = randInt(4);
            switch (dir) {
                case 0: // north
                    p.r -= distance;
                    break;
                case 1: // east
                    p.c += distance;
                    break;
                case 2: // south
                    p.r += distance;
                    break;
                case 3: // west
                    p.c -= distance;
                    break;
                default:
                    break;
            }
        }
        
        return p;
    }
    
    
    
    
    return p;
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    
    if(!validShot) return; // ignore if shot was not valid
    
    m_attacked[p.r][p.c] = true;
    
    if(state == 1){ // switch from state 1 to state 2
        if(shotHit && !shipDestroyed) {
            state = 2;
            stateChangePoint = p;
            return;
        }
    }
    
    if(state == 2){ // switch from state 2 to state 1
        if(shotHit && shipDestroyed) {
            state = 1;
            return;
        }
    }
    
}

void MediocrePlayer::recordAttackByOpponent(Point p){
    
}

// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.

typedef AwfulPlayer GoodPlayer;

/*
class GoodPlayer : public Player
{
  public:
    GoodPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
    int m_prob[MAXROWS][MAXCOLS];
};
*/
//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
