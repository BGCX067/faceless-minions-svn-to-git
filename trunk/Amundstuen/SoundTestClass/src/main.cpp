#include "../include/main.hpp"
#include "../include/sound.hpp"

int main()
{
    std::cout << "-------------------" << std::endl;
    std::cout << "Entering sound test." << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "Press 'P' to pause" << std::endl;
    std::cout << "Press 'num-pad -' to lower volume." << std::endl;
    std::cout << "Press 'num-pad +' to increase volume." << std::endl;
    std::cout << "Press 'RETURN' to exit." << std::endl;
    std::cout << "-------------------" << std::endl;

    sound *sound_= new sound();

    //float sound_volume = sound_->get_sfx_volume();
    //float music_volume = sound_->get_bgm_volume();
    bool playing_bgm_;
    bool pause_pressed_ = false;


    sf::RenderWindow window(sf::VideoMode(800, 800, 32), "FUCK THIS SHIT!!");
    sf::Texture texture;
    texture.loadFromFile("sprites/gundam_wing_zero.gif");

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, 82, 105));
    sprite.setColor(sf::Color(255, 255, 255, 255));
    sprite.setPosition(370, 350);

    //circle 1
    sf::CircleShape shape(400.f);
    shape.setFillColor(sf::Color::Red);
    //circle 2
    sf::CircleShape circ(300.f);
    circ.setFillColor(sf::Color::Green);
    circ.setPosition(100, 100);
    //Rectangle 3
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(400, 400));
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setPosition(200, 200);


    while (window.isOpen())
    {
        float sound_volume = sound_->get_sfx_volume();
        float music_volume = sound_->get_bgm_volume();
        //bool playing_bgm_;
        //bool pause_pressed_ = false;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
            {
                sound_->set_bgm_volume(music_volume - 1.0f);
                sound_->set_sfx_volume(sound_volume - 1.0f);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
            {
                float volume = sound_->get_bgm_volume();
                sound_->set_bgm_volume(volume + 1.0f);
                sound_->set_sfx_volume(volume + 1.0f);
            }

            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            {
                pause_pressed_ = false;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            {

                if ((playing_bgm_ == true) && (pause_pressed_ == false))
                {
                    sound_->play_sfx("ball.wav");
                   sound_->pause_bgm();
                   playing_bgm_ = false;
                   pause_pressed_ = true;
                   std::cout << "-------------------" << std::endl;
                }
                if ((playing_bgm_ == false) && (pause_pressed_ == false))
                {
                    sound_->play_sfx("ball.wav");
                    sound_->play_bgm();
                    playing_bgm_ = true;
                    pause_pressed_ = true;
                    std::cout << "-------------------" << std::endl;
                }
            }


            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
            {
                std::cout << "-------------------" << std::endl;
                std::cout << "     EXITING" << std::endl;
                std::cout << "-------------------" << std::endl;
                return 0;
            }
        }

        window.clear(sf::Color::Blue);
        window.draw(shape);
        window.draw(circ);
        window.draw(rectangle);
        window.draw(sprite);
        window.display();
    }

    return EXIT_SUCCESS;
}
