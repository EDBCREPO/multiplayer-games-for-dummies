
#include <nodepp/nodepp.h>
#include <nodepp/timer.h>
#include <nodepp/json.h>
#include <nodepp/date.h>
#include <nodepp/ws.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace rl {
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/raygl.h>
};

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

event_t<>         onOpen;
event_t<float>    onLoop;
event_t<>         onDraw;
event_t<>         onClose;

event_t<string_t> onSend;
event_t<>         onConnect;
event_t<string_t> onMessage;
event_t<>         onDisconnect;

/*────────────────────────────────────────────────────────────────────────────*/

void start_client(){

    auto srv = ws::connect( "ws://localhost:8000" );

    srv.onConnect([=]( ws_t cli ){

        cli.onData([]( string_t data ){ onMessage.emit( data ); });

        cli.onClose([](){ onDisconnect.emit(); onSend.clear(); });

        onSend.on([=]( string_t data ){ cli.write( data ); });

        onConnect.emit(); console::log("connected");

    });

}

/*────────────────────────────────────────────────────────────────────────────*/

void start_player(){

    ptr_t<rl::Vector2> pos = new rl::Vector2({ 30, 30 });
    ptr_t<int> /*-*/ speed = new int( 100 );
    
    onDraw.on([=](){
        rl::ClearBackground( rl::GRAY );
        rl::DrawCircleV( *pos, 10, rl::BLUE );
    });

    onLoop.on([=]( float delta ){

        if( rl::IsKeyDown( 'W' ) )
          { pos->y += delta ** speed *-1; }
      elif( rl::IsKeyDown( 'S' ) )
          { pos->y += delta ** speed * 1; }

        if( rl::IsKeyDown( 'A' ) )
          { pos->x += delta ** speed *-1; }
      elif( rl::IsKeyDown( 'D' ) )
          { pos->x += delta ** speed * 1; }

    });

    timer::interval([=](){

        onSend.emit( json::stringify( object_t({
            { "pos", object_t({
            { "x", string::to_string( pos->x ) },
            { "y", string::to_string( pos->y ) }
        }) } }) ));
        
    }, 1 );

}

/*────────────────────────────────────────────────────────────────────────────*/

void start_npc(){

    map_t<string_t,object_t> players;

    onDraw([=](){

        players.map([=]( type::pair<string_t,object_t> val ){

            rl::Vector2 pos ({
                string::to_float( val.second["pos"]["x"].as<string_t>() ),
                string::to_float( val.second["pos"]["y"].as<string_t>() )
            });

            rl::DrawCircleV( pos, 10, rl::RED );

        });

    });

    onMessage([=]( string_t data ){ try {
        auto mem = regex::get_memory( data, "([^\n]+)\n(.+)" );
        if ( mem.size() != 2 ){ throw ""; }
        players[ mem[0] ] = json::parse( mem[1] );
    } catch(...) {} });

}

/*────────────────────────────────────────────────────────────────────────────*/

int main( int argc, char** argv ) {

    rl::InitWindow( 300, 300, "raylib - wasm multiplayer");
    rl::SetTargetFPS( 60 ); start_client();
    
    onOpen   .once([=](){ start_player(); });
    onConnect.once([=](){ start_npc   (); });

    /*─······································································─*/

    process::add( coroutine::add( COROUTINE(){
    coBegin ; onOpen.emit();

        for(;;){
            onLoop.emit( rl::GetFrameTime() );
            rl::BeginDrawing(); onDraw.emit();
            rl::EndDrawing();   coNext;
        }

    coFinish
    }));

    /*─······································································─*/

    while( !rl::WindowShouldClose() ){ process::next(); }
    onClose.emit ( ); rl::CloseWindow();
    process::exit(1);

    /*─······································································─*/

    return 0;

}

/*────────────────────────────────────────────────────────────────────────────*/