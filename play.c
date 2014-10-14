// Phillip Stewart

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


#define FNAME "play.bmp"
#define HEIGHT 960
#define WIDTH 640

typedef struct _pixel{
    unsigned char B;
    unsigned char G;
    unsigned char R;
} pixel;


int isPrime(const int i)
{
	if (i <= 1) return 0;
	int j;
	for(j = 2; j*j <= i; j++)
	{
		if(0 == i%j) return 0;
	}
	return 1;
}


int isPrimeArray(const int i, int* prevPrimes)
{
	int ctr, primer = 0;
	
	for (ctr = prevPrimes[primer]; ctr*ctr <= i; ctr = prevPrimes[++primer])
	{
		if (0 == i%ctr) return 0;
	}
	return 1;
}


int* arrayOfPrimes(int numElems)
{
	int* p1;
	p1 = (int*) malloc(numElems * sizeof(int));
	int i, j = 0;
	p1[0] = 2;
	
	for (i=2; j<numElems; i++) if (isPrimeArray(i, p1)) p1[j++] = i;
	
	return p1;
}


int NumFactors(int num)
{
	if (num <= 1) return 1;
	int ctr;
	int numFacts = 1;
	
	for (ctr = 2; ctr*ctr < num; ctr++)
	{
		if (0 == num % ctr)
		{
			num /= ctr;
			++numFacts;
			ctr = 1;
		}
	}
	return numFacts;
}


int SumOfPrimeFactors(int num)
{
	if (num <= 1) return 1;
	int ctr;
	int sopf = 0;
	
	for (ctr = 2; ctr*ctr < num; ctr++)
	{
		if (0 == num % ctr)
		{
			num /= ctr;
			sopf += ctr;
			ctr = 1;
		}
	}
	return sopf;
}


void writeHeader(const int fd, unsigned int width, unsigned int height, unsigned int pixelBytesPerRow, unsigned int paddingBytesPerRow)
{
    static unsigned char header[54] = {66,77,0,0,0,0,0,0,0,0,54,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,1,0,24}; //rest is zeroes
    unsigned int* sizeOfFileEntry = (unsigned int*) &header[2];
    *sizeOfFileEntry = 54 + (pixelBytesPerRow+paddingBytesPerRow)*height;  
    unsigned int* widthEntry = (unsigned int*) &header[18];    
    *widthEntry = width;
    unsigned int* heightEntry = (unsigned int*) &header[22];    
    *heightEntry = height;    
    write(fd, header, 54);
}


void writeRow(const int fd, pixel pixels[], unsigned int pixelBytesPerRow, unsigned int paddingBytesPerRow)
{
    static unsigned char zeroes[3] = {0,0,0}; //for padding
	write(fd,pixels,pixelBytesPerRow);
	write(fd,zeroes,paddingBytesPerRow);
}


int main(void)
{
    int sopfs[WIDTH];
	int prims[WIDTH];
	int numfs[WIDTH];
	int avgfs[WIDTH];
	
    pixel pix[WIDTH];
    pixel *pp;
    
	//Open file, write header
	char filename[] = FNAME;
	int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);  
	unsigned int pixelBytesPerRow = WIDTH*sizeof(pixel);
    unsigned int paddingBytesPerRow = (4-(pixelBytesPerRow%4))%4;
	writeHeader(fd, WIDTH, HEIGHT, pixelBytesPerRow, paddingBytesPerRow);
	
    int row;
    int col;
	int i, j;
	//start at end, bmp loads upside-down
    for (row = HEIGHT - 1; row >= 0; row--)
	{
        for (i = 0, j = row * WIDTH; i < WIDTH; i++, j++)
		{
//			sopfs[i] = SumOfPrimeFactors(j);
			prims[i] = isPrime(j);
//			numfs[i] = NumFactors(j);
//			avgfs[i] = sopfs[i] / numfs[i] * 4;
//			if (sopfs[i] > 255) sopfs[i] = 255;
//			if (avgfs[i] > 255) avgfs[i] = 255;
		}
        for (col = 0; col < WIDTH; col++)
		{
			i = prims[col] * 255;
            pix[col].R = i;
			pix[col].G = i;
			pix[col].B = i;
//			if (i == 0) pix[col].R = avgfs[col];
		}
		writeRow(fd, pix, pixelBytesPerRow, paddingBytesPerRow);
	}
    
	close(fd);
    
    return 0;
}
