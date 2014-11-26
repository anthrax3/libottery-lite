
#include "otterylite.h"
#include "otterylite-impl.h"
#include "otterylite_rng.h"

#include <assert.h>

#define MAXSKIP 8192

static void
dumphex(const char *label, const u8 *bytes, int n)
{
  int m = 0;
  if (label) printf("%s: ", label);
  while (n--) {
    printf("%02x", *bytes++);
    if (0 == (++m % 32) && n)
      printf("\n");
  }
  puts("");
}

static void
experiment(const u8 *key, const u8 *nonce, unsigned skip)
{
  const unsigned OUTPUT = 512;
  u8 buf[MAXSKIP + OUTPUT];
  u8 k[KEYLEN];
  unsigned n = skip + OUTPUT;

  assert(n <= sizeof(buf));

  memset(k, 0, sizeof(k));
  memcpy(k, key, 32);
  memcpy(k + 32, nonce, 8);

  chacha20_blocks(k, n / BLOCKSIZE, buf);

  puts("================================================================");
  dumphex("   key", key, 32);
  dumphex(" nonce", nonce, 8);
  printf("offset: %d\n", skip);

  dumphex(NULL, buf + skip, OUTPUT);
}

#define X(key,nonce,skip)                                              \
  do {                                                                 \
    experiment((const u8*)(key),(const u8*)(nonce),(skip));            \
  } while (0)


int
main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  (void)ottery_setkey;
  (void)ottery_bytes;

  X("helloworld!helloworld!helloworld", "!hellowo", 0);
  X("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
    "\0\0\0\0\0\0\0\0", 0);
  X("helloworld!helloworld!helloworld", "!hellowo", 8192);
  X("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
    "\0\0\0\0\0\0\0\0", 8192);
  X("Zombie ipsum reversus ab viral i", "nferno, ", 128);
  X("nam rick grimes malum cerebro. D", "e carne ", 512);
  X("lumbering animata corpora quaeri", "tis. Sum", 640);
  X("mus brains sit, morbo vel malefi", "cia? De ", 704);

  return 0;
}