#include "zipFile.h"
#include "unzipFile.h"
#include <stdio.h>
#include <string.h>

static int writeTextFile(const char *path, const char *content) {
    FILE *f = fopen(path, "wb");
    if (!f) return 1;
    fwrite(content, 1, strlen(content), f);
    fclose(f);
    return 0;
}

static int filesEqual(const char *a, const char *b) {
    FILE *fa = fopen(a, "rb");
    FILE *fb = fopen(b, "rb");
    int ca, cb;

    if (!fa || !fb) {
        if (fa) fclose(fa);
        if (fb) fclose(fb);
        return 0;
    }

    while (1) {
        ca = fgetc(fa);
        cb = fgetc(fb);
        if (ca != cb) {
            fclose(fa);
            fclose(fb);
            return 0;
        }
        if (ca == EOF) {
            break;
        }
    }

    fclose(fa);
    fclose(fb);
    return 1;
}

static int runRoundTripCase(const char *name, const char *content) {
    char input[128];
    char zip[128];
    char out[128];

    snprintf(input, sizeof(input), "test_%s.txt", name);
    snprintf(zip, sizeof(zip), "test_%s.zip", name);
    snprintf(out, sizeof(out), "test_%s_out.txt", name);

    if (writeTextFile(input, content) != 0) {
        printf("[FAIL] %s: cannot write input\n", name);
        return 1;
    }

    if (zipFilePath(input, zip) != 0) {
        printf("[FAIL] %s: zip failed\n", name);
        return 1;
    }

    if (unzipFilePath(zip, out) != 0) {
        printf("[FAIL] %s: unzip failed\n", name);
        return 1;
    }

    if (!filesEqual(input, out)) {
        printf("[FAIL] %s: roundtrip mismatch\n", name);
        return 1;
    }

    printf("[PASS] %s\n", name);
    return 0;
}

int main(void) {
    int failures = 0;

    failures += runRoundTripCase("simple", "aaaaabbbbbcccccdddddeeeee");
    failures += runRoundTripCase("mixed", "The quick brown fox jumps over the lazy dog. 1234567890");
    failures += runRoundTripCase("repeated", "xyzxyzxyzxyzxyzxyzxyzxyz");
    failures += runRoundTripCase("multiline", "line1\nline2\nline3\nline4\n");

    if (failures == 0) {
        printf("[OK] All LZ77 roundtrip tests passed.\n");
        return 0;
    }

    printf("[ERROR] LZ77 tests failed: %d\n", failures);
    return 1;
}
