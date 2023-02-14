#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <string.h>

#pragma pack(1)
struct header{
    char bm[2];

    int size;

    uint16_t reserve;

    uint16_t reserve1;

    int off;

    int headSize;

    int width;

    int height;

    uint16_t numColorPlanes;

    uint16_t bitsPChanel;

    int compScheme;

    int imgSize;

    int hRes;

    int vRes;

    int numColor;

    int imColor;
}head;

//Flips the LSB and MSB of a given byte//
uint8_t flipByte(uint8_t color){
    return ((color << 4) | (color >> 4));
}

//Function which prints the binary of a given byte, used during development//
void printBin(int num){
    for(int i = 7; i >= 0; i --){
        printf("%d", ((num >> i)&1));
    }
    printf("\n");
}

//Function to print the data of a BMP file//
void printInfo(struct header head){
	printf("=== BMP Header ===\nType: %c%c\nSize: %d\nReserved 1: %d\nReserved 2: %d\nImage offset: %d\n\n=== DIB Header ===\nSize : %d\nWidth : %d\nHeight : %d\n# color planes : %d\n# bits per pixel : %d\nCompression scheme : %d\nImage size : %d\nHorizontal resolution : %d\nVertical resolution : %d\n# colors in palette : %d\n# important colors : %d\n", head.bm[0],head.bm[1], head.size, head.reserve, head.reserve1, head.off, head.headSize, head.width, head.height, head.numColorPlanes, head.bitsPChanel, head.compScheme, head.imgSize, head.hRes, head.vRes, head.numColor, head.imColor);
	return;
}

//---Begin main program---//
int main(int argc, const char **argv) {
	int offset = 0;

	FILE *fileStream;
	FILE *fileStream1;

    //headers used to store the info of each image//
	struct header head1;

	if(argc == 3 && strcmp(argv[1], "--info") == 0){

		fileStream = fopen(argv[2], "r");

		if(fileStream == NULL){
			printf("Couldn't open file\n");
			return -1;
		}

		fread(&head, sizeof(head), 1, fileStream);
		printInfo(head);
		fclose(fileStream);
	}

	else if(argc == 3 && strcmp(argv[1], "--reveal") == 0){
		fileStream = fopen(argv[2], "r+");

		//verifies the file has been opened properly
		if(fileStream == NULL){
			printf("Couldn't open file correctly\n");
			return -1;
		}

		//reads header of BMP file
		fread(&head, sizeof(head), 1, fileStream);

		//verfies the file is a BMP file
		if(head.bm[0] != 'B' || head.bm[1] != 'M'){
			printf("File format not supported...\n");
			return -1;
		}

		//determines how many padding bytes are used at the end of a row
		while((head.width + offset) % 4 != 0){
			offset ++;
		}

		//advances to the start of the image array
	    	fseek(fileStream, head.off, SEEK_SET);

		//var used later to flip bytes
		uint8_t color = 0;

		for(int y = 0; y < head.height; y ++){
			for(int x = 0; x < head.width; x ++){
				//loops through the RGB value at pixel (x,y)
				for(int c = 0; c < 3; c ++){
					//grabs byte
					fread(&color, 1, 1, fileStream);

					//back tracks 1 byte
					fseek(fileStream, -1, SEEK_CUR);

					//writes flipped byte over byte//
					color = flipByte(color);
					fwrite(&color, 1, 1, fileStream);
				}
			}
			//skips offset
			fseek(fileStream, offset, SEEK_CUR);
		}

		//closes the file stream saves edits
		fclose(fileStream);
	}

	else if(argc == 4 && strcmp(argv[1], "--hide") == 0){
		fileStream = fopen(argv[2], "r+");
		fileStream1 = fopen(argv[3], "r");

		//grabs header of both images//
		fread(&head, sizeof(head), 1, fileStream);
		fread(&head1, sizeof(head1), 1, fileStream1);

		//advances file stream past the headers//
		fseek(fileStream, 54, SEEK_SET);
		fseek(fileStream1, 54, SEEK_SET);

		if(fileStream == NULL || fileStream1 == NULL){
			printf("Couldn't open files properly\n");
			return -1;
		}

		if(head.bm[0] != 'B' || head.bm[1] != 'M' || head1.bm[0] != 'B' || head1.bm[1] != 'M'){
			printf("File format not supported...");
			return -1;
		}

		//determines how many padding bytes are used at the end of a row
		while((head.width + offset) % 4 != 0){
			offset ++;
		}

		//var used later to flip bytes
		uint8_t color = 0;
		uint8_t color1 = 0;

		for(int y = 0; y < head.height; y ++){
			for(int x = 0; x < head.width; x ++){
				//loops through the RGB value at pixel (x,y)
				for(int c = 0; c < 3; c ++){
					//grabs byte
					fread(&color, 1, 1, fileStream);
					fread(&color1, 1, 1, fileStream1);

					//back tracks 1 byte
					fseek(fileStream, -1, SEEK_CUR);

					//bitshifting magic to produce the merged color
                    			uint8_t merge = ((color >> 4)<<4) | (color1 >> 4);

					//writes merged byte over byte//
					fwrite(&merge, 1, 1, fileStream);
				}
			}
			//skips offset in both file streams
			fseek(fileStream, offset, SEEK_CUR);
			fseek(fileStream1, offset, SEEK_CUR);
		}

        //closes both file streams, saves output//
		fclose(fileStream);
		fclose(fileStream1);
	}

	//user didn't provide enough arguments for any commands to be properly executed//
	else{
		printf("Too few arguments provided\n");
		return -1;
	}
	return 0;
}
