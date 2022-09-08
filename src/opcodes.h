#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

int imp(struct cpu_state *cpu);
int acc(struct cpu_state *cpu);
int imm(struct cpu_state *cpu);
int zpa(struct cpu_state *cpu);
int zpx(struct cpu_state *cpu);
int zpy(struct cpu_state *cpu);
int aba(struct cpu_state *cpu);
int abx(struct cpu_state *cpu);
int aby(struct cpu_state *cpu);
int ind(struct cpu_state *cpu);
int idx(struct cpu_state *cpu);
int idy(struct cpu_state *cpu);
int rel(struct cpu_state *cpu);

int and(struct cpu_state *cpu);
int eor(struct cpu_state *cpu);
int ora(struct cpu_state *cpu);
int asl(struct cpu_state *cpu);
int lsr(struct cpu_state *cpu);
int rol(struct cpu_state *cpu);
int ror(struct cpu_state *cpu);
int bit(struct cpu_state *cpu);
int cmp(struct cpu_state *cpu);
int cpx(struct cpu_state *cpu);
int cpy(struct cpu_state *cpu);
int brk(struct cpu_state *cpu);
int rti(struct cpu_state *cpu);
int jmp(struct cpu_state *cpu);
int jsr(struct cpu_state *cpu);
int rts(struct cpu_state *cpu);
int adc(struct cpu_state *cpu);
int sbc(struct cpu_state *cpu);
int lda(struct cpu_state *cpu);
int sta(struct cpu_state *cpu);
int ldx(struct cpu_state *cpu);
int stx(struct cpu_state *cpu);
int ldy(struct cpu_state *cpu);
int sty(struct cpu_state *cpu);
int inc(struct cpu_state *cpu);
int dec(struct cpu_state *cpu);
int tax(struct cpu_state *cpu);
int txa(struct cpu_state *cpu);
int tay(struct cpu_state *cpu);
int tya(struct cpu_state *cpu);
int txs(struct cpu_state *cpu);
int tsx(struct cpu_state *cpu);
int inx(struct cpu_state *cpu);
int dex(struct cpu_state *cpu);
int iny(struct cpu_state *cpu);
int dey(struct cpu_state *cpu);
int pha(struct cpu_state *cpu);
int pla(struct cpu_state *cpu);
int php(struct cpu_state *cpu);
int plp(struct cpu_state *cpu);
int clc(struct cpu_state *cpu);
int sec(struct cpu_state *cpu);
int cli(struct cpu_state *cpu);
int sei(struct cpu_state *cpu);
int clv(struct cpu_state *cpu);
int cld(struct cpu_state *cpu);
int sed(struct cpu_state *cpu);
int bra(struct cpu_state *cpu);
int nop(struct cpu_state *cpu);
int ill(struct cpu_state *cpu);

/* Opcode Lookup Table */
struct Instruction {
    uint8_t opcode;
    char addressingMode[3];
    char operation[3];
    int (*mode)(struct cpu_state *cpu);
    int (*op)(struct cpu_state *cpu);
    int cycles;
    int size;
};

const struct Instruction opcodes[256] = {
    {0x00,"IMP","BRK",&imp,&brk,7,1},{0x01,"IDX","ORA",&idx,&ora,6,2},
    {0x02,"IMM","JAM",&imp,&ill,0,2},{0x03,"IDX","SLO",&idx,&ill,0,2},
    {0x04,"ZPA","NOP",&zpa,&ill,0,2},{0x05,"ZPA","ORA",&zpa,&ora,3,2},
    {0x06,"ZPA","ASL",&zpa,&asl,5,2},{0x07,"ZPA","SLO",&zpa,&ill,0,2},
    {0x08,"IMP","PHP",&imp,&php,3,1},{0x09,"IMM","ORA",&imm,&ora,2,2},
    {0x0A,"ACC","ASL",&acc,&asl,2,1},{0x0B,"IMM","ANC",&imm,&ill,0,2},
    {0x0C,"ABA","NOP",&aba,&ill,0,3},{0x0D,"ABA","ORA",&aba,&ora,4,3},
    {0x0E,"ABA","ASL",&aba,&asl,6,3},{0x0F,"ABA","SLO",&aba,&ill,0,3},
    {0x10,"REL","BPL",&rel,&bra,2,2},{0x11,"IDY","ORA",&idy,&ora,5,2},
    {0x12,"IMM","JAM",&imm,&ill,0,2},{0x13,"IDY","SLO",&idy,&ill,0,2},
    {0x14,"ZPX","NOP",&zpx,&ill,0,2},{0x15,"ZPX","ORA",&zpx,&ora,4,2},
    {0x16,"ZPX","ASL",&zpx,&asl,6,2},{0x17,"ZPX","SLO",&zpx,&ill,0,2},
    {0x18,"IMP","CLC",&imp,&clc,2,1},{0x19,"ABY","ORA",&aby,&ora,4,3},
    {0x1A,"IMP","NOP",&imp,&ill,0,1},{0x1B,"ABY","SLO",&aby,&ill,0,3},
    {0x1C,"ABX","NOP",&abx,&ill,0,3},{0x1D,"ABX","ORA",&abx,&ora,4,3},
    {0x1E,"ABX","ASL",&abx,&asl,7,3},{0x1F,"ABX","SLO",&abx,&ill,0,3},
    {0x20,"ABA","JSR",&aba,&jsr,6,3},{0x21,"IDX","AND",&idx,&and,6,2},
    {0x22,"IMM","JAM",&imm,&ill,0,2},{0x23,"IDX","RLA",&idx,&ill,0,2},
    {0x24,"ZPA","BIT",&zpa,&bit,3,2},{0x25,"ZPA","AND",&zpa,&and,3,2},
    {0x26,"ZPA","ROL",&zpa,&rol,5,2},{0x27,"ZPA","RLA",&zpa,&ill,0,2},
    {0x28,"IMP","PLP",&imp,&plp,4,1},{0x29,"IMM","AND",&imm,&and,2,2},
    {0x2A,"ACC","ROL",&acc,&rol,2,1},{0x2B,"IMM","ANC",&imm,&ill,0,2},
    {0x2C,"ABA","BIT",&aba,&bit,4,3},{0x2D,"ABA","AND",&aba,&and,4,3},
    {0x2E,"ABA","ROL",&aba,&rol,6,3},{0x2F,"ABA","RLA",&aba,&ill,0,3},
    {0x30,"REL","BMI",&rel,&bra,2,2},{0x31,"IDY","AND",&idy,&and,5,2},
    {0x32,"IMM","JAM",&imm,&ill,0,2},{0x33,"IDY","RLA",&idy,&ill,0,2},
    {0x34,"ZPX","NOP",&zpx,&ill,0,2},{0x35,"ZPX","AND",&zpx,&and,4,2},
    {0x36,"ZPX","ROL",&zpx,&rol,6,2},{0x37,"ZPX","RLA",&zpx,&ill,0,2},
    {0x38,"IMP","SEC",&imp,&sec,2,1},{0x39,"ABY","AND",&aby,&and,4,3},
    {0x3A,"IMP","NOP",&imp,&ill,0,1},{0x3B,"ABY","RLA",&aby,&ill,0,3},
    {0x3C,"ABX","NOP",&abx,&ill,0,3},{0x3D,"ABX","AND",&abx,&and,4,3},
    {0x3E,"ABX","ROL",&abx,&rol,7,3},{0x3F,"ABX","RLA",&abx,&ill,0,3},
    {0x40,"IMP","RTI",&imp,&rti,6,1},{0x41,"IDX","EOR",&idx,&eor,6,2},
    {0x42,"IMM","JAM",&imm,&ill,0,2},{0x43,"IDX","SRE",&idx,&ill,0,2},
    {0x44,"ZPA","NOP",&zpa,&ill,0,2},{0x45,"ZPA","EOR",&zpa,&eor,3,2},
    {0x46,"ZPA","LSR",&zpa,&lsr,5,2},{0x47,"ZPA","SRE",&zpa,&ill,0,2},
    {0x48,"IMP","PHA",&imp,&pha,3,1},{0x49,"IMM","EOR",&imm,&eor,2,2},
    {0x4A,"ACC","LSR",&acc,&lsr,2,1},{0x4B,"IMM","ALR",&imm,&ill,0,2},
    {0x4C,"ABA","JMP",&aba,&jmp,3,3},{0x4D,"ABA","EOR",&aba,&eor,4,3},
    {0x4E,"ABA","LSR",&aba,&lsr,6,3},{0x4F,"ABA","SRE",&aba,&ill,0,3},
    {0x50,"REL","BVC",&rel,&bra,2,2},{0x51,"IDY","EOR",&idy,&eor,5,2},
    {0x52,"IMM","JAM",&imm,&ill,0,2},{0x53,"IDY","SRE",&idy,&ill,0,2},
    {0x54,"ZPX","NOP",&zpx,&ill,0,2},{0x55,"ZPX","EOR",&zpx,&eor,4,2},
    {0x56,"ZPX","LSR",&zpx,&lsr,6,2},{0x57,"ZPX","SRE",&zpx,&ill,0,2},
    {0x58,"IMP","CLI",&imp,&cli,2,1},{0x59,"ABY","EOR",&aby,&eor,4,3},
    {0x5A,"IMP","NOP",&imp,&ill,0,1},{0x5B,"ABY","SRE",&aby,&ill,0,3},
    {0x5C,"ABX","NOP",&abx,&ill,0,3},{0x5D,"ABX","EOR",&abx,&eor,4,3},
    {0x5E,"ABX","LSR",&abx,&lsr,7,3},{0x5F,"ABX","SRE",&abx,&ill,0,3},
    {0x60,"IMP","RTS",&imp,&rts,6,1},{0x61,"IDX","ADC",&idx,&adc,5,2},
    {0x62,"IMM","JAM",&imm,&ill,0,2},{0x63,"IDX","RRA",&idx,&ill,0,2},
    {0x64,"ZPA","NOP",&zpa,&ill,0,2},{0x65,"ZPA","ADC",&zpa,&adc,3,2},
    {0x66,"ZPA","ROR",&zpa,&ror,5,2},{0x67,"ZPA","RRA",&zpa,&ill,0,2},
    {0x68,"IMP","PLA",&imp,&pla,4,1},{0x69,"IMM","ADC",&imm,&adc,2,2},
    {0x6A,"ACC","ROR",&acc,&ror,2,1},{0x6B,"IMM","ARR",&imm,&ill,0,2},
    {0x6C,"IND","JMP",&ind,&jmp,5,3},{0x6D,"ABA","ADC",&aba,&adc,4,3},
    {0x6E,"ABA","ROR",&aba,&ror,6,3},{0x6F,"ABA","RRA",&aba,&ill,0,3},
    {0x70,"REL","BVS",&rel,&bra,2,2},{0x71,"IDY","ADC",&idy,&adc,5,2},
    {0x72,"IMM","JAM",&imm,&ill,0,2},{0x73,"IDY","RRA",&idy,&ill,0,2},
    {0x74,"ZPX","NOP",&zpx,&ill,0,2},{0x75,"ZPX","ADC",&zpx,&adc,4,2},
    {0x76,"ZPX","ROR",&zpx,&ror,6,2},{0x77,"ZPX","RRA",&zpx,&ill,0,2},
    {0x78,"IMP","SEI",&imp,&sei,2,1},{0x79,"ABY","ADC",&aby,&adc,4,3},
    {0x7A,"IMP","NOP",&imp,&ill,0,1},{0x7B,"ABY","RRA",&aby,&ill,0,3},
    {0x7C,"ABX","NOP",&abx,&ill,0,3},{0x7D,"ABX","ADC",&abx,&adc,4,3},
    {0x7E,"ABX","ROR",&abx,&ror,7,3},{0x7F,"ABX","RRA",&abx,&ill,0,3},
    {0x80,"IMM","NOP",&imm,&ill,0,2},{0x81,"IDX","STA",&idx,&sta,6,2},
    {0x82,"IMM","NOP",&imm,&ill,0,2},{0x83,"IDX","SAX",&idx,&ill,0,2},
    {0x84,"ZPA","STY",&zpa,&sty,3,2},{0x85,"ZPA","STA",&zpa,&sta,3,2},
    {0x86,"ZPA","STX",&zpa,&stx,3,2},{0x87,"ZPA","SAX",&zpa,&ill,0,2},
    {0x88,"IMP","DEY",&imp,&dey,2,1},{0x89,"IMM","NOP",&imm,&ill,0,2},
    {0x8A,"IMP","TXA",&imp,&txa,2,1},{0x8B,"IMM","ANE",&imm,&ill,0,2},
    {0x8C,"ABA","STY",&aba,&sty,4,3},{0x8D,"ABA","STA",&aba,&sta,4,3},
    {0x8E,"ABA","STX",&aba,&stx,4,3},{0x8F,"ABA","SAX",&aba,&ill,0,3},
    {0x90,"REL","BCC",&rel,&bra,2,2},{0x91,"IDY","STA",&idy,&sta,5,2},
    {0x92,"IMM","JAM",&imm,&ill,0,2},{0x93,"IDY","SHA",&idy,&ill,0,2},
    {0x94,"ZPX","STY",&zpx,&sty,4,2},{0x95,"ZPX","STA",&zpx,&sta,4,2},
    {0x96,"ZPY","STX",&zpy,&stx,4,2},{0x97,"ZPY","SAX",&zpy,&ill,0,2},
    {0x98,"IMP","TYA",&imp,&tya,2,1},{0x99,"ABY","STA",&aby,&sta,5,3},
    {0x9A,"IMP","TXS",&imp,&txs,2,1},{0x9B,"ABY","TAS",&aby,&ill,0,3},
    {0x9C,"ABX","SHY",&abx,&ill,0,3},{0x9D,"ABX","STA",&abx,&sta,5,3},
    {0x9E,"ABY","SHX",&aby,&ill,0,3},{0x9F,"ABY","SHA",&aby,&ill,0,3},
    {0xA0,"IMM","LDY",&imm,&ldy,2,2},{0xA1,"IDX","LDA",&idx,&lda,6,2},
    {0xA2,"IMM","LDX",&imm,&ldx,2,2},{0xA3,"IDX","LAX",&idx,&ill,0,2},
    {0xA4,"ZPA","LDY",&zpa,&ldy,3,2},{0xA5,"ZPA","LDA",&zpa,&lda,3,2},
    {0xA6,"ZPA","LDX",&zpa,&ldx,3,2},{0xA7,"ZPA","LAX",&zpa,&ill,0,2},
    {0xA8,"IMP","TAY",&imp,&tay,2,1},{0xA9,"IMM","LDA",&imm,&lda,2,2},
    {0xAA,"IMP","TAX",&imp,&tax,2,1},{0xAB,"IMM","LXA",&imm,&ill,0,2},
    {0xAC,"ABA","LDY",&aba,&ldy,4,3},{0xAD,"ABA","LDA",&aba,&lda,4,3},
    {0xAE,"ABA","LDX",&aba,&ldx,4,3},{0xAF,"ABA","LAX",&aba,&ill,0,3},
    {0xB0,"REL","BCS",&rel,&bra,2,2},{0xB1,"IDY","LDA",&idy,&lda,5,2},
    {0xB2,"IMM","JAM",&imm,&ill,0,2},{0xB3,"IDY","LAX",&idy,&ill,0,2},
    {0xB4,"ZPX","LDY",&zpx,&ldy,4,2},{0xB5,"ZPX","LDA",&zpx,&lda,4,2},
    {0xB6,"ZPY","LDX",&zpy,&ldx,4,2},{0xB7,"ZPY","LAX",&zpy,&ill,0,2},
    {0xB8,"IMP","CLV",&imp,&clv,2,1},{0xB9,"ABY","LDA",&aby,&lda,4,3},
    {0xBA,"IMP","TSX",&imp,&tsx,2,1},{0xBB,"ABY","LAS",&aby,&ill,0,3},
    {0xBC,"ABX","LDY",&abx,&ldy,4,3},{0xBD,"ABX","LDA",&abx,&lda,4,3},
    {0xBE,"ABY","LDX",&aby,&ldx,4,3},{0xBF,"ABY","LAX",&aby,&ill,0,3},
    {0xC0,"IMM","CPY",&imm,&cpy,2,2},{0xC1,"IDX","CMP",&idx,&cmp,6,2},
    {0xC2,"IMM","NOP",&imm,&ill,0,2},{0xC3,"IDX","DCP",&idx,&ill,0,2},
    {0xC4,"ZPA","CPY",&zpa,&cpy,3,2},{0xC5,"ZPA","CMP",&zpa,&cmp,3,2},
    {0xC6,"ZPA","DEC",&zpa,&dec,5,2},{0xC7,"ZPA","DCP",&zpa,&ill,0,2},
    {0xC8,"IMP","INY",&imp,&iny,2,1},{0xC9,"IMM","CMP",&imm,&cmp,2,2},
    {0xCA,"IMP","DEX",&imp,&dex,2,1},{0xCB,"IMM","SBX",&imm,&ill,0,2},
    {0xCC,"ABA","CPY",&aba,&cpy,4,3},{0xCD,"ABA","CMP",&aba,&cmp,4,3},
    {0xCE,"ABA","DEC",&aba,&dec,5,3},{0xCF,"ABA","DCP",&aba,&ill,0,3},
    {0xD0,"REL","BNE",&rel,&bra,2,2},{0xD1,"IDY","CMP",&idy,&cmp,5,2},
    {0xD2,"IMM","JAM",&imm,&ill,0,2},{0xD3,"IDY","DCP",&idy,&ill,0,2},
    {0xD4,"ZPX","NOP",&zpx,&ill,0,2},{0xD5,"ZPX","CMP",&zpx,&cmp,4,2},
    {0xD6,"ZPX","DEC",&zpx,&dec,6,2},{0xD7,"ZPX","DCP",&zpx,&ill,0,2},
    {0xD8,"IMP","CLD",&imp,&cld,2,1},{0xD9,"ABY","CMP",&aby,&cmp,4,3},
    {0xDA,"IMP","NOP",&imp,&ill,0,1},{0xDB,"ABY","DCP",&aby,&ill,0,3},
    {0xDC,"ABX","NOP",&abx,&ill,0,3},{0xDD,"ABX","CMP",&abx,&cmp,4,3},
    {0xDE,"ABX","DEC",&abx,&dec,7,3},{0xDF,"ABX","DCP",&abx,&ill,0,3},
    {0xE0,"IMM","CPX",&imm,&cpx,2,2},{0xE1,"IDX","SBC",&idx,&sbc,6,2},
    {0xE2,"IMM","NOP",&imm,&ill,0,2},{0xE3,"IDX","ISC",&idx,&ill,0,2},
    {0xE4,"ZPA","CPX",&zpa,&cpx,3,2},{0xE5,"ZPA","SBC",&zpa,&sbc,3,2},
    {0xE6,"ZPA","INC",&zpa,&inc,5,2},{0xE7,"ZPA","ISC",&zpa,&ill,0,2},
    {0xE8,"IMP","INX",&imp,&inx,2,1},{0xE9,"IMM","SBC",&imm,&sbc,2,2},
    {0xEA,"IMP","NOP",&imp,&nop,2,1},{0xEB,"IMM","USB",&imm,&ill,0,2},
    {0xEC,"ABA","CPX",&aba,&cpx,4,3},{0xED,"ABA","SBC",&aba,&sbc,4,3},
    {0xEE,"ABA","INC",&aba,&inc,6,3},{0xEF,"ABA","ISC",&aba,&ill,0,3},
    {0xF0,"REL","BEQ",&rel,&bra,2,2},{0xF1,"IDY","SBC",&idy,&sbc,5,2},
    {0xF2,"IMM","JAM",&imm,&ill,0,2},{0xF3,"IDY","ISC",&idy,&ill,0,2},
    {0xF4,"ZPX","NOP",&zpx,&ill,0,2},{0xF5,"ZPX","SBC",&zpx,&sbc,4,2},
    {0xF6,"ZPX","INC",&zpx,&inc,6,2},{0xF7,"ZPX","ISC",&zpx,&ill,0,2},
    {0xF8,"IMP","SED",&imp,&sed,2,1},{0xF9,"ABY","SBC",&aby,&sbc,4,3},
    {0xFA,"IMP","NOP",&imp,&ill,0,1},{0xFB,"ABY","ISC",&aby,&ill,0,3},
    {0xFC,"ABX","NOP",&abx,&ill,0,3},{0xFD,"ABX","SBC",&abx,&sbc,4,3},
    {0xFE,"ABX","INC",&abx,&inc,7,3},{0xFF,"ABX","ISC",&abx,&ill,0,3}
};

#endif