imgCL: imgCL.c
	gcc imgCL.c utilsFLDR.c utilsJPEG.c utilsPNG.c utilsBMP.c utilsOpenCL.c -framework OpenCL -ljpeg -lpng -o img