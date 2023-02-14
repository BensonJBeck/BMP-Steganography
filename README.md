# BMP-Steganography
A command line tool written in C which can be used to hide mp files within one another

'--info fileName.bmp' dumps fileName.bmp's metadata to terminal
'--reveal filename.bmp' reveals hidden image within fileName.bmp
'--hide decoy.bmp hidden.bmp' hides 'hidden.bmp' in 'decoy.bmp'

---File Requirements--
1.Decoy images must be larger than or the same size as the hidden image to prevent data loss
2.All files must be .BMP files for the tool to work
3.Images must be located in same directory as the binary of the program
