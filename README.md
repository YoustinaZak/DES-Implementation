#Compilation#
gcc -o3 main.c -o des

#Run Encryption#
des.exe "e" key.txt plaintext.text ciphertext.txt

#Run Decryption#
des.exe "d" key.txt plaintext.text ciphertext.txt
