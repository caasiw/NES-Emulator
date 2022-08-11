#include <stdint.h>
#include "memory.h"

#define c (1 << 0)
#define z (1 << 1)
#define i (1 << 2)
#define d (1 << 3)
#define b (1 << 4)
#define u (1 << 5)
#define v (1 << 6)
#define n (1 << 7)

struct cpu_state {
    uint16_t pc, opAddress;
    uint8_t acc, x, y, sp, status;
    int cycles;
};

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

int inx(struct cpu_state *cpu) {
    cpu->opAddress = read16wrap((cpu_read(cpu->pc++) + cpu->x) & 0x00FF); 
    return 0;
}

int iny(struct cpu_state *cpu) {
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
    cpu->status = ( (temp > 0x00FF) ? (cpu->status |= c) : (cpu->status &= ~c));
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
    cpu->status = ((cpu->acc >= op) ? (cpu->status |= c) : (cpu->status &= ~c));
    setFlags(cpu, (cpu->acc - op), z | n);
    return 1;
}

int sbc(struct cpu_state *cpu) {
    uint8_t operand = ~cpu_read(cpu->opAddress);
    uint16_t temp = cpu->acc + operand + (cpu->status & c);
    setFlags(cpu, (temp & 0x00FF), z | n);
    cpu->status = ( (temp > 0x00FF) ? (cpu->status |= c) : (cpu->status &= ~c));
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
    cpu->status = ( (operand & n) ? (cpu->status |= c) : (cpu->status &= ~c));
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
    cpu->status = ( (operand & n) ? (cpu->status |= c) : (cpu->status &= ~c));
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
    cpu->status = ( (operand & c) ? (cpu->status |= c) : (cpu->status &= ~c));
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
    cpu->status = ( (operand & c) ? (cpu->status |= c) : (cpu->status &= ~c));
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
    cpu->status = ((operand & 0x40) ? (cpu->status |= v) : (cpu->status &= ~v));
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
    cpu->status = ((cpu->y >= op) ? (cpu->status |= c) : (cpu->status &= ~c));
    setFlags(cpu, (cpu->y - op), z | n);
    return 0;
}

int cpx(struct cpu_state *cpu) {
    uint8_t op = cpu_read(cpu->opAddress);
    cpu->status = ((cpu->x >= op) ? (cpu->status |= c) : (cpu->status &= ~c));
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
}

int jsr(struct cpu_state *cpu) {
    cpu->pc--;
    push16(cpu, cpu->pc);
    cpu->pc = cpu->opAddress;
}

int rti(struct cpu_state *cpu) {
    cpu->status = pop(cpu);
    cpu->pc = pop16(cpu);
    cpu->status &= ~b;
    cpu->status |= u;
}

int rts(struct cpu_state *cpu) {
    cpu->pc = pop16(cpu);
    cpu->pc++;
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
    return cpu;
}

void execute_instruction(struct cpu_state *cpu){
    uint8_t opcode = cpu_read(cpu->pc++);
    uint8_t aaa = opcode >> 5;
    uint8_t bbb = (opcode >> 2) & 0x07;
    uint8_t cc  = opcode & 0x03;

    switch (cc) {
        case 1 : // Group One Opcodes (aaabbb01)
            switch (bbb) {
                case 0 :
                    inx(cpu);
                    break;
                case 1 :
                    zpa(cpu);
                    break;
                case 2 :
                    imm(cpu);
                    break;
                case 3 :
                    aba(cpu);
                    break;
                case 4 :
                    iny(cpu);
                    break;
                case 5 :
                    zpx(cpu);
                    break;
                case 6 :
                    aby(cpu);
                    break;
                case 7 :
                    abx(cpu);
                    break;
            }
            switch (aaa) {
                case 0 :
                    ora(cpu);
                    break;
                case 1 :
                    and(cpu);
                    break;
                case 2 :
                    eor(cpu);
                    break;
                case 3 :
                    adc(cpu);
                    break;
                case 4 :
                    if (bbb != 2)
                        sta(cpu);
                    break;
                case 5 : 
                    lda(cpu);
                    break;
                case 6 :
                    cmp(cpu);
                    break;
                case 7 :
                    sbc(cpu);
                    break;
            }
            break;
        case 2 :    // Group Two Opcodes (aaabbb10)
            switch (bbb) {
                case 0 :
                    if (aaa > 3)
                        imm(cpu);
                    break;
                case 1 :
                    zpa(cpu);
                    break;
                case 3 :
                    aba(cpu);
                    break;
                case 5 :
                    if ((aaa == 4) || (aaa == 5))
                        zpy(cpu);
                    else
                        zpx(cpu);
                    break;
                case 7 :
                    if ((aaa == 4) || (aaa == 5))
                        aby(cpu);
                    else
                        abx(cpu);
                    break;
            }
            switch (aaa) {
                case 0 :
                    if (bbb == 2)
                        asl(cpu, 0);
                    else if ((bbb != 0) && (bbb != 4) && (bbb != 6))
                        asl(cpu, 1);
                    break;
                case 1 :
                    if (bbb == 2)
                        rol(cpu, 0);
                    else if ((bbb != 0) && (bbb != 4) && (bbb != 6))
                        rol(cpu, 1);
                    break;
                case 2 :
                    if (bbb == 2)
                        lsr(cpu, 0);
                    else if ((bbb != 0) && (bbb != 4) && (bbb != 6))
                        lsr(cpu, 1);
                    break;
                case 3 :
                    if (bbb == 2)
                        ror(cpu, 0);
                    else if ((bbb != 0) && (bbb != 4) && (bbb != 6))
                        ror(cpu, 1);
                    break;
                case 4 :
                    if ((bbb == 1) || (bbb == 3) || (bbb == 5))
                        stx(cpu);
                    else if (bbb == 2)
                        cpu->acc = cpu->x;
                    else if (bbb == 6)
                        cpu->sp = cpu->x;
                    break;
                case 5 :
                    if (bbb == 2)
                        cpu->x = cpu->acc;
                    else if (bbb == 6)
                        cpu->x = cpu->sp;
                    else if (bbb != 4)
                        ldx(cpu);
                    break;
                case 6 :
                    if (bbb & 0x01)
                        dec(cpu);
                    else if (bbb == 2)
                        cpu->x--;
                    break;
                case 7 :
                    if (bbb & 0x01)
                        inc(cpu);
                    break;
            }
            break;
        case 0 : // Group Three Opcodes (aaabbb00)
            switch (bbb) {
                case 0 :
                    if (aaa > 3)
                        imm(cpu);
                    else if (aaa == 1)
                        aba(cpu);
                    break;
                case 1 :
                    zpa(cpu);
                    break;
                case 3 :
                    aba(cpu);
                    break;
                case 5 :
                    zpx(cpu);
                    break;
                case 7 :
                    abx(cpu);
                    break;
            }
            switch (aaa) {
                case 0 :
                    if (bbb == 0)
                        brk(cpu);
                    else if (bbb == 2)
                        push(cpu, cpu->status);
                    else if (bbb == 4)
                        branch(cpu, opcode);
                    else if (bbb == 6)
                        cpu->status &= ~c;
                    break;
                case 1 :
                    if (bbb == 0)
                        jsr(cpu);
                    else if (bbb == 2)
                        cpu->status = pop(cpu);
                    else if (bbb == 4)
                        branch(cpu, opcode);
                    else if (bbb == 6)
                        cpu->status |= c;
                    else if ((bbb == 1) || (bbb == 3))
                        bit(cpu);
                    break;
                case 2 :
                    if (bbb == 0)
                        rti(cpu);
                    else if (bbb == 2)
                        push(cpu, cpu->acc);
                    else if (bbb == 4)
                        branch(cpu, opcode);
                    else if (bbb == 6)
                        cpu->status &= ~i;
                    else if (bbb == 3)
                        jmp(cpu);
                    break;
                case 3 :
                    if (bbb == 0)
                        rts(cpu);
                    else if (bbb == 2)
                        cpu->acc = pop(cpu);
                    else if (bbb == 4)
                        branch(cpu, opcode);
                    else if (bbb == 6)
                        cpu->status |= i;
                    else if (bbb == 3) {
                        ind(cpu);
                        jmp(cpu);
                    }
                    break;
                case 4 :
                    if (bbb == 2)
                        cpu->y--;
                    else if (bbb == 4)
                        branch(cpu, opcode);
                    else if (bbb == 6)
                        cpu->acc = cpu->y;
                    else if ((bbb == 1) || (bbb == 3) || (bbb == 5))
                        sty(cpu);
                    break;
                case 5 :
                    if (((bbb % 2) == 1) || (bbb == 0))
                        ldy(cpu);
                    else if (bbb == 2)
                        cpu->y = cpu->acc;
                    else if (bbb == 4)
                        branch(cpu, opcode);
                    else if (bbb == 6)
                        cpu->status &= ~v;
                    break;
                case 6 :
                    if (bbb == 2)
                        cpu->y++;
                    else if (bbb == 4)
                        branch(cpu, opcode);
                    else if (bbb == 6)
                        cpu->status &= ~d;
                    else if ((bbb == 0) || (bbb == 1) || (bbb == 3))
                        cpy(cpu);
                    break;
                case 7 :
                    if (bbb == 2)
                        cpu->x++;
                    else if (bbb == 4)
                        branch(cpu, opcode);
                    else if (bbb == 6)
                        cpu->status |= d;
                    else if ((bbb == 0) || (bbb == 1) || (bbb == 3))
                        cpx(cpu);
                    break;
            }
            break;
        case 3 :
            switch (bbb) {
                case 0 :
                    inx(cpu);
                    break;
                case 1 :
                    zpa(cpu);
                    break;
                case 2 :
                    imm(cpu);
                    break;
                case 3 :
                    aba(cpu);
                    break;
                case 4 :
                    iny(cpu);
                    break;
                case 5 :
                    if ((aaa == 4) || (aaa == 5))
                        zpy(cpu);
                    else    
                        zpx(cpu);
                    break;
                case 6 :
                    aby(cpu);
                    break;
                case 7 :
                    if ((aaa == 4) || (aaa == 5))
                        aby(cpu);
                    else 
                        abx(cpu);
                    break;
            }
            break;
    }
}

void cpu_clock(struct cpu_state *cpu) {

}