  prog_uint16_t bcdTab[128] PROGMEM = {
    // set up the BCD table
    0x8000,              // invalid, cause error null
    0x8000,              // invalid, cause error soh
    0x8000,              // invalid, cause error stx
    0x8000,              // invalid, cause error etx
    0x8000,              // invalid, cause error eot
    0x8000,              // invalid, cause error enq
    0x8000,              // invalid, cause error ack
    0x8000,              // invalid, cause error bel
    0x8000,              // invalid, cause error bs
    0x8000,              // invalid, cause error ht
    0x2000,              // nl release key
    0x8000,              // invalid, cause error vt
    0x8000,              // invalid, cause error ff
    0x0000,              // do nothing, no error cr
    0x8000,              // invalid, cause error so
    0x8000,              // invalid, cause error si
    0x8000,              // invalid, cause error dle
    0x8000,              // invalid, cause error xon
    0x8000,              // invalid, cause error dc2
    0x8000,              // invalid, cause error xoff
    0x8000,              // invalid, cause error dc4
    0x8000,              // invalid, cause error nak
    0x8000,              // invalid, cause error syn
    0x8000,              // invalid, cause error etb
    0x8000,              // invalid, cause error can
    0x8000,              // invalid, cause error em
    0x8000,              // invalid, cause error sub
    0x8000,              // invalid, cause error esc
    0x8000,              // invalid, cause error fs
    0x8000,              // invalid, cause error gs
    0x8000,              // invalid, cause error rs
    0x8000,              // invalid, cause error us
    0x1000,              // space 
    0x0600,              // minus zero 11 0
    0x0806,              // Group Mark 12 7 8
    0x0042,              // # 3 8
    0x0442,              // $ 11 3 8
    0x0222,              // % 0 4 8
    0x0800,              // & 12
    0x0282,              // Record Mark 0 2 8
    0x0222,              // ( 0 4 8
    0x0822,              // Lozenge 12 4 8
    0x0422,              // * 11 4 8
    0x0800,              // + 12
    0x0242,              // , 0 3 8
    0x0400,              // - 11
    0x0842,              // . 12 3 8
    0x0300,              // / 0 1
    0x0200,              // 0 0
    0x0100,              // 1 1
    0x0080,              // 2 2
    0x0040,              // 3 3
    0x0020,              // 4 4
    0x0010,              // 5 5
    0x0008,              // 6 6
    0x0004,              // 7 7
    0x0002,              // 8 8
    0x0001,              // 9 9
    0x0012,              // : 5 8
    0x8000,              // invalid, cause error ;
    0x080A,              // < 12 6 8
    0x0042,              // = 3 8
    0x000A,              // > 6 8
    0x0A00,              // plus zero 12 0
    0x0022,              // @ 4 8
    0x0900,              // A 12 1
    0x0880,              // B 12 2
    0x0840,              // C 12 3
    0x0820,              // D 12 4
    0x0810,              // E 12 5
    0x0808,              // F 12 6
    0x0804,              // G 12 7
    0x0802,              // H 12 8
    0x0801,              // I 12 9
    0x0500,              // J 11 1
    0x0480,              // K 11 2
    0x0440,              // L 11 3
    0x0420,              // M 11 4
    0x0410,              // N 11 5
    0x0408,              // O 11 6
    0x0404,              // P 11 7
    0x0402,              // Q 11 8
    0x0401,              // R 11 9
    0x0280,              // S 0 2
    0x0240,              // T 0 3
    0x0220,              // U 0 4
    0x0210,              // V 0 5
    0x0208,              // W 0 6
    0x0204,              // X 0 7
    0x0202,              // Y 0 8
    0x0201,              // Z 0 9
    0x8000,              // invalid, cause error [
    0x8000,              // invalid, cause error \
    0x8000,              // invalid, cause error ]
    0x8000,              // invalid, cause error ^
    0x8000,              // invalid, cause error _
    0x8000,              // invalid, cause error `
    0x0900,              // a 12 1
    0x0880,              // b 12 2
    0x0840,              // c 12 3
    0x0820,              // d 12 4
    0x0810,              // e 12 5
    0x0808,              // f 12 6
    0x0804,              // g 12 7
    0x0802,              // h 12 8
    0x0801,              // i 12 9
    0x0500,              // j 11 1
    0x0480,              // k 11 2
    0x0440,              // l 11 3
    0x0420,              // m 11 4
    0x0410,              // n 11 5
    0x0408,              // o 11 6
    0x0404,              // p 11 7
    0x0402,              // q 11 8
    0x0401,              // r 11 9
    0x0280,              // s 0 2
    0x0240,              // t 0 3
    0x0220,              // u 0 4
    0x0210,              // v 0 5
    0x0208,              // w 0 6
    0x0204,              // x 0 7
    0x0202,              // y 0 8
    0x0201,              // z 0 9
    0x8000,              // invalid, cause error {
    0x8000,              // invalid, cause error |
    0x8000,              // invalid, cause error }
    0x8000,              // invalid, cause error ~
    0x8000               // invalid, cause error del
};

  prog_uint16_t ebcdicTab[128] PROGMEM = {
    // set up the EBCDIC table
  0x8000,              // invalid, cause error null
  0x8000,              // invalid, cause error soh
  0x8000,              // invalid, cause error stx
  0x8000,              // invalid, cause error etx
  0x8000,              // invalid, cause error eot
  0x8000,              // invalid, cause error enq
  0x8000,              // invalid, cause error ack
  0x8000,              // invalid, cause error bel
  0x8000,              // invalid, cause error bs
  0x8000,              // invalid, cause error ht
  0x2000,              // nl release key
  0x8000,              // invalid, cause error vt
  0x8000,              // invalid, cause error ff
  0x0000,              // do nothing, no error cr
  0x8000,              // invalid, cause error so
  0x8000,              // invalid, cause error si
  0x8000,              // invalid, cause error dle
  0x8000,              // invalid, cause error xon
  0x8000,              // invalid, cause error dc2
  0x8000,              // invalid, cause error xoff
  0x8000,              // invalid, cause error dc4
  0x8000,              // invalid, cause error nak
  0x8000,              // invalid, cause error syn
  0x8000,              // invalid, cause error etb
  0x8000,              // invalid, cause error can
  0x8000,              // invalid, cause error em
  0x8000,              // invalid, cause error sub
  0x8000,              // invalid, cause error esc
  0x8000,              // invalid, cause error fs
  0x8000,              // invalid, cause error gs
    0x8000,              // invalid, cause error rs
    0x8000,              // invalid, cause error us
    0x1000,              // space 
    0x0482,              // ! 11 2 8
    0x0006,              // " 7 8
    0x0042,              // # 3 8
    0x0442,              // $ 11 3 8
    0x0222,              // % 0 4 8
    0x0800,              // & 12
    0x0012,              // ' 5 8
    0x0812,              // ( 12 5 8
    0x0412,              // ) 11 5 8
    0x0422,              // * 11 4 8
    0x080A,              // + 12 6 8
    0x0242,              // , 0 3 8
    0x0400,              // - 11
    0x0842,              // . 12 3 8
    0x0300,              // / 0 1
    0x0200,              // 0 0
    0x0100,              // 1 1
    0x0080,              // 2 2
    0x0040,              // 3 3
    0x0020,              // 4 4
    0x0010,              // 5 5
    0x0008,              // 6 6
    0x0004,              // 7 7
    0x0002,              // 8 8
    0x0001,              // 9 9
    0x0082,              // : 2 8
    0x040A,              // ; 11 6 8
    0x0822,              // < 12 4 8
  0x000A,              // = 6 8
  0x020A,              // > 0 6 8
  0x0206,              // ? 0 7 8
  0x0022,              // @ 4 8
  0x0900,              // A 12 1
  0x0880,              // B 12 2
  0x0840,              // C 12 3
  0x0820,              // D 12 4
  0x0810,              // E 12 5
  0x0808,              // F 12 6
  0x0804,              // G 12 7
  0x0802,              // H 12 8
  0x0801,              // I 12 9
  0x0500,              // J 11 1
  0x0480,              // K 11 2
  0x0440,              // L 11 3
  0x0420,              // M 11 4
  0x0410,              // N 11 5
  0x0408,              // O 11 6
  0x0404,              // P 11 7
  0x0402,              // Q 11 8
  0x0401,              // R 11 9
  0x0280,              // S 0 2
  0x0240,              // T 0 3
  0x0220,              // U 0 4
  0x0210,              // V 0 5
  0x0208,              // W 0 6
  0x0204,              // X 0 7
  0x0202,              // Y 0 8
  0x0201,              // Z 0 9
  0x0882,              // [ is cent sign 12 2 8
  0x0288,              // \ 0 2 6
  0x0C00,              // ] is vertical broken bar 12 11
  0x0406,              // ^ is PL/I not 11 7 8
  0x0212,              // _ 0 5 8
  0x0102,              // ` 1 8
  0x0B00,              // a 12 0 1
  0x0A80,              // b 12 0 2
  0x0A40,              // c 12 0 3
  0x0A20,              // d 12 0 4
  0x0A10,              // e 12 0 5
  0x0A08,              // f 12 0 6
  0x0A04,              // g 12 0 7
  0x0A02,              // h 12 0 8
  0x0A01,              // i 12 0 9
  0x0D00,              // j 12 11 1
  0x0C80,              // k 12 11 2
  0x0C40,              // l 12 11 3
  0x0C20,              // m 12 11 4
  0x0C10,              // n 12 11 5
  0x0C08,              // o 12 11 6
  0x0C04,              // p 12 11 7
  0x0C02,              // q 12 11 8
  0x0C01,              // r 12 11 9
  0x0680,              // s 11 0 2
  0x0640,              // t 11 0 3
  0x0620,              // u 11 0 4
  0x0610,              // v 11 0 5
  0x0608,              // w 11 0 6
  0x0604,              // x 11 0 7
  0x0602,              // y 11 0 8
  0x0601,              // z 11 0 9
  0x0A00,              // { 12 0
  0x0806,              // | 12 7 8
  0x0600,              // } 11 0
  0x0700,              // ~ 11 0 1
  0x8000               // invalid, cause error del
};

