module load eigen
g++ rect_square.cpp -o convert 
rm log.dat
# ./convert 2>&1 "log.dat"
./convert >> "log.dat"