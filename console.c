// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void consputc(int);

static int panicked = 0;

static struct {
	struct spinlock lock;
	int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
	static char digits[] = "0123456789abcdef";
	char buf[16];
	int i;
	uint x;

	if(sign && (sign = xx < 0))
		x = -xx;
	else
		x = xx;

	i = 0;
	do{
		buf[i++] = digits[x % base];
	}while((x /= base) != 0);

	if(sign)
		buf[i++] = '-';

	while(--i >= 0)
		consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
	int i, c, locking;
	uint *argp;
	char *s;

	locking = cons.locking;
	if(locking)
		acquire(&cons.lock);

	if (fmt == 0)
		panic("null fmt");

	argp = (uint*)(void*)(&fmt + 1);
	for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
		if(c != '%'){
			consputc(c);
			continue;
		}
		c = fmt[++i] & 0xff;
		if(c == 0)
			break;
		switch(c){
		case 'd':
			printint(*argp++, 10, 1);
			break;
		case 'x':
		case 'p':
			printint(*argp++, 16, 0);
			break;
		case 's':
			if((s = (char*)*argp++) == 0)
				s = "(null)";
			for(; *s; s++)
				consputc(*s);
			break;
		case '%':
			consputc('%');
			break;
		default:
			// Print unknown % sequence to draw attention.
			consputc('%');
			consputc(c);
			break;
		}
	}

	if(locking)
		release(&cons.lock);
}

void
panic(char *s)
{
	int i;
	uint pcs[10];

	cli();
	cons.locking = 0;
	// use lapiccpunum so that we can call panic from mycpu()
	cprintf("lapicid %d: panic: ", lapicid());
	cprintf(s);
	cprintf("\n");
	getcallerpcs(&s, pcs);
	for(i=0; i<10; i++)
		cprintf(" %p", pcs[i]);
	panicked = 1; // freeze other CPU
	for(;;)
		;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
#define LEFT 0xE4
#define RIGHT 0xE5
#define LINE_LENGTH 80
#define GRAY 0x0800
#define WHITE 0x0700
#define BLUE 0x0300

static ushort *crt = (ushort*)P2V(0xb8000);	// CGA memory

static void colorize(int position, int color)
{
	crt[position] = (crt[position] & 0xff) | color;
}

static int get_position()
{
	// Cursor position: col + LINE_LENGTH * row.
	outb(CRTPORT, 14);
	int position = inb(CRTPORT + 1) << 8;
	outb(CRTPORT, 15);
	position |= inb(CRTPORT + 1);
	return position;
}

static int scroll_up(int position)
{
	memmove(crt, crt + LINE_LENGTH, sizeof(crt[0]) * 23 * LINE_LENGTH);
	position -= LINE_LENGTH;
	memset(crt + position, 0, sizeof(crt[0]) * (24 * LINE_LENGTH - position));
	return position;
}

static void change_outb(int position)
{
	outb(CRTPORT, 14);
	outb(CRTPORT + 1, position >> 8);
	outb(CRTPORT, 15);
	outb(CRTPORT + 1, position);
}

static void cgaputc(int new_character)
{
	int position = get_position();

	switch (new_character)
	{
		case (RIGHT):
			position++;
			colorize(position, GRAY);
			break;
	
		case (LEFT):
			if (position > 0)
				--position;
			colorize(position, WHITE);
			break;
	
		case ('\n'):
			position += LINE_LENGTH - position % LINE_LENGTH;
			break;

		case (BACKSPACE):
			if(position > 0)
			{
				memmove(crt + position - 1, crt + position, 24 * LINE_LENGTH * sizeof(crt[0]));
				--position;
			}
			break;

		default:
			memmove(crt + position + 1, crt + position, 24 * LINE_LENGTH * sizeof(crt[0]));
			crt[position] = new_character;
			colorize(position++, BLUE);
			break;
	}

	if(position < 0 || position > 25 * LINE_LENGTH)
		panic("position under/overflow");

	if((position / LINE_LENGTH) >= 24)
		position = scroll_up(position);

	change_outb(position);		/// @todo Choose better name for this function
}

void
consputc(int c)
{
	if(panicked){
		cli();
		for(;;)
			;
	}

	if(c == BACKSPACE){
		uartputc('\b'); uartputc(' '); uartputc('\b');
	} else
		uartputc(c);
	cgaputc(c);
}

#define INPUT_BUF 128
struct {
	char buf[INPUT_BUF];
	uint read_index;
	uint write_index;
	uint end_index;
	uint edit_index;	
} input;

#define C(x)	((x)-'@')	// Control-x

void add_to_buffer(int new_character)
{
	new_character = (new_character == '\r') ? '\n' : new_character;

	consputc(new_character);
	if(new_character == '\n' || new_character == C('D') || input.end_index == input.read_index + INPUT_BUF)
	{
		input.buf[input.end_index++ % INPUT_BUF] = new_character;
		input.write_index = input.end_index;
		input.edit_index = input.end_index;
		wakeup(&input.read_index);
	}
	else
	{
		memmove(input.buf + input.edit_index + 1, input.buf + input.edit_index,
				input.end_index - input.edit_index);
		input.buf[input.edit_index % INPUT_BUF] = new_character;
		input.edit_index++;
		input.end_index++;
	}
}

void consoleintr(int (*getc)(void))
{
	int new_character, doprocdump = 0;
	acquire(&cons.lock);

	while((new_character = getc()) >= 0)
	{
		switch(new_character)
		{
			case C('P'):		// Process listing.
				doprocdump = 1;
				break;
	
			case C('U'):		// Kill line.
				while(input.end_index != input.write_index 
						&& input.buf[(input.end_index-1) % INPUT_BUF] != '\n')
				{
					input.end_index--;
					consputc(BACKSPACE);
				}
				input.edit_index = input.write_index;
				break;
	
			case C('H'):
			case BACKSPACE:
				if(input.end_index != input.write_index)
				{
					memmove(input.buf + input.edit_index - 1, input.buf + input.edit_index, 
							input.end_index - input.edit_index);
					input.end_index--;
					input.edit_index--;
					consputc(BACKSPACE);
				}
				break;
	
			case (RIGHT):
				if (input.edit_index < input.end_index)
				{
					input.edit_index++;
					consputc(new_character);
				}
				break;
	
			case (LEFT):
				if(input.edit_index > input.write_index)
				{
					input.edit_index--;
					consputc(new_character);
				}
				break;
			
			default:
				if(new_character != 0 && input.end_index - input.read_index < INPUT_BUF)
					add_to_buffer(new_character);
				break;
		}
	}

	release(&cons.lock);
	if(doprocdump)
		procdump();		// now call procdump() wo. cons.lock held
}

int
consoleread(struct inode *ip, char *dst, int n)
{
	uint target;
	int c;

	iunlock(ip);
	target = n;
	acquire(&cons.lock);
	while(n > 0){
		while(input.read_index == input.write_index){
			if(myproc()->killed){
				release(&cons.lock);
				ilock(ip);
				return -1;
			}
			sleep(&input.read_index, &cons.lock);
		}
		c = input.buf[input.read_index++ % INPUT_BUF];
		if(c == C('D')){	// EOF
			if(n < target){
				// Save ^D for next time, to make sure
				// caller gets a 0-byte result.
				input.read_index--;
			}
			break;
		}
		*dst++ = c;
		--n;
		if(c == '\n')
			break;
	}
	release(&cons.lock);
	ilock(ip);

	return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
	int i;

	iunlock(ip);
	acquire(&cons.lock);
	for(i = 0; i < n; i++)
		consputc(buf[i] & 0xff);
	release(&cons.lock);
	ilock(ip);

	return n;
}

void
consoleinit(void)
{
	initlock(&cons.lock, "console");

	devsw[CONSOLE].write = consolewrite;
	devsw[CONSOLE].read = consoleread;
	cons.locking = 1;

	ioapicenable(IRQ_KBD, 0);
}
