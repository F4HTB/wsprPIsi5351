#ifndef MEncode_H
#define MEncode_H

#define WSPR_SYMBOL_COUNT                   162


void wspr_encode(const char * call, const char * loc, const int8_t dbm, uint8_t * symbols);
void wspr_message_prep(char * call, char * loc, int8_t dbm);
void wspr_bit_packing(uint8_t * c);
void convolve(uint8_t * c, uint8_t * s, uint8_t message_size, uint8_t bit_size);
void wspr_interleave(uint8_t * s);
void wspr_merge_sync_vector(uint8_t * g, uint8_t * symbols);
uint8_t wspr_code(char c);
void pad_callsign(char * call);

#endif //MEncode_H