/*	
 *	A 'l33t programming language' interpreter
 *
 *	-DSOCKET to enable CON.
 *	Spec does not clearly specify CON is TCP or UDP.
 *	We use TCP here.
 *
 *	$Id: l33t.c,v 1.3 2005/04/15 18:07:26 alecs Exp alecs $
 *	 	
 *	Copyright (C) 2005	Alecs
 *	License: GPL
 *	    
 *	DEMO ONLY.
 *	      
 *	THIS SOFTWARE COMES "AS IS" WITHOUT ANY WARRANTIES.
 */

#ifdef SOCKET
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#define SA	struct sockaddr
static 	char	*err = "h0s7 5uXz0r5! c4N'7 c0Nn3<7 l0l0l0l0l l4m3R !!!\n";
#endif

#include <stdio.h>
#include <stdlib.h>

#define SIZE	(64 * 1024)

#define NOP	0
#define	WRT	1
#define RD	2
#define IF	3
#define EIF	4
#define FWD	5
#define BAK	6
#define INC	7
#define DEC	8
#define CON	9
#define END	10

static void	input(FILE *fin);
static void	run(void);
static void	inc(unsigned char **who, int by);
static void	dec(unsigned char **who, int by);

static unsigned char	data[SIZE], *op, *mem;

int
main(int argc, char *argv[])
{
	FILE	*fin;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(1);
	}

	if ((fin = fopen(argv[1], "r")) == NULL) {
		perror("fopen error");
		exit(1);
	}

	op = mem = data;
	input(fin);
	run();

	return (0);
}

static void
input(FILE *fin)
{
	int		c;
	unsigned char	val, last, now;

	last = ' ';
	val = 0;
	while ((c = fgetc(fin)) != EOF) {
		now = (unsigned char) c;
		if (now == ' ' || now == '\t' || now == '\r' || now == '\n') {
			now = ' ';
			if (last == now)
				continue;
			else {
				*mem++ = val;
				val = 0;
			}
		} else if (now >= '0' && now <= '9')
			val += now - '0';
		last = now;
	}
	if (last != ' ')	/* in case file not ended by newline */
		*mem++ = val;
}

static void
run(void)
{
	int			nest;
#ifdef SOCKET
	int			fd;
	uint32_t		addr, a, b, c, d;
	uint16_t		port, e, f;
	struct sockaddr_in	peer;
#endif

	while (*op != END) {
		switch (*op) {
		case NOP:
			inc(&op, 1);
			break;
		case WRT:
			putchar(*mem);
			inc(&op, 1);
			break;
		case RD:
			*mem = getchar();
			inc(&op, 1);
			break;
		case IF:
			if (*mem == 0) {
				nest = 1;
				while (nest > 0) {
					inc(&op, 1);
					if (*op == IF)
						nest++;
					else if (*op == EIF)
						nest--;
				}
			}
			inc(&op, 1);
			break;
		case EIF:
			if (*mem != 0) {
				nest = 1;
				while (nest > 0) {
					dec(&op, 1);
					if (*op == EIF)
						nest++;
					else if (*op == IF)
						nest--;
				}
			}
			inc(&op, 1);
			break;
		case FWD:
			inc(&mem, *(op + 1) + 1);
			inc(&op, 2);
			break;
		case BAK:
			dec(&mem, *(op + 1) + 1);
			inc(&op, 2);
			break;
		case INC:
			*mem += *(op + 1) + 1;
			inc(&op, 2);
			break;
		case DEC:
			*mem -= *(op + 1) + 1;
			inc(&op, 2);
			break;
		case CON:
#ifdef SOCKET
			a = *mem++;
			b = *mem++;
			c = *mem++;
			d = *mem++;
			addr = a << 24 | b << 16 | c << 8 | d;
			e = *mem++;
			f = *mem++;
			port = e << 8 | f;
			if (addr == 0 && port == 0)
				continue;
			peer.sin_family = AF_INET;
			peer.sin_addr.s_addr = htonl(addr);
			peer.sin_port = htons(port);
			if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
				fprintf(stderr, err);
				continue;
			}
			if (connect(fd, (SA *) &peer, sizeof(peer)) < 0) {
				fprintf(stderr, err);
				close(fd);
				continue;
			}
			dup2(fd, 0);
			dup2(fd, 1);
			break;
#else
			fprintf(stderr, "( () |\\| n07 1mp|3m3n73d y37!\n");
			exit(1);
#endif
		case END:
			return;
		default:
			fprintf(stderr, "j00 4r3 teh 5ux0r\n");
			exit(1);
		}
	}
}

/* increment pointers; wrap if needed */
static void
inc(unsigned char **who, int by)
{
	if ((*who += by) >= data + SIZE)
		*who = data;
}

/* decrement pointers; wrap if needed */
static void
dec(unsigned char **who, int by)
{
	if ((*who -= by) < data)
		*who = data;
}