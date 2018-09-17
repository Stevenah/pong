//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int BORDER_PADDING = 15;

int getDistance(SDL_Rect object_1, SDL_Rect object_2)
{
    int xDistance = object_1.x - object_2.x;
    int yDistance = object_1.y - object_2.y;

    return sqrt(pow(xDistance, 2) + pow(yDistance, 2));
}

class Paddle 
{
    protected:
        int m_movement_speed;

        bool m_player_controlled;

        SDL_Keycode m_up_button;
        SDL_Keycode m_down_button;

    public:
        SDL_Rect renderObject;

        Paddle(
            std::string name,
            int movement_speed,
            int x_pos,
            int y_pos)
        {
            m_movement_speed = movement_speed;

            renderObject.w = 10;
            renderObject.h = 60;
            renderObject.x = x_pos;
            renderObject.y = y_pos;
        }

        void setYPos(int y_pos)
        {
            renderObject.y = y_pos;
        }

        bool Collides(SDL_Rect object)
        {
            bool collisionX = renderObject.x + renderObject.w >= object.x &&
                object.x + object.w >= renderObject.x;
            
            bool collisionY = renderObject.y + renderObject.h >= object.y &&
                object.y + object.h >= renderObject.y;

            return collisionX && collisionY;
        }
};

class PlayerPaddle : public Paddle
{
    protected:
        SDL_Keycode m_up_button;
        SDL_Keycode m_down_button;
    public:
        PlayerPaddle(
            std::string name,
            int movement_speed,
            int x_pos,
            int y_pos,
            SDL_Keycode up_button,
            SDL_Keycode down_button) : Paddle(name, movement_speed, x_pos, y_pos)
        {
            m_up_button = up_button;
            m_down_button = down_button;
        }

        void Move( SDL_Event event )
        {
            if ( event.type == SDL_KEYDOWN ) {

                if ( event.key.keysym.sym == m_up_button )
                {
                    renderObject.y -= m_movement_speed;
                }

                if ( event.key.keysym.sym == m_down_button )
                {
                    renderObject.y += m_movement_speed;
                }
            }
        }
};

class AIPaddle : public Paddle
{
    public:
        AIPaddle(
            std::string name,
            int movement_speed,
            int x_pos,
            int y_pos) : Paddle(name, movement_speed, x_pos, y_pos)
        {

        }

        void Move(int ballPosition)
        {
            renderObject.y = ballPosition;
        }
};

class Ball 
{
    protected:
        int m_movement_speed;
        int m_directionY;
        int m_directionX;
    public:
        SDL_Rect renderObject;

        Ball(int movement_speed)
        {
            m_movement_speed = movement_speed;
            m_directionX = 1;
            m_directionY = 1;

            renderObject.w = 10;
            renderObject.h = 10;
            renderObject.x = (SCREEN_WIDTH / 2) - (renderObject.w / 2);
            renderObject.y = (SCREEN_HEIGHT / 2) - (renderObject.h / 2);
        }

        void ChangeDirectionX() { m_directionX *= -1; }
        void ChangeDirectionY() { m_directionY *= -1; }

        void Move()
        {
            renderObject.x += m_directionX * m_movement_speed;
            renderObject.y += m_directionY * m_movement_speed;

            if (renderObject.x <= 0 || renderObject.x >= SCREEN_WIDTH - renderObject.w)
            {
                renderObject.x = (SCREEN_WIDTH / 2) - (renderObject.w / 2);
                renderObject.y = (SCREEN_HEIGHT / 2) - (renderObject.h / 2);
            }
        }

        bool Collides(SDL_Rect object)
        {

            bool collisionX = renderObject.x + renderObject.w >= object.x &&
                object.x + object.w >= renderObject.x;
            
            bool collisionY = renderObject.y + renderObject.h >= object.y &&
                object.y + object.h >= renderObject.y;

            return collisionX && collisionY;
        }
};

class Border
{
    protected:
        int m_x_pos;
        int m_y_pos;
        SDL_Rect m_number[9][9];

    public:

        SDL_Rect renderObject;

        Border(int y_pos)
        {
            m_y_pos = y_pos;

            renderObject.w = SCREEN_WIDTH - 30;
            renderObject.h = 10;
            renderObject.x = (SCREEN_WIDTH / 2) - (renderObject.w / 2);
            renderObject.y = m_y_pos;
        }
};

int main( int argc, char* args[] )
{

    bool quit = false;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Pong",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED);

    PlayerPaddle *player_1 = new PlayerPaddle( "player_1", 4, 30, 30, SDLK_UP, SDLK_DOWN );
    
    
    // AIPaddle *player_1 = new AIPaddle( "player_1", 4, 30, 30 );
    AIPaddle *player_2 = new AIPaddle( "player_2", 4, SCREEN_WIDTH - 30 - 10, 30 );

    Ball *ball = new Ball( 4 );

    Border *top_border = new Border ( BORDER_PADDING );
    Border *bottom_border = new Border ( SCREEN_HEIGHT - BORDER_PADDING - 10 );

    while ( !quit )
    {
        while ( SDL_PollEvent( &event ) != 0 )
        {
            if ( event.type == SDL_QUIT ) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
        SDL_RenderClear( renderer );

        SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

        SDL_RenderFillRect( renderer, &player_1->renderObject );
        SDL_RenderFillRect( renderer, &player_2->renderObject );

        SDL_RenderFillRect( renderer, &ball->renderObject );

        SDL_RenderFillRect( renderer, &top_border->renderObject );
        SDL_RenderFillRect( renderer, &bottom_border->renderObject );

        player_1->Move( event );
        player_2->Move( ball->renderObject.y );

        ball->Move();

        if ( player_1->Collides( top_border->renderObject ) )
        {
            player_1->setYPos( top_border->renderObject.y + top_border->renderObject.h);
        }

        if ( player_2->Collides( top_border->renderObject ) )
        {
            player_2->setYPos( top_border->renderObject.y + top_border->renderObject.h);
        }


        if ( player_1->Collides( bottom_border->renderObject ) )
        {
            player_1->setYPos( bottom_border->renderObject.y - player_1->renderObject.h);
        }

        if ( player_2->Collides( bottom_border->renderObject ) )
        {
            player_2->setYPos( bottom_border->renderObject.y - player_2->renderObject.h);
        }


        if ( ball->Collides( player_1->renderObject ) || 
            ball->Collides( player_2->renderObject ) )
        {
            ball->ChangeDirectionX();
        }

        if ( ball->Collides( top_border->renderObject ) || 
            ball->Collides( bottom_border->renderObject ) )
        {
            ball->ChangeDirectionY();
        }

        SDL_RenderPresent( renderer );
        SDL_Delay( 10 );
    }

    SDL_DestroyWindow( window );
    window = NULL;
    SDL_Quit();

    return 0;
}