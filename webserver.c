#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lowlevel.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "Interrupts.h"			// For number of interrupt
#include "ADC.h"				// For light & temp
#include "TypeConversion.h"		//sprintf conversion

static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24};  
static uint8_t myip[4] = {192,168,0,50};	//changed last value
static uint16_t mywwwport = 80; // listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 1000
static uint8_t buf[BUFFER_SIZE+1];

//**************************************************************************************
uint16_t PrintLinkpage (uint8_t *buf)
{
   uint16_t plen;
   plen = Fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<center><h1>Links to other webservers in third semester ET<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.0.30\">Group 1: Hasse, Mike, Mikkel & Mohib </a><hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.0.40\">Group 2: Unissa, HC & Jonas</a><hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.0.50\">Group 3: Baldur, Bo, Osvaldas & Petru</a><hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.0.60\">Group 4: Lazlo & Thomaz</a><hr>"));

   plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.0.29\">Back to frontpage</a><hr>"));
   return plen;
}

//**************************************************************************************
uint16_t PrintWebpage (uint8_t *buf)
{	
	uint16_t plen;

   //Write/Publish to the webserver
   plen = Fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<center><p><h1>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<TITLE>KEA_EWS</TITLE><hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<font color=\"blue\">Group 3 - Members: Osvaldas, Baldur, Petru and Bo<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<A HREF=\"http://192.168.0.50\">Reload</A><hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<A HREF=\"http://192.168.0.50/M\">Menu</A><hr>"));
   
   //Sending sensor + interrrupts data
   plen = Fill_tcp_data_p(buf,plen,PSTR("Temperature:"));
   plen = Fill_tcp_data(buf,plen,intToString(getTempNatural()));
   plen = Fill_tcp_data_p(buf,plen,PSTR("."));
   plen = Fill_tcp_data(buf,plen,intToString(getTempFractional()));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<hr>Interrupts from LAN:"));
   plen = Fill_tcp_data(buf,plen,intToString(isrCount));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<hr>LDR values:"));
   
   plen = Fill_tcp_data_p(buf,plen,PSTR("<hr>Time since reset:"));
   plen = Fill_tcp_data(buf,plen,intToString(time_count));

   return(plen);
}

//**************************************************************************************
void InitPhy (void)
{
   /*initialize enc28j60*/
   Enc28j60Init(mymac);
   DelayMs(10);
     
   /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
   // LEDA=green LEDB=yellow
   //
   // 0x880 is PHLCON LEDB=on, LEDA=on
   // enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
   Enc28j60PhyWrite(PHLCON,0x880);
   DelayMs(500);
   //
   // 0x990 is PHLCON LEDB=off, LEDA=off
   // enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
   Enc28j60PhyWrite(PHLCON,0x990);
   DelayMs(500);
   //
   // 0x880 is PHLCON LEDB=on, LEDA=on
   // enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
   Enc28j60PhyWrite(PHLCON,0x880);
   DelayMs(500);
   //
   // 0x990 is PHLCON LEDB=off, LEDA=off
   // enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
   Enc28j60PhyWrite(PHLCON,0x990);
   DelayMs(500);
   //
   // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
   // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
   Enc28j60PhyWrite(PHLCON,0x476);
   DelayMs(100);
     
   //init the ethernet/ip layer:
   Init_ip_arp_udp_tcp(mymac,myip,80);
}

//**************************************************************************************
uint8_t GetCommand (char *x)
{
   if (strncmp("/ ", x, 2) == 0) return 0;
   if (strncmp("/D", x, 2) == 0) return 1;
   if (strncmp("/U", x, 2) == 0) return 2;
   if (strncmp("/M", x, 2) == 0) return 3;
   return 0;
}

//*****************************************************************************
void LanTask (void)
{
   uint16_t plen, dat_p = 0;
   int send, linkPage;

	while(1)
	{   
      plen = Enc28j60PacketReceive(BUFFER_SIZE, buf);
         /*plen will unequal to zero if there is a valid packet (without crc error) */
      if (plen == 0) return;

      // arp is broadcast if unknown but a host may also verify the mac address by sending it to a unicast address.
      if (Eth_type_is_arp_and_my_ip(buf, plen))
      {
         Make_arp_answer_from_request(buf);
         continue;
      }
      // check if the ip packet is for us:
      if (Eth_type_is_ip_and_my_ip(buf, plen) == 0)
      {
         continue;
      }
      // ping
      if((buf[IP_PROTO_P] == IP_PROTO_ICMP_V) && (buf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V))
      {
         Make_echo_reply_from_request(buf, plen);
         continue;
      }

      // tcp port www start, compare only the lower byte
      if ((buf[IP_PROTO_P] == IP_PROTO_TCP_V) && (buf[TCP_DST_PORT_H_P] == 0) && (buf[TCP_DST_PORT_L_P] == mywwwport))
      {
         if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
         {
            Make_tcp_synack_from_syn(buf); // make_tcp_synack_from_syn does already send the syn,ack
            continue;
         }
         if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
         {
            Init_len_info(buf); // init some data structures
            dat_p = Get_tcp_data_pointer();
            if (dat_p == 0)
            { 
               // we can possibly have no data, just ack:
               if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
               {
                  Make_tcp_ack_from_any(buf);
               }
               continue;
            }
         }
         send = 0;
         if (strncmp("GET ",(char *) & (buf[dat_p]), 4) != 0)
         {
          	// head, post and other methods for possible status codes see:
            // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
            plen = Fill_tcp_data_p(buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
            send = 1;
         }

	      if (!send)
         {
            char *p;
            linkPage = 0;
            p = (char*) &(buf[dat_p+4]);  // start of incomming payload
            switch (GetCommand(p))
            {
               case 0:
                  break;
               case 1:
                 //countUp = false;
                  break;
               case 2:
                  //countUp = true;
                  break;
               case 3:
                  linkPage = 1;
                  break;
            }
            if (linkPage)
            {
               plen = PrintLinkpage(buf);
            }
            else
            {
               plen = PrintWebpage(buf);
            }
            send = 1;
         }

         Make_tcp_ack_from_any(buf); // send ack for http get
         Make_tcp_ack_with_data(buf, plen); // send data       
      }
   }
}
//*****************************************************************************
