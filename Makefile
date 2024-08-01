all:
	g++ -o Luminara .\main.cpp -I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.5\include" -L"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.5\lib\x64" -lOpenCL
