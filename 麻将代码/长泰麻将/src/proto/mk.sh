

make clean; 

protoc --cpp_out=./ *.proto 

make  -j 
