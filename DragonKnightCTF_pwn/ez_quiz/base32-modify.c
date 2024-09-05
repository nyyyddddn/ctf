#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char base32_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

int base32_lookup(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= '2' && c <= '7') return c - '2' + 26;
    return -1;
}

void __fastcall decode(char *encoded, char *decoded) {
    int buffer = 0;
    int bits_left = 0;
    int count = 0;
    int length = strlen(encoded);
    
    for (int i = 0; i < length; i++) {
        if (encoded[i] == '=') break; // Padding character
        
        int val = base32_lookup(encoded[i]);
        if (val == -1) continue; // Skip invalid characters
        
        buffer = (buffer << 5) | val;
        bits_left += 5;

        if (bits_left >= 8) {
            decoded[count++] = ~(char)((buffer >> (bits_left - 8)) & 0xFF);
            bits_left -= 8;
        }
    }
    decoded[count] = '\0';
}

int main() {
    char encoded[] = "XOW3JPFLXGCK7TWMX6GMZIGOTK7ZJIELS65KBHU3TOG2BT4ZUDEJPGVATS7JDPVNQ2QL7EM3UCHZNGUC"; // Example encoded string
    char decoded[256];
    
    decode(encoded, decoded);
    
    printf("Decoded: %s\n", decoded);
    
    return 0;
}
