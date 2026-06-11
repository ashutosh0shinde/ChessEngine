#include <SFML/Graphics.hpp>
#include <iostream>
#include<chrono>

using namespace std;
using namespace sf;


//DO THESE
//Write different function for make and undo move which purely do moves without checking legal moves or changing anything for engine

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

bool draw = false;
bool blackWon = false;
bool whiteWon = false;

int whiteTotalLegalMoves = 0;
int blackTotalLegalMoves = 0;

float evalMax = 2500;

float bishopPairAdv = 40;

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
bool hasWhiteKingMoved;
bool hasBlackKingMoved;

bool hasWhiteKingRookMoved;
bool hasWhiteQueenRookMoved;

bool hasBlackKingRookMoved;
bool hasBlackQueenRookMoved;
#pragma endregion

#pragma region Moves

struct Move
{
    Vector2i from;
    Vector2i to;

    int movedPiece;
    int capturedPiece;

    bool isCastle = false;
    Vector2i rookSquare;
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
void DrawWindow(auto& window, auto piecesTexture, bool clearSelected);

int PieceColor(Vector2i pos);
Vector2i FindKing(bool isWhite);
bool IsKingInCheck(bool isWhite);
int WhichPiece(Vector2i pos);
bool HasLegalMoves(bool isWhite);
bool IsCheckmate(bool isWhite);
bool IsStalemate(bool isWhite);

bool MakeMove(Vector2i pieceFrom, Vector2i pieceTo);
void UndoMove();

void GenerateLegalMoves(Vector2i pos);
void GeneratePsuedoMoves(Vector2i pos);
void GenerateSlidingMoves(Vector2i pos, int st, int end, int steps = 8);
vector<Move> GenerateAllLegalMoves(bool isWhite);
vector<Move> GenerateAllPseudoMoves(bool isWhite);

void PrintLegalMoves();
void PrintPsuedoMoves();

//Engine
float Evaluate();
bool MakeMoveEngine(bool isWhite);
float Minimax(bool isWhite, int depth);

#pragma endregion

bool isWhiteTurn = true;
bool playAsWhite = false;

int nodes;

//Functions Start
#pragma region BOAED&MAIN

int main()
{
    srand(time(nullptr));

    bool isLeftPressed = false;
    bool isRightPressed = false;

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


        if (!playAsWhite && isWhiteTurn && !whiteWon && !blackWon && !draw)
        {
            if (MakeMoveEngine(isWhiteTurn))
                isWhiteTurn = !isWhiteTurn;
        }

        Vector2i pos = Mouse::getPosition(window);
        if (Mouse::isButtonPressed(Mouse::Button::Right))
        {
            if (!isRightPressed)
            {
                UndoMove();
                isRightPressed = true;
            }
        }
        else
        {
            isRightPressed = false;
        }
        if (Mouse::isButtonPressed(Mouse::Button::Left))
        {
            if (!isLeftPressed)
            {
                //select piece
                int x = (pos.y) / sqSize;
                int y = (pos.x - evalWidth) / sqSize;

                if (board[x][y] != EMPTY && selectedPiece.x == -1 && pos.x >= 30)
                {


                    if (isWhiteTurn == (PieceColor({ x,y }) == 1) && playAsWhite == isWhiteTurn)
                    {
                        selectedPiece.x = x;
                        selectedPiece.y = y;

                        selectedSquares[x][y] = true;

                        GenerateLegalMoves({ x,y });
                        //highlight Squares
                        for (auto mv : legalMoves)
                        {
                            selectedSquares[mv.to.x][mv.to.y] = true;
                        }
                    }
                }
                else if (pos.x >= 30 && selectedPiece.x != -1)
                {
                    if (PieceColor(selectedPiece) != PieceColor({ x,y }))
                    {
                        if (MakeMove(selectedPiece, { x,y }))
                        {
                            isWhiteTurn = !isWhiteTurn;

                            DrawWindow(window, piecesTexture, true);

                            auto start = chrono::high_resolution_clock::now();

                            if (MakeMoveEngine(isWhiteTurn))
                                isWhiteTurn = !isWhiteTurn;

                            auto end = chrono::high_resolution_clock::now();
                            double seconds = chrono::duration<double>(end - start).count();

                            cout << "Nodes: " << nodes << " NPS: " << nodes / seconds << " Seconds: " << seconds << endl;
                            nodes = 0;
                        }

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
            isLeftPressed = true;
        }
        else
        {
            isLeftPressed = false;
        }
        DrawWindow(window, piecesTexture, false);
    }
}
void DrawWindow(auto& window, auto piecesTexture, bool clearSelected)
{
    if (clearSelected)
    {
        for (int i = 0; i < 8;i++)
        {
            for (int j = 0;j < 8;j++)
            {
                selectedSquares[i][j] = false;
            }
        }
    }
    //EVAL BAR
    float eval = Evaluate();

    if (eval < 5000 && eval > -5000)
    {
        if (eval > evalMax)
            eval = evalMax - 20;
        else if (eval < -evalMax)
            eval = -evalMax + 20;
    }
    RectangleShape evalRectangle({ evalWidth, windowHeight / 2.f - (eval / evalMax) * (windowHeight / 2.f) });
    evalRectangle.setFillColor(Color(30, 30, 30));
    //cout << eval/100 << endl;

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
#pragma endregion

#pragma region Moves
bool MakeMove(Vector2i pieceFrom, Vector2i pieceTo)
{
    checkSquare = { -1,-1 };

    if (PieceColor(pieceFrom) == 0 || PieceColor(pieceFrom) == PieceColor(pieceTo))
        return false;

    GenerateLegalMoves(pieceFrom);

    Move prev;
    for (auto& mv : legalMoves)
    {
        if (mv.to == pieceTo)
        {

            if (mv.isCastle = true)
            {
                prev.isCastle = true;
                prev.movedPiece = board[pieceFrom.x][pieceFrom.y];
                prev.from = pieceFrom;
                prev.to = pieceTo;
                prev.capturedPiece = board[pieceTo.x][pieceTo.y];

                board[pieceTo.x][pieceTo.y] = board[pieceFrom.x][pieceFrom.y];
                board[pieceFrom.x][pieceFrom.y] = EMPTY;

                if (board[pieceTo.x][pieceTo.y] == WK)
                {
                    if (pieceTo.x == 7 && pieceTo.y == 6)
                    {
                        prev.rookSquare = { 7,5 };
                        board[7][5] = WR;
                        board[7][7] = EMPTY;
                    }
                    else if (pieceTo.x == 7 && pieceTo.y == 2)
                    {
                        prev.rookSquare = { 7,3 };
                        board[7][3] = WR;
                        board[7][0] = EMPTY;
                    }

                }
                else if (board[pieceTo.x][pieceTo.y] == BK)
                {
                    if (pieceTo.x == 0 && pieceTo.y == 6)
                    {
                        prev.rookSquare = { 0,5 };
                        board[0][5] = BR;
                        board[0][7] = EMPTY;
                    }
                    else if (pieceTo.x == 0 && pieceTo.y == 2)
                    {
                        prev.rookSquare = { 0,3 };
                        board[0][3] = BR;
                        board[0][0] = EMPTY;
                    }

                }
            }
            else
            {
                prev.movedPiece = board[pieceFrom.x][pieceFrom.y];
                prev.from = pieceFrom;
                prev.to = pieceTo;
                prev.capturedPiece = board[pieceTo.x][pieceTo.y];


                board[pieceTo.x][pieceTo.y] = board[pieceFrom.x][pieceFrom.y];
                board[pieceFrom.x][pieceFrom.y] = EMPTY;

                if (prev.movedPiece == 1 && pieceFrom.x == 1 && pieceTo.x == 0)
                {
                    board[pieceTo.x][pieceTo.y] = 5;
                }
                else if (prev.movedPiece == 7 && pieceFrom.x == 6 && pieceTo.x == 7)
                {
                    board[pieceTo.x][pieceTo.y] = 11;
                }
            }

            if (IsKingInCheck(PieceColor(pieceTo) != 1))
                checkSquare = FindKing(PieceColor(pieceTo) != 1);

            prevMoves.push_back(prev);


            if(pieceFrom.x == 7 && pieceFrom.y==4)
                hasWhiteKingMoved=true;
            if(pieceFrom.x == 0 && pieceFrom.y==4)
                hasBlackKingMoved=true;

            if (pieceFrom.x == 7 && pieceFrom.y == 7)
                hasWhiteKingRookMoved = true;
            if (pieceFrom.x == 7 && pieceFrom.y == 0)
                hasWhiteQueenRookMoved = true;
            if (pieceFrom.x == 0 && pieceFrom.y == 7)
                hasBlackKingRookMoved = true;
            if (pieceFrom.x == 0 && pieceFrom.y == 4)
                hasBlackQueenRookMoved = true;


            //check for mate or draw
            if (IsStalemate(PieceColor(pieceTo) != 1))
            {
                cout << "DRAW: STALEMATE";
                draw = true;
            }
            else if (IsCheckmate(PieceColor(pieceTo) != 1))
            {
                cout << "CHECKMATE";
                if (PieceColor(pieceTo) == 1)
                    whiteWon = true;
                else
                    blackWon = true;
            }

            return true;
        }
    }
    if (IsKingInCheck(PieceColor(pieceTo) != 1))
        checkSquare = FindKing(PieceColor(pieceTo) != 1);
    return false;
}
void UndoMove()
{
    if (prevMoves.size() < 1)
        return;
    Move prev = prevMoves.back();

    board[prev.from.x][prev.from.y] = prev.movedPiece;
    board[prev.to.x][prev.to.y] = prev.capturedPiece;

    if (prev.isCastle)
    {
        if (prev.rookSquare.x == 7 && prev.rookSquare.y == 5)
        {
            board[7][7] = board[prev.rookSquare.x][prev.rookSquare.y];
            board[prev.rookSquare.x][prev.rookSquare.y] = EMPTY;
            hasWhiteKingMoved = false;
        }
        else if (prev.rookSquare.x == 7 && prev.rookSquare.y == 3)
        {
            board[7][0] = board[prev.rookSquare.x][prev.rookSquare.y];
            board[prev.rookSquare.x][prev.rookSquare.y] = EMPTY;
            hasWhiteKingMoved = false;
        }
        else if (prev.rookSquare.x == 0 && prev.rookSquare.y == 5)
        {
            board[0][7] = board[prev.rookSquare.x][prev.rookSquare.y];
            board[prev.rookSquare.x][prev.rookSquare.y] = EMPTY;
            hasBlackKingMoved = false;
        }
        else if (prev.rookSquare.x == 0 && prev.rookSquare.y == 3)
        {
            board[0][0] = board[prev.rookSquare.x][prev.rookSquare.y];
            board[prev.rookSquare.x][prev.rookSquare.y] = EMPTY;
            hasBlackKingMoved = false;
        }
    }


    prevMoves.pop_back();

    if (IsKingInCheck(true))
        checkSquare = FindKing(true);
    else if (IsKingInCheck(false))
        checkSquare = FindKing(false);
    else
        checkSquare = { -1,-1 };
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
            if (PieceColor({ pos.x - 1,pos.y - 1 }) == 2 && pos.y > 0)
            {
                move.to.x = pos.x - 1;
                move.to.y = pos.y - 1;
                pseudoMoves.push_back(move);
            }


            if (PieceColor({ pos.x - 1,pos.y + 1 }) == 2 && pos.y < 7)
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

            if (PieceColor({ pos.x + 1,pos.y - 1 }) == 1 && pos.y > 0)
            {
                move.to.x = pos.x + 1;
                move.to.y = pos.y - 1;
                pseudoMoves.push_back(move);
            }

            if (PieceColor({ pos.x + 1,pos.y + 1 }) == 1 && pos.y < 7)
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
        if (isWhite)
        {
            if (!hasWhiteKingMoved && !hasWhiteKingRookMoved && board[7][4] == WK)
            {
                if (board[7][5] == EMPTY && board[7][6] == EMPTY)
                {
                    move.to.x = 7;
                    move.to.y = 6;
                    move.isCastle = true;
                    pseudoMoves.push_back(move);
                }
                if (board[7][1] == EMPTY && board[7][2] == EMPTY && board[7][3] == EMPTY)
                {
                    move.to.x = 7;
                    move.to.y = 2;
                    move.isCastle = true;
                    pseudoMoves.push_back(move);
                }
            }
        }
        else
        {
            if (!hasBlackKingMoved && !hasBlackKingRookMoved && board[0][4] == BK)
            {
                if (board[0][5] == EMPTY && board[0][6] == EMPTY)
                {
                    move.to.x = 0;
                    move.to.y = 6;
                    move.isCastle = true;
                    pseudoMoves.push_back(move);
                }
                if (board[0][1] == EMPTY && board[0][2] == EMPTY && board[0][3] == EMPTY)
                {
                    move.to.x = 0;
                    move.to.y = 2;
                    move.isCastle = true;
                    pseudoMoves.push_back(move);
                }
            }
        }
        break;
    }
}
vector<Move> GenerateAllPseudoMoves(bool isWhite)
{
    vector<Move> allMoves;
    for (int i = 0;i < 8;i++)
    {
        for (int j = 0;j < 8;j++)
        {
            if (PieceColor({ i,j }) != 0 && (PieceColor({ i,j }) == 1 && isWhite) || (PieceColor({ i,j }) == 2 && !isWhite))
            {
                GeneratePsuedoMoves({ i,j });
                for (auto& mv : pseudoMoves)
                {
                    allMoves.push_back(mv);
                }
            }
        }
    }
    return allMoves;
}
vector<Move> GenerateAllLegalMoves(bool isWhite)
{
    vector<Move> allMoves;
    for (int i = 0;i < 8;i++)
    {
        for (int j = 0;j < 8;j++)
        {
            if (PieceColor({ i,j }) != 0 && (PieceColor({ i,j }) == 1 && isWhite) || (PieceColor({ i,j }) == 2 && !isWhite))
            {
                GenerateLegalMoves({ i,j });
                for (auto& mv : legalMoves)
                {
                    allMoves.push_back(mv);
                }
            }
        }
    }
    return allMoves;
}
void GenerateLegalMoves(Vector2i pos)
{
    //GeneratePsuedoMoves(pos);
    //legalMoves = pseudoMoves;
    //return;
    legalMoves.clear();
    if (PieceColor(pos) == 0)
        return;

    bool isWhite = PieceColor(pos) == 1;
    GeneratePsuedoMoves(pos);
    vector<Move> pseudoMovesCopy = pseudoMoves;

    for (auto& mv : pseudoMovesCopy)
    {
        int movedPiece = board[mv.from.x][mv.from.y];
        int capturedPiece = board[mv.to.x][mv.to.y];

        board[mv.to.x][mv.to.y] = movedPiece;
        board[mv.from.x][mv.from.y] = EMPTY;

        if (!IsKingInCheck(isWhite))
            legalMoves.push_back(mv);

        board[mv.to.x][mv.to.y] = capturedPiece;
        board[mv.from.x][mv.from.y] = movedPiece;
    }
}
#pragma endregion


#pragma region PieceProperties
bool HasLegalMoves(bool isWhite)
{
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (PieceColor({ x, y }) != (isWhite ? 1 : 2))
                continue;

            GenerateLegalMoves({ x, y });

            if (!legalMoves.empty())
                return true;
        }
    }

    return false;
}
bool IsCheckmate(bool isWhite)
{
    return !HasLegalMoves(isWhite) && IsKingInCheck(isWhite);
}
bool IsStalemate(bool isWhite)
{
    return !HasLegalMoves(isWhite) && !IsKingInCheck(isWhite);
}
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
                int pieceType = WhichPiece({ x,y });

                //check pawn attacks
                if (pieceType == 1)
                {
                    bool isWhitePawn = PieceColor({ x,y }) == 1;

                    if (isWhitePawn)
                    {
                        if (kingPos.x == x - 1 && (kingPos.y == y + 1 || kingPos.y == y - 1))
                            return true;
                    }
                    else
                    {
                        if (kingPos.x == x + 1 && (kingPos.y == y + 1 || kingPos.y == y - 1))
                            return true;
                    }
                    continue;
                }

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
    else if (board[pos.x][pos.y] == 1 || board[pos.x][pos.y] == 7) //Pawn
        return 1;
    else if (board[pos.x][pos.y] == 3 || board[pos.x][pos.y] == 9)//Knight
        return 2;
    else if (board[pos.x][pos.y] == 4 || board[pos.x][pos.y] == 10)//Bishop
        return 3;
    else if (board[pos.x][pos.y] == 2 || board[pos.x][pos.y] == 8)//Rook
        return 4;
    else if (board[pos.x][pos.y] == 5 || board[pos.x][pos.y] == 11)//Queen
        return 5;
    else if (board[pos.x][pos.y] == 6 || board[pos.x][pos.y] == 12)//King
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
    for (auto& mv : pseudoMoves)
    {
        cout << mv.from.x << ":" << mv.from.y << "  " << mv.to.x << ":" << mv.to.y << endl;
    }
    cout << endl;
}
void PrintLegalMoves()
{
    for (auto& mv : legalMoves)
    {
        cout << mv.from.x << ":" << mv.from.y << "  " << mv.to.x << ":" << mv.to.y << endl;
    }
    cout << endl;
}

#pragma endregion

#pragma region ENGINE
bool MakeMoveEngine(bool isWhite)
{
    int bestScore = isWhite ? INT_MIN : INT_MAX;
    Move bestMove;
    vector<Move> moves = GenerateAllLegalMoves(isWhite);
    if (moves.empty())
        return false;

    for (auto& mv : moves)
    {
        int movedPiece = board[mv.from.x][mv.from.y];
        int capturedPiece = board[mv.to.x][mv.to.y];

        board[mv.to.x][mv.to.y] = movedPiece;
        board[mv.from.x][mv.from.y] = EMPTY;

        int score = Minimax(!isWhite, 2);

        board[mv.to.x][mv.to.y] = capturedPiece;
        board[mv.from.x][mv.from.y] = movedPiece;

        if (isWhite)
        {
            if (score > bestScore)
            {
                bestScore = score;
                bestMove = mv;
            }
        }
        else
        {
            if (score < bestScore)
            {
                bestScore = score;
                bestMove = mv;
            }
        }
    }

    return MakeMove(bestMove.from, bestMove.to);
}
float Minimax(bool isWhite, int depth)
{
    nodes++;
    if (depth == 0)
        return Evaluate();

    auto moves = GenerateAllLegalMoves(isWhite);

    if (moves.empty())
    {
        if (IsKingInCheck(isWhite))
            return isWhite ? -100000 : 100000;

        return 0; // stalemate
    }

    if (isWhite)
    {
        int bestScore = INT_MIN;

        for (auto& mv : moves)
        {
            int movedPiece = board[mv.from.x][mv.from.y];
            int capturedPiece = board[mv.to.x][mv.to.y];

            board[mv.to.x][mv.to.y] = movedPiece;
            board[mv.from.x][mv.from.y] = EMPTY;

            int score = Minimax(false, depth - 1);

            board[mv.to.x][mv.to.y] = capturedPiece;
            board[mv.from.x][mv.from.y] = movedPiece;

            bestScore = max(bestScore, score);
        }

        return bestScore;
    }
    else
    {
        int bestScore = INT_MAX;

        for (auto& mv : moves)
        {
            int movedPiece = board[mv.from.x][mv.from.y];
            int capturedPiece = board[mv.to.x][mv.to.y];

            board[mv.to.x][mv.to.y] = movedPiece;
            board[mv.from.x][mv.from.y] = EMPTY;

            int score = Minimax(true, depth - 1);

            board[mv.to.x][mv.to.y] = capturedPiece;
            board[mv.from.x][mv.from.y] = movedPiece;

            bestScore = min(bestScore, score);
        }
        return bestScore;
    }
}
float Evaluate()
{
    float eval = 0;

    int whiteBishopCount = 0;
    int blackBishopCount = 0;

    //check mated or drawn
    if (draw)
        return eval;
    else if (whiteWon)
        return 10000;
    else if (blackWon)
        return -10000;

    //evaluation on the basis of material
    for (int i = 0; i < 8;i++)
    {
        for (int j = 0;j < 8;j++)
        {
            switch (board[i][j])
            {
            case WP: eval += P; break;
            case WN: eval += N; break;
            case WB: eval += B; whiteBishopCount++; break;
            case WR: eval += R; break;
            case WQ: eval += Q; break;

            case BP: eval -= P; break;
            case BN: eval -= N; break;
            case BB: eval -= B; blackBishopCount++; break;
            case BR: eval -= R; break;
            case BQ: eval -= Q; break;
            }

            //Bishop Pair advantage
            if (whiteBishopCount == 2)
            {
                eval += bishopPairAdv;
                whiteBishopCount = INT_MIN;
            }
            if (blackBishopCount == 2)
            {
                eval -= bishopPairAdv;
                blackBishopCount = INT_MIN;
            }
        }
    }

    eval += GenerateAllPseudoMoves(true).size() / 2;
    eval -= GenerateAllPseudoMoves(false).size() / 2;

    return eval;
}
#pragma endregion