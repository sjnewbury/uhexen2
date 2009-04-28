/*
	net.h
	quake's interface to the networking layer
	network functions and data, common to the
	whole engine

	$Header: /home/ozzie/Download/0000/uhexen2/hexen2/net.h,v 1.25 2009-04-28 14:00:32 sezero Exp $
*/

#ifndef __HX2_NET_H
#define __HX2_NET_H


#define	NET_NAMELEN		64

#define NET_MAXMESSAGE		16384


extern int		DEFAULTnet_hostport;
extern int		net_hostport;

extern cvar_t		hostname;

extern	double		net_time;
extern	sizebuf_t	net_message;
extern	int		net_activeconnections;


void	NET_Init (void);
void	NET_Shutdown (void);

struct qsocket_s	*NET_CheckNewConnections (void);
// returns a new connection number if there is one pending, else -1

struct qsocket_s	*NET_Connect (const char *host);
// called by client to connect to a host.  Returns -1 if not able to

qboolean NET_CanSendMessage (struct qsocket_s *sock);
// Returns true or false if the given qsocket can currently accept a
// message to be transmitted.

int	NET_GetMessage (struct qsocket_s *sock);
// returns data in net_message sizebuf
// returns 0 if no data is waiting
// returns 1 if a message was received
// returns 2 if an unreliable message was received
// returns -1 if the connection died

int	NET_SendMessage (struct qsocket_s *sock, sizebuf_t *data);
int	NET_SendUnreliableMessage (struct qsocket_s *sock, sizebuf_t *data);
// returns 0 if the message connot be delivered reliably, but the connection
//		is still considered valid
// returns 1 if the message was sent properly
// returns -1 if the connection died

int	NET_SendToAll(sizebuf_t *data, double blocktime);
// This is a reliable *blocking* send to all attached clients.

void	NET_Close (struct qsocket_s *sock);
// if a dead connection is returned by a get or send function, this function
// should be called when it is convenient

// Server calls when a client is kicked off for a game related misbehavior
// like an illegal protocal conversation.  Client calls when disconnecting
// from a server.
// A netcon_t number will not be reused until this function is called for it

void	NET_Poll (void);


// Server list related globals:
extern	qboolean	slistInProgress;
extern	qboolean	slistSilent;
extern	qboolean	slistLocal;

void	NET_Slist_f (void);


/* FIXME: driver related, but public:
 */
extern	qboolean	serialAvailable;
extern	qboolean	ipxAvailable;
extern	qboolean	tcpipAvailable;
extern	char		my_ipx_address[NET_NAMELEN];
extern	char		my_tcpip_address[NET_NAMELEN];

#define	NET_USE_SERIAL	0
#if defined(PLATFORM_DOS)
#undef	NET_USE_SERIAL
#define	NET_USE_SERIAL	1
#endif	/* allow serial */

#if NET_USE_SERIAL
extern	void (*GetComPortConfig) (int portNumber, int *port, int *irq, int *baud, qboolean *useModem);
extern	void (*SetComPortConfig) (int portNumber, int port, int irq, int baud, qboolean useModem);
extern	void (*GetModemConfig) (int portNumber, char *dialType, char *clear, char *init, char *hangup);
extern	void (*SetModemConfig) (int portNumber, const char *dialType, const char *clear, const char *init, const char *hangup);
#endif	/* NET_USE_SERIAL */

#endif	/* __HX2_NET_H */

