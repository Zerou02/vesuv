time ./scripts/compileShader.sh
time ccache gcc -o vesuv *.cpp -lvulkan -lglfw -lstdc++ -lc -ldl -lm -lstb -lglm && ./vesuv