#ifndef MAIN_H
#define MAIN_H
#include "sound.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>

class main
{
    public:
        main();
        virtual ~main();
    protected:
    private:
        //static void playMusic;

        enum GameState {playingMusic};

        static GameState _gameState;
};

#endif // MAIN_H
