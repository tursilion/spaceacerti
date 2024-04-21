#include <vdp.h>
#include <kscan.h>

const unsigned int grmbase = 0x9808;
const unsigned int story1 = 0xc000;
const unsigned int story2 = 0xe000;
const unsigned int lines1 = 204;
const unsigned int lines2 = 185;
const unsigned int screen_size = 22;

int line;
unsigned char oldkey;

void __attribute__ ((noinline)) copylines(unsigned int grom, unsigned int vdp, unsigned int sz) {
    // we abuse this pretty badly, changing registers we aren't allowed to change
    __asm__ volatile(
            "movb %[grom],@>9c02"   // set GROM address
        "\n\tswpb %[grom]"
        "\n\tmovb %[grom],@>9c02"
        "\n\tswpb %[grom]"
        
        "\n\tswpb %[vdp]"                  // set VDP address
        "\n\tmovb %[vdp],@>8c02"
        "\n\tswpb %[vdp]"
        "\n\tori %[vdp],>4000"
        "\n\tmovb %[vdp],@>8c02"
        
        "\nlbl%="
        "\n\tmovb *%[base],@>8c00"
        "\n\tdec %[size]"
        "\n\tjne lbl%="
        
        :
        : [base] "r" (grmbase), [grom] "r" (grom), [vdp] "r" (vdp), [size] "r" (sz)
        : "cc"
    );
}

int main() {
    set_text();
    charsetlc();
    VDP_SET_REGISTER(7,0xe4);
    vdpmemset(gImage, ' ', 40*24);
    //                       1234567890123456789012345678901234567890
    vdpmemcpy(gImage+23*40, "E/X for line, 4/6 for page, QUIT to exit", 40);
    line = 0;
    oldkey=KSCAN_KEY;
    
    for (;;) {
        // redraw screen - normally we can use a large copy, but
        // if we are at the junction point, it requires two
        if (line <= lines1-screen_size) {
            // single copy from bank 1
            copylines(story1+line*40, gImage, screen_size*40);
        } else if (line >= lines1) {
            // single copy from bank 2
            copylines(story2+(line-lines1)*40, gImage, screen_size*40);
        } else {
            // copy from both banks
            unsigned int l1 = lines1-line;
            copylines(story1+line*40, gImage, l1*40);
            copylines(story2, gImage+(l1*40), (screen_size-l1)*40);
        }
        
        // now wait for the input
        int oldline = line;
        while (oldline == line) {
            while (KSCAN_KEY == oldkey) {
                VDP_INT_POLL;
                kscanfast(0);
            }
            oldkey = KSCAN_KEY;
            
            switch (oldkey) {
                case 'E':
                    if (line > 0) --line;
                    break;
                    
                case '6':
                    if (line > 0) {
                        line -= screen_size;
                        if (line < 0) line = 0;
                    }
                    break;
                
                case 'X':
                    if (line < lines1+lines2-screen_size) ++line;
                    break;
                    
                case '4':
                    if (line < lines1+lines2-screen_size) {
                        line += screen_size;
                        if (line > lines1+lines2-screen_size) {
                            line = lines1+lines2-screen_size;
                        }
                    }
                    break;
            }
        }
    }
    
    return 0;
}
