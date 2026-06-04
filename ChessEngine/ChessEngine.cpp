#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;
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
                int x = pos.x / sqSize;
                int y = pos.y / sqSize;

                if (board[y][x] != EMPTY && selectedPiece.x == -1)
                {
                    selectedPiece.x = x;
                    selectedPiece.y = y;

                    for (int i = 0; i < 8 * 8;i++)
                    {
                        *(*selectedSquares + i) = false;
                    }
                    selectedSquares[y][x] = true;
                }
                else
                {
                    if (selectedPiece.x != -1)
                    {
                        int x = pos.x / sqSize;
                        int y = pos.y / sqSize;
                        if (y != selectedPiece.y || x != selectedPiece.x)
                        {
                            board[y][x] = board[selectedPiece.y][selectedPiece.x];
                            board[selectedPiece.y][selectedPiece.x] = EMPTY;
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