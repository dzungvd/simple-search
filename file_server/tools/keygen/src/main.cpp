#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
extern "C" {
  #include "sodium.h"
}
#include "message_crypto.h"

#define MESSAGE (const unsigned char *) "Message"
#define MESSAGE_LEN 7
#define CIPHERTEXT_LEN (crypto_box_SEALBYTES + MESSAGE_LEN)


int main (int argc, char** args) {

  unsigned char recipient_pk[crypto_box_PUBLICKEYBYTES];
  unsigned char recipient_sk[crypto_box_SECRETKEYBYTES];
  crypto_box_keypair(recipient_pk, recipient_sk);
  std::cout << "public key: " << recipient_pk << std::endl
            << "private key: " << recipient_sk << std::endl;

  std::ofstream fout ("secret_key.key", std::ios::binary);

  fout.write ((char*)recipient_sk, crypto_box_SECRETKEYBYTES);

  fout.close();

  fout.open ("public_key.key", std::ios::binary);

  fout.write ((char*)recipient_pk, crypto_box_PUBLICKEYBYTES);

  fout.close();
  return 0;
}
