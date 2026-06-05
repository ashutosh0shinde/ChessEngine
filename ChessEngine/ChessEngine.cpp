#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;


unsigned int windowWidth = 650;
unsigned int windowHeight = 650;
float sqSize = windowHeight / 8.f;

enum Piece
{
    EMPTY = 0,
    WP = 1, WR = 2, WN = 3, WB = 4, WQ = 5, WK = 6,
    BP = 7, BR = 8, BN = 9, BB = 10, BQ = 11, BK = 12
};

//default board
int board[8][8] =
{
    {8,9,10,11,12,10,9,8},
    {7,7,7,7,7,7,7,7},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1},
    {2,3,4,5,6,4,3,2}
};
bool generatedMoves[8][8];

Vector2i directions[8] =
{
    {0,-1}, //up
    {1,0},  //right
    {0,1},  //down
    {-1,0},  //left

    {-1,-1}, //up left
    {1,-1},  //up right
    {1,1},   //down right
    {-1,1},  //down left
};

bool selectedSquares[8][8];    
Vector2i selectedPiece;

int PieceColor(Vector2i pos);
void GenerateMoves(Vector2i pos);
bool IsEnemy(Vector2i target, Vector2i enemyTo);
int WhichPiece(Vector2i pos);
void SlidingMovesGenerate(Vector2i pos, bool isWhite, int st, int end, int steps);
bool MakeMove(Vector2i piecePos, Vector2i targetPos);
void PrintGeneratedMoves();

int main()
{



    bool isPressed = false;

    selectedPiece.x = -1;
    selectedPiece.y = -1;

    
    Texture piecesTexture[13];

    RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "Chess");

    piecesTexture[WP].loadFromFile("PiecesIMG/WP.png");
    piecesTexture[WN].loadFromFile("PiecesIMG/WN.png");
    piecesTexture[WB].loadFromFile("PiecesIMG/WB.png");
    piecesTexture[WR].loadFromFile("PiecesIMG/WR.png");
    piecesTexture[WQ].loadFromFile("PiecesIMG/WQ.png");
    piecesTexture[WK].loadFromFile("PiecesIMG/WK.png");

    piecesTexture[BP].loadFromFile("PiecesIMG/BP.png");
    piecesTexture[BN].loadFromFile("PiecesIMG/BN.png");
    piecesTexture[BB].loadFromFile("PiecesIMG/BB.png");
    piecesTexture[BR].loadFromFile("PiecesIMG/BR.png");
    piecesTexture[BQ].loadFromFile("PiecesIMG/BQ.png");
    piecesTexture[BK].loadFromFile("PiecesIMG/BK.png");

    for (int i = 1;i < 13;i++)
    {
        piecesTexture[i].setSmooth(true);
    }

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(Color(127, 127, 127));


        Vector2i pos = Mouse::getPosition(window);
        if (Mouse::isButtonPressed(Mouse::Button::Left))
        {
            if (!isPressed)
            {
                //select piece
                int x = pos.y / sqSize;
                int y = pos.x / sqSize;

                if (board[x][y] != EMPTY && selectedPiece.x == -1)
                {
                    selectedPiece.x = x;
                    selectedPiece.y = y;

                    for (int i = 0; i < 8 * 8;i++)
                    {
                        *(*selectedSquares + i) = false;
                    }
                    selectedSquares[x][y] = true;

                    GenerateMoves({ x,y });
                    for (int i = 0; i < 8;i++)
                    {
                        for (int j = 0;j < 8;j++)
                        {
                            if (generatedMoves[i][j])
                                selectedSquares[i][j] = true;
                        }
                    }
                }
                else
                {
                    if (selectedPiece.x != -1)
                    {
                        int x = pos.y / sqSize;
                        int y = pos.x / sqSize;
                        MakeMove(selectedPiece, { x,y });
                    }
                }

            }
            isPressed = true;
        }
        else
        {
            //if(isPressed)
                //cout << "Released, " << pos.x << " " << pos.y << endl << endl;
            isPressed = false;
        }


        for (int row = 0; row < 8;row++)
        {
            for (int col = 0; col < 8; col++)
            {
                //draw pieces
                int pieceId = board[row][col];
                Sprite piece(piecesTexture[pieceId]);

                if (pieceId != EMPTY)
                {

                    Vector2u size = piecesTexture[pieceId].getSize();

                    piece.setScale({ sqSize / size.x,   sqSize / size.y });

                    piece.setPosition({ col * sqSize, row * sqSize });
                }

                RectangleShape rect({ sqSize,sqSize });
                rect.setPosition({ (sqSize * col),(sqSize * row) });
                rect.setOutlineThickness(1);
                rect.setOutlineColor(Color(0, 0, 0));

                if (selectedSquares[row][col] == true)
                {
                    rect.setFillColor(Color(167, 240, 189));
                }
                else
                    if ((row + col) % 2 == 0)
                    {
                        rect.setFillColor(Color(245, 231, 185));
                    }
                    else
                    {
                        rect.setFillColor(Color(141, 96, 37));
                    }

                window.draw(rect);
                window.draw(piece);
            }
        }
        window.display();
    }
}
bool MakeMove(Vector2i piecePos, Vector2i targetPos)
{
    int x = targetPos.x;
    int y = targetPos.y;
    if (generatedMoves[x][y])
    {
        if (y != selectedPiece.y || x != selectedPiece.x)
        {
            board[x][y] = board[selectedPiece.x][selectedPiece.y];
            board[selectedPiece.x][selectedPiece.y] = EMPTY;
        }
    }
    selectedPiece.x = -1;
    for (int i = 0; i < 8 * 8;i++)
    {
        *(*selectedSquares + i) = false;
    }
    return true;
}
void PrintGeneratedMoves()
{
    cout << endl;
    for (int i = 0;i < 8;i++)
    {
        for (int j = 0; j < 8;j++)
        {
            cout << generatedMoves[i][j] << " ";
        }
        cout << endl;
    }
}
int PieceColor(Vector2i pos) //0-empty 1-white 2-black
{
    if (board[pos.x][pos.y] == EMPTY)
        return 0;
    else if (board[pos.x][pos.y] <= 6)
        return 1;
    else if (board[pos.x][pos.y] > 6)
        return 2;
}
bool IsEnemy(Vector2i target, Vector2i enemyTo)
{
    if (PieceColor(target) != PieceColor(enemyTo))
        return true;
    return false;
}
int WhichPiece(Vector2i pos)
{
    if (board[pos.x][pos.y] == EMPTY)
        return 0;
    else if (board[pos.x][pos.y] == 1 || board[pos.x][pos.y] == 7)
        return 1;
    else if (board[pos.x][pos.y] == 3 || board[pos.x][pos.y] == 9)
        return 2;
    else if (board[pos.x][pos.y] == 4 || board[pos.x][pos.y] == 10)
        return 3;
    else if (board[pos.x][pos.y] == 2 || board[pos.x][pos.y] == 8)
        return 4;
    else if (board[pos.x][pos.y] == 5 || board[pos.x][pos.y] == 11)
        return 5;
    else if (board[pos.x][pos.y] == 6 || board[pos.x][pos.y] == 12)
        return 6;
}


Vector2i knightCoords[8] =
{
    {-1,-2}, {1,-2},
    {2,-1},  {2,1},
    {1,2},   {-1,2},
    {-2,1},  {-2,-1}
};
void SlidingMovesGenerate(Vector2i pos, bool isWhite, int st, int end, int steps = 8)
{
    Vector2i newPos;
    for (int i = st; i < end;i++)
    {
        newPos = pos;
        for (int j = 0; j < steps;j++)
        {
            newPos.x += directions[i].x;
            newPos.y += directions[i].y;

            if (newPos.x < 0 || newPos.x > 7 || newPos.y < 0 || newPos.y > 7 || !IsEnemy(newPos,pos))
                break;

            generatedMoves[newPos.x][newPos.y] = true;

            if (IsEnemy(newPos, pos) && WhichPiece(newPos) != EMPTY)
                break;
        }
    }
}
void GenerateMoves(Vector2i pos)
{
    for (int i = 0; i < 8;i++)
    {
        for (int j = 0;j < 8;j++)
        {
            generatedMoves[i][j] = false;
        }
    }

    if (PieceColor(pos) == EMPTY)
        return;

    bool isWhite = PieceColor(pos) == 1 ? true : false;
    int pieceType = WhichPiece(pos);

    switch (pieceType)
    {
    case 1: //pawn
        
        break;
    case 2: //knight

        for (int i = 0;i < 8;i++)
        {
            int x = pos.x + knightCoords[i].x;
            int y = pos.y + knightCoords[i].y;
            if (x < 0 || y < 0 || x > 7 || y > 7)
                continue;
            if (IsEnemy({x,y}, pos))
                generatedMoves[x][y] = true;
        }

        break;
    case 3: //Bishop
        SlidingMovesGenerate(pos, isWhite, 4, 8);
        break;
    case 4: //Rook
        SlidingMovesGenerate(pos, isWhite, 0, 4);
        break;
    case 5: //Queen
        SlidingMovesGenerate(pos, isWhite, 0, 8);
        break;
    case 6: //King
        SlidingMovesGenerate(pos, isWhite, 0, 8,1);
        break;
    default:
        break;
    }
}