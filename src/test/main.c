#include <stdio.h>
#include <string.h>

#include <huge.h>

int main() {
    huge_t a, b, c;
    char cmd[20], sa[1000], sb[1000], sc[1000];

    printf("Commands: add(2), sub(2), mul(2), div(2), mod(2), p(1), quit(0)\n");
    while (1) {
        printf(" * ");
        scanf("%s", cmd);

        if (strcmp(cmd, "add") == 0) {
            scanf("%s%s", sa, sb);
            a = huge_from_dec(sa);
            b = huge_from_dec(sb);
            c = huge_add(a, b);
            huge_to_dec(c, sc, NULL);
            printf("Result: %s\n", sc);
            huge_free(a);
            huge_free(b);
            huge_free(c);
        } else if (strcmp(cmd, "sub") == 0) {
            scanf("%s%s", sa, sb);
            a = huge_from_dec(sa);
            b = huge_from_dec(sb);
            c = huge_sub(a, b);
            huge_to_dec(c, sc, NULL);
            printf("Result: %s\n", sc);
            huge_free(a);
            huge_free(b);
            huge_free(c);
        } else if (strcmp(cmd, "mul") == 0) {
            scanf("%s%s", sa, sb);
            a = huge_from_dec(sa);
            b = huge_from_dec(sb);
            c = huge_mul(a, b);
            huge_to_dec(c, sc, NULL);
            printf("Result: %s\n", sc);
            huge_free(a);
            huge_free(b);
            huge_free(c);
        } else if (strcmp(cmd, "div") == 0) {
            scanf("%s%s", sa, sb);
            a = huge_from_dec(sa);
            b = huge_from_dec(sb);
            c = huge_div(a, b);
            huge_to_dec(c, sc, NULL);
            printf("Result: %s\n", sc);
            huge_free(a);
            huge_free(b);
            huge_free(c);
        } else if (strcmp(cmd, "p") == 0) {
            scanf("%s", sa);
            a = huge_from_dec(sa);
            huge_to_bin(a, sa, NULL);
            printf("bin: %s\n", sa);
            huge_to_oct(a, sa, NULL);
            printf("oct: %s\n", sa);
            huge_to_dec(a, sa, NULL);
            printf("dec: %s\n", sa);
            huge_to_hex(a, sa, NULL);
            printf("hex: %s\n", sa);
            huge_free(a);
        } else {
            break;
        }
    }

    return 0;
}

