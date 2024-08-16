g++ -I`gcc -print-file-name=plugin`/include -fPIC -fno-rtti -shared src/mirror.c -o mirror.so
