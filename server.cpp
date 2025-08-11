#include <nodepp/nodepp.h>
#include <nodepp/timer.h>
#include <nodepp/http.h>
#include <nodepp/path.h>
#include <nodepp/ws.h>
#include <nodepp/fs.h>

using namespace nodepp;

queue_t<ws_t> list;

void onMain(){

    auto server = http::server([=]( http_t cli ){

        string_t dir = "www/index.html";
        if( cli.path.size() > 1 )
          { dir = path::join( "www", cli.path ); }

        if( !fs::exists_file(dir) ){
            cli.write_header( 404, header_t({
                { "content-type", "text/plain" } }
            ));
            cli.write("Oops: 404 Error");
            cli.close(); return;
        }

        auto str = fs::readable( dir );

        if( cli.headers["Range"].empty() ){

            cli.write_header( 200, header_t({
                { "Content-Length", string::to_string(str.size()) },
                { "Cross-Origin-Embedder-Policy", "require-corp" },
                { "Cross-Origin-Opener-Policy", "same-origin" },
            //  { "Cache-Control", "public, max-age=3600" },
                { "Content-Type",   path::mimetype(dir) }
            }));

            if(!regex::test(path::mimetype(dir),"audio|video",true) )
              { stream::pipe( str, cli ); }

        } else {

            auto  range = regex::match_all(cli.headers["Range"],"\\d+",true);
            ulong rang[3];rang[0] = string::to_ulong( range[0] );
                  rang[2] = min( rang[0]+CHUNK_MB(10), str.size() );
                  rang[1] = min( rang[0]+CHUNK_MB(10), str.size()-1 );

            cli.write_header( 206, header_t({
                { "Content-Range", string::format("bytes %lu-%lu/%lu",rang[0],rang[1],str.size()) },
                { "Cross-Origin-Embedder-Policy", "require-corp" },
                { "Cross-Origin-Opener-Policy", "same-origin" },
                { "Content-Type",  path::mimetype(dir) },
                { "Accept-Range", "bytes" }
            }));

            str.set_range( rang[0], rang[2] );
            stream::pipe( str, cli );

        }

    }); ws::server( server );

    server.onConnect([=]( ws_t cli ){

        list.push( cli ); auto ID = list.last();
        auto fd = cli.get_fd();

        cli.onData([=]( string_t data ){
            auto msg = regex::format("player_${0}\n${1}",fd,data);
            auto x = list.first(); while( x!=nullptr ){
            auto y = x->next;
                if( x->data.get_fd() != fd )
                  { x->data.write( msg ); }
            x=y; }
        });

        cli.onDrain([=](){
            console::log( "Disconnected" );
            list.erase( ID );
        });

        console::log( "Connected" );

    });

    server.listen( "localhost", 8000, [=]( ... ){
        console::log("server started at http://localhost:8000");
    });

}
