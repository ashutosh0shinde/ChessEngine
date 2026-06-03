#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

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
Vector2i knightCoords[8] =
{
    {-1,-2}, {1,-2},
    {2,-1},  {2,1},
    {1,2},   {-1,2},
    {-2,1},  {-2,-1}
};
bool generatedMoves[8][8];
Vector2i enPassantSquare = { -1,-1 };
bool whiteTurn = true;

bool IsWhite(Vector2i pos);
bool IsLegal(Vector2i startPos, Vector2i endPos);
void GenerateMoves(Vector2i pos);
int WhichPiece(Vector2i pos);

int main()
{
    unsigned int windowWidth = 650;
    unsigned int windowHeight = 650;
    float sqSize = windowHeight / 8.f;

    bool selectedSquares[8][8];

    bool isPressed = false;
    

    Vector2i selectedPiece;
    selectedPiece.x = -1;
    selectedPiece.y = -1;

   
    Texture piecesTexture[13];

    RenderWindow window(sf::VideoMode({ windowWidth, windowHeight}),"Chess");

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
        window.clear(Color(127,127,127));


        Vector2i pos = Mouse::getPosition(window);
        if (Mouse::isButtonPressed(Mouse::Button::Left))
        {
            if (!isPressed)
            {
                //select piece
                int x = pos.x / sqSize;
                int y = pos.y / sqSize;

                if (board[y][x] != EMPTY && selectedPiece.x == -1)
                {
                    if ((whiteTurn && board[y][x] <= 6) || (!whiteTurn && board[y][x] > 6))
                    {
                        selectedPiece.x = x;
                        selectedPiece.y = y;

                        for (int i = 0; i < 8 * 8;i++)
                        {
                            *(*selectedSquares + i) = false;
                        }
                        selectedSquares[y][x] = true;
                    }  
                }
                else
                {
                    if (selectedPiece.x != -1)
                    {
                        int x = pos.x / sqSize;
                        int y = pos.y / sqSize;

                        if ((y != selectedPiece.y || x != selectedPiece.x) && (board[y][x] == EMPTY || ((whiteTurn && board[y][x] > 6) || (!whiteTurn && board[y][x] <= 6))))
                        {
                            if (IsLegal(selectedPiece, {x,y}))
                            {
                                if (WhichPiece(selectedPiece) == 1 && x == enPassantSquare.x && y == enPassantSquare.y)
                                {
                                    board[y][x] = board[selectedPiece.y][selectedPiece.x];
                                    board[selectedPiece.y][selectedPiece.x] = EMPTY;
                                    if(whiteTurn)
                                        board[y + 1][x] = EMPTY;
                                    else
                                        board[y - 1][x] = EMPTY;
                                }
                                else if (WhichPiece(selectedPiece) == 1 && (y == 0 || y == 7)) // promotion to queen only, replace with selection later
                                {
                                    board[y][x] = whiteTurn ? WQ : BQ;
                                    board[selectedPiece.y][selectedPiece.x] = EMPTY;
                                }
                                else
                                {
                                    board[y][x] = board[selectedPiece.y][selectedPiece.x];
                                    board[selectedPiece.y][selectedPiece.x] = EMPTY;
                                }
                                

                                if (WhichPiece({x,y}) == 1 && (selectedPiece.y - y == 2 || selectedPiece.y - y == -2))
                                    enPassantSquare = { x,whiteTurn ? y + 1 : y - 1 };
                                else
                                    enPassantSquare = { -1,-1 };

                                whiteTurn = !whiteTurn;
                            }
                        }
                        selectedPiece.x = -1;
                        for (int i = 0; i < 8 * 8;i++)
                        {
                            *(*selectedSquares + i) = false;
                        }
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

                    piece.setScale({sqSize / size.x,   sqSize / size.y });

                    piece.setPosition({ col * sqSize, row * sqSize });
                }

                RectangleShape rect({ sqSize,sqSize });
                rect.setPosition({(sqSize * col),(sqSize * row) });
                rect.setOutlineThickness(1);
                rect.setOutlineColor(Color(0, 0, 0));

                if (selectedSquares[row][col] == true)
                {
                    rect.setFillColor(Color(167, 240, 189));
                }
                else
                if ((row+col) % 2 == 0)
                {
                    rect.setFillColor(Color(245, 231, 185));
                }
                else
                {
                    rect.setFillColor(Color(176, 139, 107));
                }

                window.draw(rect);
                window.draw(piece);
            }
        }
        window.display();
    }
}
bool IsWhite(Vector2i pos)
{
    if (board[pos.y][pos.x] >= 6)
    {
        return false;
    }
    return true;
}
int WhichPiece(Vector2i pos) // 0-empty 1-pawn 2-knight 3-bishop 4-rook 5-queen 6-king
{
    if (board[pos.y][pos.x] == EMPTY)
        return 0;
    else if (board[pos.y][pos.x] == 1 || board[pos.y][pos.x] == 7)
        return 1;
    else if (board[pos.y][pos.x] == 3 || board[pos.y][pos.x] == 9)
        return 2;
    else if (board[pos.y][pos.x] == 4 || board[pos.y][pos.x] == 10)
        return 3;
    else if (board[pos.y][pos.x] == 2 || board[pos.y][pos.x] == 8)
        return 4;
    else if (board[pos.y][pos.x] == 5 || board[pos.y][pos.x] == 11)
        return 5;
    else if (board[pos.y][pos.x] == 6 || board[pos.y][pos.x] == 12)
        return 6;
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
    int pieceType = WhichPiece(pos);
    switch (pieceType)
    {
    case 1: //pawn
        if (IsWhite(pos))
        {
            //movement
            if (board[pos.y - 1][pos.x] == EMPTY)
                generatedMoves[pos.y - 1][pos.x] = true;
            if (board[pos.y - 2][pos.x] == EMPTY && board[pos.y - 1][pos.x] == EMPTY && pos.y == 6)
                generatedMoves[pos.y - 2][pos.x] = true;

            //diagonal capture
            if(enPassantSquare.x != -1)
                generatedMoves[enPassantSquare.y][enPassantSquare.x] = true;
            if (board[pos.y - 1][pos.x + 1] != EMPTY)
                generatedMoves[pos.y - 1][pos.x + 1] = true;
            if (board[pos.y - 1][pos.x - 1] != EMPTY)
                generatedMoves[pos.y - 1][pos.x - 1] = true;
        }
        else
        {
            //movement
            if (board[pos.y + 1][pos.x] == EMPTY)
                generatedMoves[pos.y + 1][pos.x] = true;
            if (board[pos.y + 2][pos.x] == EMPTY && board[pos.y + 1][pos.x] == EMPTY && pos.y == 1)
                generatedMoves[pos.y + 2][pos.x] = true;

            //diagonal capture
            if (enPassantSquare.x != -1)
                generatedMoves[enPassantSquare.y][enPassantSquare.x] = true;
            if (board[pos.y + 1][pos.x + 1] != EMPTY)
                generatedMoves[pos.y + 1][pos.x + 1] = true;
            if (board[pos.y + 1][pos.x - 1] != EMPTY)
                generatedMoves[pos.y + 1][pos.x - 1] = true;
        }
        break;
    case 2: //knight

        for (int i = 0;i < 8;i++)
        {
            int x = pos.x + knightCoords[i].x;
            int y = pos.y + knightCoords[i].y;
            if (x < 0 || y < 0 || x > 7 || y > 7)
                continue;
            if (board[y][x] == EMPTY || ((!whiteTurn && IsWhite({ x,y })) || (whiteTurn && !IsWhite({ x,y }))))
                generatedMoves[y][x] = true;
        }

        break;
    case 3: //Bishop

        break;
    case 4: //Rook

        break;
    case 5: //Queen

        break;
    case 6: //King

        break;
    default:
        break;
    }
}

bool IsLegal(Vector2i startPos, Vector2i endPos)
{
    GenerateMoves(startPos);
    if (generatedMoves[endPos.y][endPos.x] == true)
    {
        return true;
    }
    return false;
}
