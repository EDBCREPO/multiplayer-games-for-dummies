if [ ! -d "www" ]; then
    mkdir "www"
fi

em++ -o www/index.html main.cpp    \
     -lraylib -lwebsocket.js       \
     -I./include -L./lib           \
     -s USE_GLFW=3                 \
     -s ASYNCIFY=1                 \
     -s FETCH=1                    \
     -s WASM=1                     \
     -s ERROR_ON_UNDEFINED_SYMBOLS=0

#emrun ./www/index.html
#g++ -o server server.cpp -I./include ; 

./server