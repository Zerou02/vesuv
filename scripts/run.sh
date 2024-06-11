time ./scripts/compileShader.sh
time gcc -O3 -o vesuv *.cpp -lvulkan -lglfw -lstdc++ -lc -ldl -lm -lstb -lglm && ./vesuv