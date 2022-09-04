#include <stdint.h>
#include "cpu.h"
#include "memory.h"

#define Debug

#ifdef Debug
#include <stdio.h>
#endif

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

int pageCheck(uint16_t address, uint8_t offset) {
    return ( (((address & 0x00FF) + offset) & 0xFF00) ? 1 : 0);
}

void setFlags(struct cpu_state *cpu, uint8_t result, uint8_t mask) {
    if (mask & n)
        cpu->status = ((result & n) ? cpu->status | n : cpu->status & ~n);
    if (mask & z)
        cpu->status = ((result == 0) ? cpu->status | z : cpu->status & ~z);
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

// Addressing Modes
int imm(struct cpu_state *cpu) {
    cpu->opAddress = cpu->pc++; 
    return 0;
}

int aba(struct cpu_state *cpu) {
    cpu->opAddress = read16(cpu->pc++); 
    cpu->pc++; 
    return 0;
}

int abx(struct cpu_state *cpu) {
    aba(cpu); 
    uint16_t temp = cpu->opAddress; 
    cpu->opAddress += cpu->x; 
    return pageCheck(temp, cpu->x);
}

int aby(struct cpu_state *cpu) {
    aba(cpu); 
    uint16_t temp = cpu->opAddress; 
    cpu->opAddress += cpu->y; 
    return pageCheck(temp, cpu->y);
}

int zpa(struct cpu_state *cpu) {
    cpu->opAddress = cpu_read(cpu->pc++); 
    return 0;
}

int zpx(struct cpu_state *cpu) {
    cpu->opAddress = ((cpu_read(cpu->pc++) + cpu->x)& 0x00FF); 
    return 0;
}

int zpy(struct cpu_state *cpu) {
    cpu->opAddress = ((cpu_read(cpu->pc++) + cpu->y)& 0x00FF); 
    return 0;
}

int ind(struct cpu_state *cpu) {
    cpu->opAddress = read16wrap(cpu->opAddress); 
    return 0;
}

int idx(struct cpu_state *cpu) {
    cpu->opAddress = read16wrap((cpu_read(cpu->pc++) + cpu->x) & 0x00FF); 
    return 0;
}

int idy(struct cpu_state *cpu) {
    uint16_t temp = read16wrap(cpu_read(cpu->pc++));
    cpu->opAddress = temp + cpu->y;
    return pageCheck(temp, cpu->y);
}

int rel(struct cpu_state *cpu) {
    uint8_t temp = cpu_read(cpu->pc++);
    cpu->opAddress = ( (temp & 0x80) ? cpu->pc - 128 : cpu->pc);
    cpu->opAddress += (temp & 0x7F);
    return ( ((cpu->opAddress & 0xFF00) != (cpu->pc & 0xFF00)) ? 1 : 0);
}

// Group One Instructions
int ora(struct cpu_state *cpu) {
    cpu->acc |= cpu_read(cpu->opAddress);
    setFlags(cpu, cpu->acc, z | n);
    return 1;
}

int and(struct cpu_state *cpu) {
    cpu->acc &= cpu_read(cpu->opAddress);
    setFlags(cpu, cpu->acc, z | n);
    return 1;
}

int eor(struct cpu_state *cpu) {
    cpu->acc ^= cpu_read(cpu->opAddress);
    setFlags(cpu, cpu->acc, z | n);
    return 1;
}

int adc(struct cpu_state *cpu) {
    uint8_t operand = cpu_read(cpu->opAddress);
    uint16_t temp = cpu->acc + operand + (cpu->status & c);
    setFlags(cpu, (temp & 0x00FF), z | n);
    if (temp > 0x00FF)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    if ( (((~(cpu->acc ^ operand)) & (cpu->acc ^ temp)) & n) )
        cpu->status |= v;
    else
        cpu->status &= ~v;
    cpu->acc = temp & 0x00FF;
    return 1;
}

int sta(struct cpu_state *cpu) {
    cpu_write(cpu->opAddress, cpu->acc);
    return 0;
}

int lda(struct cpu_state *cpu) {
    cpu->acc = cpu_read(cpu->opAddress);
    setFlags(cpu, cpu->acc, z | n);
    return 1;
}

int cmp(struct cpu_state *cpu) {
    uint8_t op = cpu_read(cpu->opAddress);
    if (cpu->acc >= op)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    setFlags(cpu, (cpu->acc - op), z | n);
    return 1;
}

int sbc(struct cpu_state *cpu) {
    uint8_t operand = ~cpu_read(cpu->opAddress);
    uint16_t temp = cpu->acc + operand + (cpu->status & c);
    setFlags(cpu, (temp & 0x00FF), z | n);
    if (temp > 0x00FF)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    if ( (((~(cpu->acc ^ operand)) & (cpu->acc ^ temp)) & n) )
        cpu->status |= v;
    else
        cpu->status &= ~v;
    cpu->acc = temp & 0x00FF;
    return 1;
}

// Group Two Instructions
int asl(struct cpu_state *cpu, int useAddress) {
    uint8_t operand = ( (useAddress) ? cpu_read(cpu->opAddress) : cpu->acc);
    if (operand & n)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    operand <<= 1;
    setFlags(cpu, operand, z | n);
    if (useAddress)
        cpu_write(cpu->opAddress, operand);
    else
        cpu->acc = operand;
    return 1;
}

int rol(struct cpu_state *cpu, int useAddress) {
    uint8_t operand = ( (useAddress) ? cpu_read(cpu->opAddress) : cpu->acc);
    int rolledBit = cpu->status & c;
    if (operand & n)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    operand = (operand << 1) | rolledBit;
    setFlags(cpu, operand, z | n);
    if (useAddress)
        cpu_write(cpu->opAddress, operand);
    else
        cpu->acc = operand;
    return 0;
}

int lsr(struct cpu_state *cpu, int useAddress) {
    uint8_t operand = ( (useAddress) ? cpu_read(cpu->opAddress) : cpu->acc);
    if (operand & c)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    operand >>= 1;
    setFlags(cpu, operand, z | n);
    if (useAddress)
        cpu_write(cpu->opAddress, operand);
    else
        cpu->acc = operand;
    return 1;
}

int ror(struct cpu_state *cpu, int useAddress) {
    uint8_t operand = ( (useAddress) ? cpu_read(cpu->opAddress) : cpu->acc);
    int rolledBit = cpu->status & c;
    if (operand & c)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    operand = (operand >> 1) | (rolledBit << 7);
    setFlags(cpu, operand, z | n);
    if (useAddress)
        cpu_write(cpu->opAddress, operand);
    else
        cpu->acc = operand;
    return 0;
}

int stx(struct cpu_state *cpu) {
    cpu_write(cpu->opAddress, cpu->x);
    return 0;
}

int ldx(struct cpu_state *cpu) {
    cpu->x = cpu_read(cpu->opAddress);
    setFlags(cpu, cpu->x, z | n);
    return 1;
}

int dec(struct cpu_state *cpu) {
    uint8_t temp = cpu_read(cpu->opAddress);
    setFlags(cpu, --temp, z | n);
    cpu_write(cpu->opAddress, temp);
    return 0;
}

int inc(struct cpu_state *cpu) {
    uint8_t temp = cpu_read(cpu->opAddress);
    setFlags(cpu, ++temp, z | n);
    cpu_write(cpu->opAddress, temp);
    return 0;
}

// Group Three Instructions
int bit(struct cpu_state *cpu) {
    uint8_t operand = cpu_read(cpu->opAddress);
    setFlags(cpu, operand, n);
    if (operand & 0x40)
        cpu->status |= v;
    else
        cpu->status &= ~v;
    setFlags(cpu, operand & cpu->acc, z);
    return 0;
}

int jmp(struct cpu_state *cpu) {
    cpu->pc = cpu->opAddress;
    return 0;
}

int sty(struct cpu_state *cpu) {
    cpu_write(cpu->opAddress, cpu->y);
    return 0;
}

int ldy(struct cpu_state *cpu) {
    cpu->y = cpu_read(cpu->opAddress);
    setFlags(cpu, cpu->y, z | n);
    return 1;
}

int cpy(struct cpu_state *cpu) {
    uint8_t op = cpu_read(cpu->opAddress);
    if (cpu->y >= op)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    setFlags(cpu, (cpu->y - op), z | n);
    return 0;
}

int cpx(struct cpu_state *cpu) {
    uint8_t op = cpu_read(cpu->opAddress);
    if (cpu->x >= op)
        cpu->status |= c;
    else
        cpu->status &= ~c;
    setFlags(cpu, (cpu->x - op), z | n);
    return 0;
}

int brk(struct cpu_state *cpu) {
    cpu->pc++;
    cpu->status |= (i | b);
    push16(cpu, cpu->pc);
    push(cpu, cpu->status);
    cpu->status &= ~b;
    cpu->pc = read16(0xFFFE);
    return 0;
}

int jsr(struct cpu_state *cpu) {
    cpu->pc--;
    push16(cpu, cpu->pc);
    cpu->pc = cpu->opAddress;
    return 0;
}

int rti(struct cpu_state *cpu) {
    cpu->status = pop(cpu);
    cpu->pc = pop16(cpu);
    cpu->status &= ~b;
    cpu->status |= u;
    return 0;
}

int rts(struct cpu_state *cpu) {
    cpu->pc = pop16(cpu);
    cpu->pc++;
    return 0;
}

int branch(struct cpu_state *cpu, uint8_t opcode) {
    uint8_t flag;
    if (opcode & (1 << 7))
        flag = ( (opcode & (1 << 6)) ? z : c);
    else
        flag = ( (opcode & (1 << 6)) ? v : n);

    if ((!!(cpu->status & flag)) == (!!(opcode & (1 << 5)))) {
        cpu->pc = cpu->opAddress;
        cpu->cycles++;
        return 1;
    }
    return 0;
}


struct cpu_state cpu_init() {
    struct cpu_state cpu = {0};
    cpu.pc = (cpu_read(0xFFFD) << 8) | (cpu_read(0xFFFC));
    cpu.status = 0x34;
    cpu.sp = 0xFD;
    return cpu;
}

void execute_instruction(struct cpu_state *cpu){
    uint8_t opcode = cpu_read(cpu->pc++);
    int oopsCycle = 0;
    int accOrAddress = 1;

    printf("%02X\n", opcode);

    switch (opcode) {
        case 0x00 :case 0x40 :case 0x60 :case 0x02 :case 0x12 :case 0x22 :
        case 0x32 :case 0x42 :case 0x52 :case 0x62 :case 0x72 :case 0x92 :
        case 0xB2 :case 0xD2 :case 0xF2 :case 0x08 :case 0x18 :case 0x28 :
        case 0x38 :case 0x48 :case 0x58 :case 0x68 :case 0x78 :case 0x88 :
        case 0x98 :case 0xA8 :case 0xB8 :case 0xC8 :case 0xD8 :case 0xE8 :
        case 0xF8 :case 0x1A :case 0x3A :case 0x5A :case 0x7A :case 0x8A :
        case 0x9A :case 0xAA :case 0xBA :case 0xCA :case 0xDA :case 0xEA :
        case 0xFA :
            cpu->cycles += 2;
            break;
        case 0x0A :case 0x2A :case 0x4A :case 0x6A :
            accOrAddress = 0;
            break;
        case 0x80 :case 0xA0 :case 0xC0 :case 0xE0 :case 0x82 :case 0xA2 :
        case 0xC2 :case 0xE2 :case 0x09 :case 0x29 :case 0x49 :case 0x69 :
        case 0x89 :case 0xA9 :case 0xC9 :case 0xE9 :case 0x0B :case 0x2B :
        case 0x4B :case 0x6B :case 0x8B :case 0xAB :case 0xCB :case 0xEB :
            imm(cpu);
            cpu->cycles += 2;
            break;
        case 0x20 :case 0x0C :case 0x2C :case 0x4C :case 0x8C :case 0xAC :
        case 0xCC :case 0xEC :case 0x0D :case 0x2D :case 0x4D :case 0x6D :
        case 0x8D :case 0xAD :case 0xCD :case 0xED :case 0x0E :case 0x2E :
        case 0x4E :case 0x6E :case 0x8E :case 0xAE :case 0xCE :case 0xEE :
        case 0x0F :case 0x2F :case 0x4F :case 0x6F :case 0x8F :case 0xAF :
        case 0xCF :case 0xEF :
            aba(cpu);
            cpu->cycles += 4;
            break;
        case 0x1C :case 0x3C :case 0x5C :case 0x7C :case 0x9C :case 0xBC :
        case 0xDC :case 0xFC :case 0x1D :case 0x3D :case 0x5D :case 0x7D :
        case 0x9D :case 0xBD :case 0xDD :case 0xFD :case 0x1E :case 0x3E :
        case 0x5E :case 0x7E :case 0xDE :case 0xFE :case 0x1F :case 0x3F :
        case 0x5F :case 0x7F :case 0x9F :case 0xDF :case 0xFF :
            oopsCycle += abx(cpu);
            cpu->cycles += 4;
            break;
        case 0x19 :case 0x39 :case 0x59 :case 0x79 :case 0x99 :case 0xB9 :
        case 0xD9 :case 0xF9 :case 0x1B :case 0x3B :case 0x5B :case 0x7B :
        case 0x9B :case 0xBB :case 0xDB :case 0xFB :case 0x9E :case 0xBE :
        case 0xBF :
            oopsCycle += aby(cpu);
            cpu->cycles += 4;
            break;
        case 0x04 :case 0x24 :case 0x44 :case 0x64 :case 0x84 :case 0xA4 :
        case 0xC4 :case 0xE4 :case 0x05 :case 0x25 :case 0x45 :case 0x65 :
        case 0x85 :case 0xA5 :case 0xC5 :case 0xE5 :case 0x06 :case 0x26 :
        case 0x46 :case 0x66 :case 0x86 :case 0xA6 :case 0xC6 :case 0xE6 :
        case 0x07 :case 0x27 :case 0x47 :case 0x67 :case 0x87 :case 0xA7 :
        case 0xC7 :case 0xE7 :
            zpa(cpu);
            cpu->cycles += 3;
            break;
        case 0x14 :case 0x34 :case 0x54 :case 0x74 :case 0x94 :case 0xB4 :
        case 0xD4 :case 0xF4 :case 0x15 :case 0x35 :case 0x55 :case 0x75 :
        case 0x95 :case 0xB5 :case 0xD5 :case 0xF5 :case 0x16 :case 0x36 :
        case 0x56 :case 0x76 :case 0xD6 :case 0xF6 :case 0x17 :case 0x37 :
        case 0x57 :case 0x77 :case 0xD7 :case 0xF7 :
            zpx(cpu);
            cpu->cycles += 4;
            break;
        case 0x96 :case 0xB6 :case 0x97 :case 0xB7 :
            zpy(cpu);
            cpu->cycles += 4;
            break;
        case 0x6C :
            ind(cpu);
            break;
        case 0x01 :case 0x21 :case 0x41 :case 0x61 :case 0x81 :case 0xA1 :
        case 0xC1 :case 0xE1 :case 0x03 :case 0x23 :case 0x43 :case 0x63 :
        case 0x83 :case 0xA3 :case 0xC3 :case 0xE3 :
            idx(cpu);
            cpu->cycles += 6;
            break;
        case 0x11 :case 0x31 :case 0x51 :case 0x71 :case 0x91 :case 0xB1 :
        case 0xD1 :case 0xF1 :case 0x13 :case 0x33 :case 0x53 :case 0x73 :
        case 0x93 :case 0xB3 :case 0xD3 :case 0xF3 :
            oopsCycle += idy(cpu);
            cpu->cycles += 5;
            break;
        case 0x10 :case 0x30 :case 0x50 :case 0x70 :case 0x90 :case 0xB0 :
        case 0xD0 :case 0xF0 :
            oopsCycle += rel(cpu);
            break;
    }

    switch (opcode) {
        case 0x61 :case 0x71 :case 0x65 :case 0x75 :case 0x69 :case 0x79 :
        case 0x6D :case 0x7D :
            oopsCycle += adc(cpu);
            break;
        case 0x21 :case 0x31 :case 0x25 :case 0x35 :case 0x29 :case 0x39 :
        case 0x2D :case 0x3D :
            oopsCycle += and(cpu);
            break;
        case 0x06 :case 0x16 :case 0x0A :case 0x0E :case 0x1E :
            oopsCycle += asl(cpu, accOrAddress);
            break;
        case 0x10 :case 0x30 :case 0x50 :case 0x70 :case 0x90 :case 0xB0 :
        case 0xD0 :case 0xF0 :
            oopsCycle += branch(cpu, opcode);
            break;
        case 0x24 :case 0x2C :
            bit(cpu);
            break;
        case 0x00 :
            brk(cpu);
            break;
        case 0x18 :
            cpu->status &= ~c;
            break;
        case 0xD8 :
            cpu->status &= ~d;
            break;
        case 0x58 :
            cpu->status &= ~i;
            break;
        case 0xB8 :
            cpu->status &= ~v;
            break;
        case 0xC1 :case 0xD1 :case 0xC5 :case 0xD5 :case 0xC9 :case 0xD9 :
        case 0xCD :case 0xDD :
            oopsCycle += cmp(cpu);
            break;
        case 0xE0 :case 0xE4 :case 0xEC :
            cpx(cpu);
            break;
        case 0xC0 :case 0xC4 :case 0xCC :
            cpy(cpu);
            break;
        case 0xC6 :case 0xD6 :case 0xCE :case 0xDE :
            dec(cpu);
            break;
        case 0xCA :
            cpu->x--;
            break;
        case 0x88 :
            cpu->y--;
            break;
        case 0x41 :case 0x51 :case 0x45 :case 0x55 :case 0x49 :case 0x59 :
        case 0x4D :case 0x5D :
            oopsCycle += eor(cpu);
            break;
        case 0xE6 :case 0xF6 :case 0xEE :case 0xFE :
            inc(cpu);
            break;
        case 0xE8 :
            cpu->x++;
            break;
        case 0xC8 :
            cpu->y++;
            break;
        case 0x4C :case 0x6C :
            jmp(cpu);
            break;
        case 0x20 :
            jsr(cpu);
            break;
        case 0xA1 :case 0xB1 :case 0xA5 :case 0xB5 :case 0xA9 :case 0xB9 :
        case 0xAD :case 0xBD :
            oopsCycle += lda(cpu);
            break;
        case 0xA2 :case 0xA6 :case 0xB6 :case 0xAE :case 0xBE :
            oopsCycle += ldx(cpu);
            break;
        case 0xA0 :case 0xA4 :case 0xB4 :case 0xAC :case 0xBC :
            oopsCycle += ldy(cpu);
            break;
        case 0x46 :case 0x56 :case 0x4A :case 0x4E :case 0x5E :
            oopsCycle += lsr(cpu, accOrAddress);
            break;
        case 0x01 :case 0x11 :case 0x05 :case 0x15 :case 0x09 :case 0x19 :
        case 0x0D :case 0x1D :
            oopsCycle += ora(cpu);
            break;
        case 0x48 :
            push(cpu, cpu->acc);
            break;
        case 0x08 :
            push(cpu, cpu->status);
            break;
        case 0x68 :
            cpu->acc = pop(cpu);
            break;
        case 0x28 :
            cpu->status = pop(cpu);
            break;
        case 0x26 :case 0x36 :case 0x2A :case 0x2E :case 0x3E :
            rol(cpu, accOrAddress);
            break;
        case 0x66 :case 0x76 :case 0x6A :case 0x6E :case 0x7E :
            ror(cpu, accOrAddress);
            break;
        case 0x40 :
            rti(cpu);
            break;
        case 0x60 :
            rts(cpu);
            break;
        case 0xE1 :case 0xF1 :case 0xE5 :case 0xF5 :case 0xE9 :case 0xF9 :
        case 0xED :case 0xFD :
            oopsCycle += sbc(cpu);
            break;
        case 0x38 :
            cpu->status |= c;
            break;
        case 0xF8 :
            cpu->status |= d;
            break;
        case 0x78 :
            cpu->status |= i;
            break;
        case 0x81 :case 0x91 :case 0x85 :case 0x95 :case 0x99 :case 0x8D :
        case 0x9D :
            sta(cpu);
            break;
        case 0x86 :case 0x96 :case 0x8E :
            stx(cpu);
            break;
        case 0x84 :case 0x94 :case 0x8C :
            sty(cpu);
            break;
        case 0xAA :
            cpu->x = cpu->acc;
            break;
        case 0xA8 :
            cpu->y = cpu->acc;
            break;
        case 0xBA :
            cpu->x = cpu->sp;
            break;
        case 0x8A :
            cpu->acc = cpu->x;
            break;
        case 0x9A :
            cpu->sp = cpu->x;
            break;
        case 0x98 :
            cpu->acc = cpu->y;
            break;
    }
}

void cpu_clock(struct cpu_state *cpu) {
    if (cpu->cycles == 0) {
        execute_instruction(cpu);
    }
    cpu->cycles--;
}