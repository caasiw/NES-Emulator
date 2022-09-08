#include <stdint.h>
#include "cpu.h"
#include "memory.h"
#include "opcodes.h"

// FILE *logfile;

#define c (1 << 0)
#define z (1 << 1)
#define i (1 << 2)
#define d (1 << 3)
#define b (1 << 4)
#define u (1 << 5)
#define v (1 << 6)
#define n (1 << 7)

uint16_t read16(uint16_t address) {
    return ((cpu_read(address + 1) << 8) | cpu_read(address));
}

uint16_t read16wrap(uint16_t address) {
    if ((address & 0x00FF) == 0x00FF)
        return ((cpu_read(address & 0xFF00) << 8) | cpu_read(address));
    else
        return read16(address);
}

uint8_t pop(struct cpu_state *cpu) {
    return cpu_read(0x0100 + ++cpu->sp);
}

uint16_t pop16(struct cpu_state *cpu) {
    uint16_t popped = pop(cpu);
    popped |= (pop(cpu) << 8);
    return popped;
}

void push(struct cpu_state *cpu, uint8_t data) {
    cpu_write(0x0100 + cpu->sp--, data);
}

void push16(struct cpu_state *cpu, uint16_t data) {
    push(cpu, (data >> 8));
    push(cpu, (data & 0x00FF));
}

int pageCheck(uint16_t address, uint8_t offset) {
    return ( (((address & 0x00FF) + offset) & 0xFF00) ? 1 : 0);
}

uint8_t fetchOperand(struct cpu_state *cpu) {
    void *mode = opcodes[cpu->opcode].mode;
    if (mode == &acc)
        return cpu->acc;
    else if (mode == &imm)
        return cpu->byte2;
    else
        return cpu_read(cpu->opAddress);
}

/* Addressing Modes */
int imp(struct cpu_state *cpu) {
    return 0;
}

int acc(struct cpu_state *cpu) {
    return 0;
}

int imm(struct cpu_state *cpu) {
    cpu->opAddress = cpu->pc++;
    cpu->byte2 = cpu_read(cpu->opAddress);
    return 0;
}

int zpa(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->opAddress = cpu->byte2;
    return 0;
}

int zpx(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->opAddress = (cpu->byte2 + cpu->x) & 0x00FF;
    return 0;
}

int zpy(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->opAddress = (cpu->byte2 + cpu->y) & 0x00FF;
    return 0;
}

int aba(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->byte3 = cpu_read(cpu->pc++);
    cpu->opAddress = (cpu->byte3 << 8) | cpu->byte2;
    return 0;
}

int abx(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->byte3 = cpu_read(cpu->pc++);
    cpu->opAddress = ((cpu->byte3 << 8) | cpu->byte2) + cpu->x;
    if ((cpu->byte2 + cpu->x) & 0xFF00)
        return 1;
    return 0;
}

int aby(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->byte3 = cpu_read(cpu->pc++);
    cpu->opAddress = ((cpu->byte3 << 8) | cpu->byte2) + cpu->y;
    if ((cpu->byte2 + cpu->y) & 0xFF00)
        return 1;
    return 0;
}

int ind(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->byte3 = cpu_read(cpu->pc++);
    cpu->opAddress = (cpu->byte3 << 8) | cpu->byte2;
    cpu->opAddress = read16wrap(cpu->opAddress);
    return 0;
}

int idx(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->opAddress = read16wrap((cpu->byte2 + cpu->x) & 0x00FF);
    return 0;
}

int idy(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    uint16_t temp = read16wrap(cpu->byte2);
    cpu->opAddress = temp + cpu->y;
    return pageCheck(temp, cpu->y);
}

int rel(struct cpu_state *cpu) {
    cpu->byte2 = cpu_read(cpu->pc++);
    cpu->opAddress = ( (cpu->byte2 & 0x80) ? cpu->pc - 128 : cpu->pc);
    cpu->opAddress += (cpu->byte2 & 0x7F);
    return ((cpu->opAddress & 0xFF00) != (cpu->pc & 0xFF00));
}

/* Bitwise Operations */
int and(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->acc &= operand;
    cpu->status = ( (cpu->acc == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->acc & n ) ? (cpu->status | n) : (cpu->status & ~n));
    return 1;
}

int eor(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->acc ^= operand;
    cpu->status = ( (cpu->acc == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->acc & n ) ? (cpu->status | n) : (cpu->status & ~n));
    return 1;
}

int ora(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->acc |= operand;
    cpu->status = ( (cpu->acc == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->acc & n ) ? (cpu->status | n) : (cpu->status & ~n));
    return 1;
}

int asl(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->status = ( (operand & n ) ? (cpu->status | c) : (cpu->status & ~c));
    operand <<= 1;
    cpu->status = ( (operand == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (operand & n ) ? (cpu->status | n) : (cpu->status & ~n));
    if (opcodes[cpu->opcode].mode == &acc)
        cpu->acc = operand;
    else
        cpu_write(cpu->opAddress, operand);
    return 1;
}

int lsr(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->status = ( (operand & c ) ? (cpu->status | c) : (cpu->status & ~c));
    operand >>= 1;
    cpu->status = ( (operand == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status &= ~n;
    if (opcodes[cpu->opcode].mode == &acc)
        cpu->acc = operand;
    else
        cpu_write(cpu->opAddress, operand);
    return 1;
}

int rol(struct cpu_state *cpu) {
    uint16_t operand = fetchOperand(cpu);
    operand = (operand << 1) | (cpu->status & c);
    cpu->status = ( (operand & 0x100) ? (cpu->status | c) : (cpu->status & ~c));
    operand &= 0xFF;
    cpu->status = ( (operand == 0   ) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (operand & n    ) ? (cpu->status | n) : (cpu->status & ~n));
    if (opcodes[cpu->opcode].mode == &acc)
        cpu->acc = (uint8_t)operand;
    else
        cpu_write(cpu->opAddress, (uint8_t)operand);
    return 0;
}

int ror(struct cpu_state *cpu) {
    uint16_t operand = fetchOperand(cpu);
    operand |= ( (cpu->status & c) ? 0x100 : 0);
    cpu->status = ( (operand & c ) ? (cpu->status | c) : (cpu->status & ~c));
    operand >>= 1;
    cpu->status = ( (operand == 0   ) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (operand & n    ) ? (cpu->status | n) : (cpu->status & ~n));
    if (opcodes[cpu->opcode].mode == &acc)
        cpu->acc = (uint8_t)operand;
    else
        cpu_write(cpu->opAddress, (uint8_t)operand);
    return 0;
}

/* Comparison */
int bit(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->status = ((operand & cpu->acc) ? (cpu->status & ~z):(cpu->status | z));
    cpu->status = ((operand & v) ? (cpu->status | v) : (cpu->status & ~v));
    cpu->status = ((operand & n) ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int cmp(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    uint8_t temp = cpu->acc - operand;
    cpu->status = ((cpu->acc >= operand)? (cpu->status | c):(cpu->status & ~c));
    cpu->status = ((temp == 0) ? (cpu->status | z):(cpu->status & ~z));
    cpu->status = ((temp & n ) ? (cpu->status | n) : (cpu->status & ~n));
    return 1;
}

int cpx(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    uint8_t temp = cpu->x - operand;
    cpu->status = ((cpu->x >= operand)? (cpu->status | c):(cpu->status & ~c));
    cpu->status = ((temp == 0) ? (cpu->status | z):(cpu->status & ~z));
    cpu->status = ((temp & n ) ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int cpy(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    uint8_t temp = cpu->y - operand;
    cpu->status = ((cpu->y >= operand)? (cpu->status | c):(cpu->status & ~c));
    cpu->status = ((temp == 0) ? (cpu->status | z):(cpu->status & ~z));
    cpu->status = ((temp & n ) ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

/* Jump Operations */
int brk(struct cpu_state *cpu) {
    cpu->pc++;
    cpu->status |= i | b;
    push16(cpu, cpu->pc);
    push(cpu, cpu->status);
    cpu->status &= ~b;
    cpu->pc = read16(0xFFFE);
    return 0;
}

int rti(struct cpu_state *cpu) {
    cpu->status = pop(cpu);
    cpu->pc = pop16(cpu);
    cpu->status &= ~b;
    cpu->status |= u;
    return 0;
}

int jmp(struct cpu_state *cpu) {
    cpu->pc = cpu->opAddress;
    return 0;   
}

int jsr(struct cpu_state *cpu) {
    cpu->pc--;
    push16(cpu, cpu->pc);
    cpu->pc = cpu->opAddress;
    return 0;
}

int rts(struct cpu_state *cpu) {
    cpu->pc = pop16(cpu);
    cpu->pc++;
    return 0;
}

/* Arithmetic Operations */
int adc(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    uint16_t temp = cpu->acc + operand + (cpu->status & c);
    cpu->status = ((temp > 0xFF) ? (cpu->status | c) : (cpu->status & ~c));
    cpu->status = ((temp & 0xFF) ? (cpu->status &~ z) : (cpu->status | z));
    cpu->status = ((((~(cpu->acc ^ operand)) & (cpu->acc ^ temp)) & n) ? 
                    (cpu->status | v) : (cpu->status & ~v));
    cpu->status = ((temp & n   ) ? (cpu->status | n) : (cpu->status & ~n));
    cpu->acc = temp & 0xFF;
    return 1;
}

int sbc(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    operand ^= 0xFF;
    uint16_t temp = cpu->acc + operand + (cpu->status & c);
    cpu->status = ((temp > 0xFF) ? (cpu->status | c) : (cpu->status & ~c));
    cpu->status = ((temp & 0xFF) ? (cpu->status &~ z) : (cpu->status | z));
    cpu->status = ((((~(cpu->acc ^ operand)) & (cpu->acc ^ temp)) & n) ? 
                    (cpu->status | v) : (cpu->status & ~v));
    cpu->status = ((temp & n   ) ? (cpu->status | n) : (cpu->status & ~n));
    cpu->acc = temp & 0xFF;
    return 1;
}

/* Memory Operations */
int lda(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->acc = operand;
    cpu->status = ( (cpu->acc == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->acc & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 1;
}

int sta(struct cpu_state *cpu) {
    cpu_write(cpu->opAddress, cpu->acc);
    return 0;
}

int ldx(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->x = operand;
    cpu->status = ( (cpu->x == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->x & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 1;
}

int stx(struct cpu_state *cpu) {
    cpu_write(cpu->opAddress, cpu->x);
    return 0;
}

int ldy(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    cpu->y = operand;
    cpu->status = ( (cpu->y == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->y & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 1;
}

int sty(struct cpu_state *cpu) {
    cpu_write(cpu->opAddress, cpu->y);
    return 0;
}

int inc(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    operand++;
    cpu->status = ( (operand == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (operand & n)  ? (cpu->status | n) : (cpu->status & ~n));
    cpu_write(cpu->opAddress, operand);
    return 0;
}

int dec(struct cpu_state *cpu) {
    uint8_t operand = fetchOperand(cpu);
    operand--;
    cpu->status = ( (operand == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (operand & n)  ? (cpu->status | n) : (cpu->status & ~n));
    cpu_write(cpu->opAddress, operand);
    return 0;
}

/* Register Operations */
int tax(struct cpu_state *cpu) {
    cpu->x = cpu->acc;
    cpu->status = ( (cpu->x == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->x & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int txa(struct cpu_state *cpu) {
    cpu->acc = cpu->x;
    cpu->status = ( (cpu->acc == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->acc & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int tay(struct cpu_state *cpu) {
    cpu->y = cpu->acc;
    cpu->status = ( (cpu->y == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->y & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int tya(struct cpu_state *cpu) {
    cpu->acc = cpu->y;
    cpu->status = ( (cpu->acc == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->acc & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int txs(struct cpu_state *cpu) {
    cpu->sp = cpu->x;
    return 0;
}

int tsx(struct cpu_state *cpu) {
    cpu->x = cpu->sp;
    cpu->status = ( (cpu->x == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->x & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int inx(struct cpu_state *cpu) {
    cpu->x++;
    cpu->status = ( (cpu->x == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->x & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int dex(struct cpu_state *cpu) {
    cpu->x--;
    cpu->status = ( (cpu->x == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->x & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int iny(struct cpu_state *cpu) {
    cpu->y++;
    cpu->status = ( (cpu->y == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->y & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int dey(struct cpu_state *cpu) {
    cpu->y--;
    cpu->status = ( (cpu->y == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->y & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

/* Stack Operations */
int pha(struct cpu_state *cpu) {
    push(cpu, cpu->acc);
    return 0;
}

int pla(struct cpu_state *cpu) {
    cpu->acc = pop(cpu);
    cpu->status = ( (cpu->acc == 0) ? (cpu->status | z) : (cpu->status & ~z));
    cpu->status = ( (cpu->acc & n)  ? (cpu->status | n) : (cpu->status & ~n));
    return 0;
}

int php(struct cpu_state *cpu) {
    cpu->status |= u;
    push(cpu, cpu->status);
    cpu->status &= ~b;
    return 0;
}

int plp(struct cpu_state *cpu) {
    cpu->status = pop(cpu);
    cpu->status |= u;
    cpu->status &= ~b;
    return 0;
}

/* Flag Operations */
int clc(struct cpu_state *cpu) {
    cpu->status &= ~c;
    return 0;
}

int sec(struct cpu_state *cpu) {
    cpu->status |= c;
    return 0;
}

int cli(struct cpu_state *cpu) {
    cpu->status &= ~i;
    return 0;
}

int sei(struct cpu_state *cpu) {
    cpu->status |= i;
    return 0;
}

int clv(struct cpu_state *cpu) {
    cpu->status &= ~v;
    return 0;
}

int cld(struct cpu_state *cpu) {
    cpu->status &= ~d;
    return 0;
}

int sed(struct cpu_state *cpu) {
    cpu->status |= d;
    return 0;
}

/* Branch Operations */
int bra(struct cpu_state *cpu) {
    uint8_t flag;
    if (cpu->opcode & (1 << 7))
        flag = ( (cpu->opcode & (1 << 6)) ? z : c);
    else
        flag = ( (cpu->opcode & (1 << 6)) ? v : n);

    if ((!!(cpu->status & flag)) == (!!(cpu->opcode & (1 << 5)))) {
        cpu->pc = cpu->opAddress;
        cpu->cycles++;
        return 1;
    }
    return 0;
}

/* NOP and Illegal Opcodes */
int nop(struct cpu_state *cpu) {
    return 0;
}

int ill(struct cpu_state *cpu) {
    return 0;
}

/* Interrupts */
void nmi(struct cpu_state *cpu) {
    push16(cpu, cpu->pc);
    cpu->status &= ~b;
    cpu->status |= u | i;
    push(cpu, cpu->status);
    cpu->pc = read16(0xFFFA);
}

struct cpu_state cpu_init() {
    struct cpu_state cpu = {0};
    cpu.pc = read16(0xFFFC);
    cpu.status = 0x34;
    cpu.sp = 0xFD;
    // logfile = fopen("./logs/log.txt", "w+");
    return cpu;
}

void cpu_clock(struct cpu_state *cpu) {
    if (cpu->cycles == 0) {
        if (cpu->pendingNMI > 0) {
            cpu->pendingNMI = 0;
            cpu->cycles = 7;
            nmi(cpu);
        }
        else {
            cpu->opcode = cpu_read(cpu->pc++);
            struct Instruction op = opcodes[cpu->opcode];
            op.mode(cpu);
            op.op(cpu);
            cpu->cycles += op.cycles;

        //     switch (opcodes[cpu->opcode].size) {
        //         case 1 :
        //             fprintf(logfile, "%04X %02X       - %c%c%c (%c%c%c) - %02X %02X %02X %02X %02X\n",
        //                     cpu->pc, cpu->opcode, op.operation[0], op.operation[1], op.operation[2],
        //                     op.addressingMode[0], op.addressingMode[1], op.addressingMode[2], 
        //                     cpu->acc, cpu->x, cpu->y, cpu->sp, cpu->status);
        //             break;
        //         case 2 :
        //             fprintf(logfile, "%04X %02X %02X    - %c%c%c (%c%c%c) - %02X %02X %02X %02X %02X\n",
        //                     cpu->pc, cpu->opcode, cpu->byte2,
        //                     op.operation[0], op.operation[1], op.operation[2],
        //                     op.addressingMode[0], op.addressingMode[1], op.addressingMode[2], 
        //                     cpu->acc, cpu->x, cpu->y, cpu->sp, cpu->status);
        //             break;
        //         case 3 :
        //             fprintf(logfile, "%04X %02X %02X %02X - %c%c%c (%c%c%c) - %02X %02X %02X %02X %02X\n",
        //                     cpu->pc, cpu->opcode, cpu->byte2, cpu->byte3,
        //                     op.operation[0], op.operation[1], op.operation[2],
        //                     op.addressingMode[0], op.addressingMode[1], op.addressingMode[2], 
        //                     cpu->acc, cpu->x, cpu->y, cpu->sp, cpu->status);
        //             break;
        //     }

        // fflush(logfile);
        }
    }
    cpu->cycles--;
}