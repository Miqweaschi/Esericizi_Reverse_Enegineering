#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <unistd.h>


struct VaultState {
    char username[32];
    unsigned int user_hash;
    char license_key[20];
    int access_granted;
};


void security_check() {
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        printf(" Errore di sistema rilevato (0xDEADBEEF)\n");
        exit(1);
    }
}

unsigned int generate_hash(char *name) {
    unsigned int hash = 0x1337;
    for (int i = 0; i < strlen(name); i++) {
        hash = ((hash << 5) + hash) + name[i]; 
    }
    return hash;
}


int verify_key(struct VaultState *state) {
    
    unsigned char expected_bytes[] = {0xde, 0xad, 0xbe, 0xef, 0x42};
    
    for (int i = 0; i < 5; i++) {
        unsigned char transform = (state->user_hash >> (i * 8)) & 0xFF;
        if ((state->license_key[i] ^ transform) != expected_bytes[i]) {
            return 0;
        }
    }
    return 1;
}

int main() {
   
    security_check();

    struct VaultState *v = (struct VaultState *)malloc(sizeof(struct VaultState));
    v->access_granted = 0;

    printf("--- ENIGMA VAULT v2.0 ---\n");
    printf("Username: ");
    scanf("%31s", v->username);
    
    v->user_hash = generate_hash(v->username);

    printf("License Key: ");
    scanf("%19s", v->license_key);

    if (verify_key(v)) {
        v->access_granted = 1;
    }

    if (v->access_granted) {
        printf("\n[ACCESS GRANTED] Benvenuto operatore: %s\n", v->username);
    } else {
        printf("\n[ACCESS DENIED]\n");
    }

    free(v);
    return 0;
}