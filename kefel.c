#include <stdio.h>
#include <stdlib.h>

int i;

int powOfTwo(int k) {
    while (k != 1) {
        if (k % 2 == 1) {
            return 0;
        }
        k /= 2;
    }
    return 1;
}

int bitArray_is_zero(const int bitArray[32]) {
    for (i = 0; i < 32; i++) {
        if (bitArray[i] == 1) {
            return 0;
        }
    }
    return 1;
}

int power(int base, int power) {
    int sum = 1;
    for (i = 0; i < power; i++) {
        sum *= base;
    }
    return sum;
}

void int_to_bitArray(int array[32], int k) {
    for (i = 0; i < 32; i++) {
        array[i] = 0;
    }
    if (k < 0) {
        k = -k;
    }
    for (i = 30; i >= 0; i--) {
        if (k == 0) {
            break;
        }
        int x = power(2, i);
        if (x <= k) {
            array[i] = 1;
            k -= x;
        }
    }
}

int shift(int k, int count_shift) {
    while (k != 1) {
        k /= 2;
        count_shift++;
    }
    return count_shift;
}

int main(int argc, char *argv[]) {
    if (argc != 2 || ((atoi(argv[1]) == 0) && *argv[1] != '0')) {
        printf("Bad Input\n");
        exit(1);
    }
    int k = atoi(argv[1]);
    FILE *fileP = fopen("kefel.s", "w");
    fprintf(fileP, ".section .text\n.globl   kefel\nkefel: ");
    if (k == 0) {
        fprintf(fileP, "movl $0, %%eax\n");
    } else if (k == 1) {
        fprintf(fileP, "movl %%edi,%%eax\n");
    } else if (k == -1) {
        fprintf(fileP, "movl %%edi,%%eax\nneg %%eax\n");
    } else if (k == 3 || k == 5 || k == 9) {
        fprintf(fileP, "leal (%%eax, %%eax, %d), %%eax\n", k - 1);
    } else if ((k % 9 == 0 && (k / 9) % 9 != 0 && powOfTwo(k / 9))) {
        int count_shift = 0;
        k /= 9;
        count_shift = shift(k, count_shift);
        fprintf(fileP, "leal (%%edi,%%edi,8), %%eax\nshll $%d, %%eax\n", count_shift);
    } else if ((k % 5 == 0 && (k / 5) % 5 != 0 && powOfTwo(k / 5))) {
        int count_shift = 0;
        k /= 5;
        count_shift = shift(k, count_shift);
        fprintf(fileP, "leal (%%edi,%%edi,4), %%eax\nshll $%d, %%eax\n", count_shift);
    } else if ((k % 3 == 0 && (k / 3) % 3 != 0 && powOfTwo(k / 3))) {
        int count_shift = 0;
        k /= 3;
        count_shift = shift(k, count_shift);
        fprintf(fileP, "leal (%%edi,%%edi,2), %%eax\nshll $%d, %%eax\n", count_shift);
    } else {
        int bitArray[32];
        int eax_isempty = 1, start_index = 0, lsb_flag = 0;
        int_to_bitArray(bitArray, k);
        while (!bitArray_is_zero(bitArray)) {
            int m = 0, n = 0, m_assigned = 0, n_assigned = 0;
            for (i = start_index; i < 32; i++) {
                if (bitArray[i] == 1 && !m_assigned) {
                    n = i;
                    m = i;
                    n_assigned = 1;
                    m_assigned = 1;
                    bitArray[i] = 0;
                } else if (bitArray[i] == 1 && m_assigned) {
                    n = i;
                    bitArray[i] = 0;
                } else if (bitArray[i] == 0 && n_assigned) {
                    if (i == 1) {
                        n_assigned = 0;
                        m_assigned = 0;
                        lsb_flag = 1;
                        continue;
                    }
                    start_index = i + 1;
                    break;
                }
            }
            if (n >= m + 1 && eax_isempty) {
                if (k > 0) {
                    fprintf(fileP,
                            "movl %%edi,%%eax\nmovl %%edi,%%ebx\nshll $%d, %%eax\nshll $%d, %%ebx\nsubl %%ebx,%%eax\n",
                            n + 1, m);
                }
                else {
                    fprintf(fileP,
                            "movl %%edi,%%eax\nmovl %%edi,%%ebx\nshll $%d, %%eax\nshll $%d, %%ebx\nsubl %%ebx,%%eax\n",
                            m, n + 1);
                }
                eax_isempty = 0;
            } else if (n >= m + 1 && !eax_isempty) {
                if(k>0) {
                    fprintf(fileP,
                            "movl %%edi,%%ebx\nmovl %%edi,%%ecx\nshll $%d, %%ebx\nshll $%d, %%ecx\nsubl %%ecx,%%ebx\naddl %%ebx,%%eax\n",
                            n + 1, m);
                }
                else{
                    fprintf(fileP,
                            "movl %%edi,%%ebx\nmovl %%edi,%%ecx\nshll $%d, %%ebx\nshll $%d, %%ecx\nsubl %%ecx,%%ebx\naddl %%ebx,%%eax\n",
                            m, n+1);
                }
            } else if (n == m && eax_isempty) {
                fprintf(fileP, "movl %%edi,%%eax\nshll $%d, %%eax\n", n);
                if (k < 0) {
                    fprintf(fileP, "neg %%eax\n");
                }
                eax_isempty = 0;
            } else if (n == m && !eax_isempty) {
                if (k > 0) {
                    fprintf(fileP, "movl %%edi,%%ebx\nshll $%d, %%ebx\naddl %%ebx,%%eax\n", n);
                }
                else {
                    fprintf(fileP, "movl %%edi,%%ebx\nshll $%d, %%ebx\nsubl %%ebx,%%eax\n", n);
                }
            }
        }
        if (lsb_flag) {
            fprintf(fileP, "addl %%edi,%%eax\n");
        }
        fprintf(fileP, "ret\n");
        fclose(fileP);
        return 0;
    }
}
