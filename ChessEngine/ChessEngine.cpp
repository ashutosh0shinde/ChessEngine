#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;


#pragma region window

unsigned int windowWidth = 680;
unsigned int windowHeight = 650;
float sqSize = windowHeight / 8.f;
float evalWidth = 30.f;

#pragma endregion

#pragma region Coords

Vector2i knightCoords[8] =
{
    {-2,-1},
    {-2,1},
    {-1,2},
    {1,2},
    {2,-1},
    {2,1},
    {1,-2},
    {-1,-2}
};
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

#pragma endregion

#pragma region Eval

float evalLimit = 2500;

#pragma endregion

#pragma region Piece

enum Piece
{
    EMPTY = 0,
    WP = 1, WR = 2, WN = 3, WB = 4, WQ = 5, WK = 6,
    BP = 7, BR = 8, BN = 9, BB = 10, BQ = 11, BK = 12
};
enum PieceValue
{
    P = 100,
    N = 320,
    B = 330,
    R = 500,
    Q = 900
};

#pragma endregion

#pragma region Moves

struct Move
{
    Vector2i from;
    Vector2i to;

    int movedPiece;
};

vector<Move> prevMoves;
vector<Move> legalMoves;
vector <Move> pseudoMoves;

#pragma endregion

#pragma region Board_&_UI

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

bool selectedSquares[8][8];
Vector2i selectedPiece;
Vector2i checkSquare = { -1,-1 };

#pragma endregion

#pragma region Prototypes

int PieceColor(Vector2i pos);
Vector2i FindKing(bool isWhite);
bool IsKingInCheck(bool isWhite);
int WhichPiece(Vector2i pos);

bool MakeMove(Vector2i pieceFrom, Vector2i pieceTo);

void GenerateLegalMoves(Vector2i pos);
void GeneratePsuedoMoves(Vector2i pos);
void GenerateSlidingMoves(Vector2i pos, int st, int end, int steps = 8);

void PrintLegalMoves();
void PrintPsuedoMoves();

//Engine
float Evaluate();

#pragma endregion

bool isWhiteTurn = true;


//Functions Start
#pragma region BOAED&MAIN

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
        window.clear(Color(250, 250, 250));


        Vector2i pos = Mouse::getPosition(window);
        if (Mouse::isButtonPressed(Mouse::Button::Left))
        {
            if (!isPressed)
            {
                //select piece
                int x = (pos.y) / sqSize;
                int y = (pos.x - evalWidth) / sqSize;

                if (board[x][y] != EMPTY && selectedPiece.x == -1 && pos.x >= 30)
                {
                    selectedPiece.x = x;
                    selectedPiece.y = y;

                    selectedSquares[x][y] = true;

                    GenerateLegalMoves({x,y});
                    //highlight Squares
                    for (auto mv : legalMoves)
                    {
                        selectedSquares[mv.to.x][mv.to.y] = true;
                    } 
                }
                else if (pos.x >= 30 && selectedPiece.x != -1)
                {
                    if (PieceColor(selectedPiece) != PieceColor({x,y}))
                    {
                        MakeMove(selectedPiece, {x,y});
                    }
                    selectedPiece.x = -1;
                    selectedPiece.y = -1;

                    for (int i = 0; i < 8;i++)
                    {
                        for (int j = 0;j < 8;j++)
                        {
                            selectedSquares[i][j] = false;
                        }
                    }
                }

            }
            isPressed = true;
        }
        else
        {
            isPressed = false;
        }


        //EVAL BAR
        RectangleShape evalRectangle({ evalWidth, windowHeight / 2.f - (Evaluate() / evalLimit) * (windowHeight / 2.f) });
        evalRectangle.setFillColor(Color(30, 30, 30));

        for (int row = 0; row < 8;row++) //Board Drawing
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

                    piece.setPosition({ col * sqSize + evalWidth, row * sqSize });
                }

                RectangleShape rect({ sqSize,sqSize });
                rect.setPosition({ (sqSize * col + evalWidth),(sqSize * row) });
                rect.setOutlineThickness(1);
                rect.setOutlineColor(Color(0, 0, 0));

                if ((row + col) % 2 == 0)
                {
                    rect.setFillColor(Color(245, 231, 185));
                }
                else
                {
                    rect.setFillColor(Color(141, 96, 37));
                }

                if (checkSquare.x == row && checkSquare.y == col)
                {
                    rect.setFillColor(Color(255, 154, 129));
                }

                if (selectedSquares[row][col] == true)
                {
                    rect.setFillColor(Color(167, 240, 189));
                }

                window.draw(rect);
                window.draw(piece);
            }
        }
        window.draw(evalRectangle);
        window.display();
    }
}
#pragma endregion

#pragma region Moves
bool MakeMove(Vector2i pieceFrom, Vector2i pieceTo)
{
    if (PieceColor(pieceFrom) == 0 || PieceColor(pieceFrom) == PieceColor(pieceTo))
        return false;

    GenerateLegalMoves(pieceFrom);

    Move prev;
    for (auto mv : legalMoves)
    {
        if (mv.to == pieceTo)
        {
            prev.movedPiece = board[pieceFrom.x][pieceFrom.y];
            prev.from = pieceFrom;
            prev.to = pieceTo;

            board[pieceTo.x][pieceTo.y] = board[pieceFrom.x][pieceFrom.y];
            board[pieceFrom.x][pieceFrom.y] = 0;
        }
    }
    return true;
}
#pragma endregion

#pragma region MoveGeneration
void GenerateSlidingMoves(Vector2i pos, int st, int end, int steps)
{
    Move move;
    move.from = pos;

    Vector2i newPos;
    for (int i = st; i < end;i++)
    {
        newPos = pos;
        for (int j = 0; j < steps;j++)
        {
            newPos.x += directions[i].x;
            newPos.y += directions[i].y;

            if (newPos.x < 0 || newPos.x > 7 || newPos.y < 0 || newPos.y > 7 || PieceColor(pos) == PieceColor(newPos))
                break;

            move.to = newPos;
            pseudoMoves.push_back(move);

            if (PieceColor(newPos) != 0 && PieceColor(newPos) != PieceColor(pos))
                break;
        }
    }
}
void GeneratePsuedoMoves(Vector2i pos)
{
    pseudoMoves.clear();

    int pieceType = WhichPiece(pos);
    if (PieceColor(pos) == 0)
        return;
    bool isWhite = PieceColor(pos) == 1 ? true : false;

    Move move;
    move.from = pos;

    switch (pieceType)
    {
    case 1: //pawn
        if (isWhite)
        {
            if (board[pos.x - 1][pos.y] == EMPTY)
            {
                
                move.to.x = pos.x - 1;
                move.to.y = pos.y;
                pseudoMoves.push_back(move);

                if (pos.x == 6 && board[pos.x - 2][pos.y] == EMPTY)
                {
                    move.to.x = pos.x - 2;
                    move.to.y = pos.y;
                    pseudoMoves.push_back(move);
                }
            }
            if (PieceColor({ pos.x - 1,pos.y - 1 }) == 2)
            {
                move.to.x = pos.x - 1;
                move.to.y = pos.y - 1;
                pseudoMoves.push_back(move);
            }
                

            if (PieceColor({ pos.x - 1,pos.y + 1 }) == 2)
            {
                move.to.x = pos.x - 1;
                move.to.y = pos.y + 1;
                pseudoMoves.push_back(move);
            }
        }
        else
        {
            if (board[pos.x + 1][pos.y] == EMPTY)
            {
                move.to.x = pos.x + 1;
                move.to.y = pos.y;
                pseudoMoves.push_back(move);

                if (pos.x == 1 && board[pos.x + 2][pos.y] == EMPTY)
                {
                    move.to.x = pos.x + 2;
                    move.to.y = pos.y;
                    pseudoMoves.push_back(move);
                }
            }

            if (PieceColor({ pos.x + 1,pos.y - 1 }) == 1)
            {
                move.to.x = pos.x + 1;
                move.to.y = pos.y - 1;
                pseudoMoves.push_back(move);
            }

            if (PieceColor({ pos.x + 1,pos.y + 1 }) == 1)
            {
                move.to.x = pos.x + 1;
                move.to.y = pos.y + 1;
                pseudoMoves.push_back(move);
            }
        }
        break;
    case 2: //knight

        for (int i = 0;i < 8;i++)
        {
            int x = pos.x + knightCoords[i].x;
            int y = pos.y + knightCoords[i].y;
            if (x < 0 || y < 0 || x > 7 || y > 7)
                continue;
            if (PieceColor({ x,y }) != PieceColor(pos))
            {
                move.to.x = x;
                move.to.y = y;
                pseudoMoves.push_back(move);
            }
        }

        break;
    case 3: //Bishop
        GenerateSlidingMoves(pos, 4, 8);
        break;
    case 4: //Rook
        GenerateSlidingMoves(pos, 0, 4);
        break;
    case 5: //Queen
        GenerateSlidingMoves(pos, 0, 8);
        break;
    case 6: //King
        GenerateSlidingMoves(pos, 0, 8, 1);
        break;
    }
}
void GenerateLegalMoves(Vector2i pos)
{
    GeneratePsuedoMoves(pos);
    legalMoves = pseudoMoves; //temp
}
#pragma endregion


#pragma region PieceProperties
Vector2i FindKing(bool isWhite)
{
    int kingId = isWhite ? 6 : 12;
    for (int i = 0;i < 8;i++)
    {
        for (int j = 0;j < 8;j++)
        {
            if (board[i][j] == kingId)
            {
                //cout << i << " " << j << endl;
                return { i,j };
            }
        }
    }
    return { -1,-1 };
}
bool IsKingInCheck(bool isWhite)
{
    Vector2i kingPos = FindKing(isWhite);
    if (kingPos.x < 0)
        return false;

    int kingColor = PieceColor(kingPos);
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (PieceColor({ x,y }) != 0 && PieceColor({ x,y }) != kingColor)
            {
                GeneratePsuedoMoves({ x,y });

                for (auto& mv : pseudoMoves)
                {
                    if (mv.to == kingPos)
                        return true;
                }
            }
        }
    }
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
int PieceColor(Vector2i pos)
{
    int piece = board[pos.x][pos.y];

    if (piece == EMPTY)
        return 0;

    if (piece <= 6)
        return 1; // white

    return 2; // black
}
#pragma endregion

#pragma region Printing

void PrintPsuedoMoves()
{
    for (auto mv : pseudoMoves)
    {
        cout << mv.from.x << ":" << mv.from.y << "  " << mv.to.x << ":" << mv.to.y << endl;
    }
    cout << endl;
}
void PrintLegalMoves()
{
    for (auto mv : legalMoves)
    {
        cout << mv.from.x << ":" << mv.from.y << "  " << mv.to.x << ":" << mv.to.y << endl;
    }
    cout << endl;
}

#pragma endregion

#pragma region ENGINE
float Evaluate()
{
    float eval = 0;
    //evaluation on the basis of material
    for (int i = 0; i < 8;i++)
    {
        for (int j = 0;j < 8;j++)
        {
            switch (board[i][j])
            {
            case WP: eval += P; break;
            case WN: eval += N; break;
            case WB: eval += B; break;
            case WR: eval += R; break;
            case WQ: eval += Q; break;
                     
            case BP: eval -= P; break;
            case BN: eval -= N; break;
            case BB: eval -= B; break;
            case BR: eval -= R; break;
            case BQ: eval -= Q; break;
            }
        }
    }

    return eval;
}
#pragma endregion