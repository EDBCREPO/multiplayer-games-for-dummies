# Your first multiplayer Games | A guide for Absolute Beginners with Raylib, Nodepp and WASM
![ezgif-1207837eb919d6](https://github.com/user-attachments/assets/d88dbed7-49ce-4121-8550-7f82df1f995a)

## dependencies:
```bash
nodepp-wasm: https://github.com/NodeppOfficial/nodepp-wasm
nodepp: https://github.com/NodeppOfficial/nodepp
raylib: https://github.com/raysan5/raylib
```

## Build
```bash
#!/bin/bash

# Create the www directory if it doesn't already exist.
if [ ! -d "www" ]; then
    mkdir "www"
fi

# Use the Emscripten compiler (em++) to compile the C++ client code.
em++ -o www/index.html client.cpp  \
     -lraylib -lwebsocket.js       \
     -I./include -L./lib           \
     -s USE_GLFW=3                 \
     -s ASYNCIFY=1                 \
     -s FETCH=1                    \
     -s WASM=1                     \
     -s ERROR_ON_UNDEFINED_SYMBOLS=0

# Compile and run the C++ server application.
g++ -o server server.cpp -I./include ; ./server
```

# How it works
[![IMAGE](https://miro.medium.com/v2/resize:fit:720/format:webp/1*CRS3yK4yGTN6E5c2x1NyDQ.png)](https://medium.com/@EDBCBlog/your-first-multiplayer-games-a-guide-for-absolute-beginners-with-raylib-nodepp-and-wasm-532133942b43)

[Click Here](https://medium.com/@EDBCBlog/your-first-multiplayer-games-a-guide-for-absolute-beginners-with-raylib-nodepp-and-wasm-532133942b43)
