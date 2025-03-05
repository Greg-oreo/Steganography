# Steganography
How to run:

hide data:

stego.exe -hide -m messagefilename -c coverfilename -b 2 [-o optionalfile]

-hide: Hide data -m : File containing data to hide -c : Cover image file -b : Bits per pixel -o : Optional output file

extract data:

stego.exe -extract -s -b 2 [-o ]

-extract: Extract data -s : Stego image file -b : Bits per pixel -o : Optional output file
