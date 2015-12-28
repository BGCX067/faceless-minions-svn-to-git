const int TICKS_PER_SECOND = 25;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

DWORD next_game_tick = GetTickCount();
int loops;
float interpolation;

bool game_is_running = true;
while( game_is_running ) {

    loops = 0;
    while( GetTickCount() > next_game_tick && loops < MAX_FRAMESKIP) {
        update_game();

        next_game_tick += SKIP_TICKS;
        loops++;
    }

    interpolation = float( GetTickCount() + SKIP_TICKS - next_game_tick )
                    / float( SKIP_TICKS );
    display_game( interpolation );
}
