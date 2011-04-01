/*!
 * \file zoneparser.h
 *
 * \author modifications by Jan Kadlec <jan.kadlec@nic.cz>, most of the code
 *         by NLnet Labs.
 *         Copyright (c) 2001-2006, NLnet Labs. All rights reserved.
 *         See LICENSE for the license.
 *
 * \brief Zone compiler.
 *
 * \addtogroup zoneparser
 * @{
 */

#ifndef _KNOT_ZONEPARSER_H_
#define _KNOT_ZONEPARSER_H_

#include <stdio.h>

#include "dnslib/dname.h"
#include "dnslib/rrset.h"
#include "dnslib/node.h"
#include "dnslib/rdata.h"
#include "dnslib/zone.h"
#include "common/slab/slab.h"

#define MAXRDATALEN	64
#define MAXLABELLEN	63
#define MAXDOMAINLEN	255
#define MAX_RDLENGTH	65535
#define	MAXTOKENSLEN	512		/* Maximum number of tokens per entry */
#define	B64BUFSIZE	65535		/* Buffer size for b64 conversion */
#define	ROOT		(const uint8_t *)"\001"

#define NSEC_WINDOW_COUNT     256
#define NSEC_WINDOW_BITS_COUNT 256
#define NSEC_WINDOW_BITS_SIZE  (NSEC_WINDOW_BITS_COUNT / 8)

#define IPSECKEY_NOGATEWAY      0       /* RFC 4025 */
#define IPSECKEY_IP4            1
#define IPSECKEY_IP6            2
#define IPSECKEY_DNAME          3

#define LINEBUFSZ 1024

struct lex_data {
    size_t   len;		/* holds the label length */
    char    *str;		/* holds the data */
};

#define DEFAULT_TTL 3600

/*! \todo Implement ZoneDB. */
typedef void namedb_type;

/*!
 * \brief One-purpose linked list holding pointers to RRSets.
 */
struct rrset_list {
        dnslib_rrset_t *data; /*!< List data. */
        struct rrset_list *next; /*!< Next node. */
};

typedef struct rrset_list rrset_list_t;

/*!
 * \brief Main zoneparser structure.
 */
struct zparser {
        const char *filename; /*!< TODO */
        uint32_t default_ttl; /*!< Default TTL. */
        uint16_t default_class; /*!< Default class. */
        dnslib_zone_t *current_zone; /*!< Current zone. */
        dnslib_node_t *origin; /*!< Origin node. */
        dnslib_dname_t *prev_dname; /*!< Previous dname. */
        dnslib_node_t *default_apex; /*!< Zone default apex. */

        dnslib_node_t *last_node; /*!< Last processed node. */

        char *dname_str; /*!< TODO */

        int error_occurred; /*!< TODO */
        unsigned int errors; /*!< TODO */
        unsigned int line; /*!< TODO */

        size_t id; /*!< TODO */

        dnslib_rrset_t *current_rrset; /*!< Current RRSet. */
        dnslib_rdata_item_t *temporary_items; /*!< Temporary rdata items. */

        /*!
         * \brief list of RRSIGs that were not inside their nodes in zone file
         */
        rrset_list_t *rrsig_orphans;

        dnslib_dname_t *root_domain; /*!< Root domain name. */

        slab_cache_t *parser_slab; /*!< Slab for parser. */

        rrset_list_t *node_rrsigs; /*!< List of RRSIGs in current node. */

        int rdata_count; /*!< Count of parsed rdata. */
};

typedef struct zparser zparser_type;

extern zparser_type *parser;

/* used in zonec.lex */
extern FILE *yyin;

int yyparse(void);

int yylex(void);

void yyrestart(FILE *);

/*!
 * \brief Does all the processing of RR - saves to zone, assigns RRSIGs etc.
 */
int process_rr();

/*!
 * \brief Converts hex text format to wireformat.
 *
 * \param hex String to be converted.
 * \param len Length of string.
 */
uint16_t *zparser_conv_hex(const char *hex, size_t len);

/*!
 * \brief Converts hex text format with length to wireformat.
 *
 * \param hex String to be converted/.
 * \param len Length of string.
 */
uint16_t *zparser_conv_hex_length(const char *hex, size_t len);

/*!
 * \brief Converts time string to wireformat.
 *
 * \param time Time string to be converted.
 */
uint16_t *zparser_conv_time(const char *time);
/*!
 * \brief Converts a protocol and a list of service port numbers
 * (separated by spaces) in the rdata to wireformat
 *
 * \param protostr Protocol string.
 * \param servicestr Service string.
 */
uint16_t *zparser_conv_services(const char *protostr, char *servicestr);

/*!
 * \brief Converts serial to wireformat.
 *
 * \param periodstr Serial string.
 */
uint16_t *zparser_conv_serial(const char *serialstr);
/*!
 * \brief Converts period to wireformat.
 *
 * \param periodstr Period string.
 */
uint16_t *zparser_conv_period(const char *periodstr);

/*!
 * \brief Converts short int to wireformat.
 *
 * \param text String containing short int.
 */
uint16_t *zparser_conv_short(const char *text);

/*!
 * \brief Converts long int to wireformat.
 *
 * \param text String containing long int.
 */
uint16_t *zparser_conv_long(const char *text);

/*!
 * \brief Converts byte to wireformat.
 *
 * \param text String containing byte.
 */
uint16_t *zparser_conv_byte(const char *text);

/*!
 * \brief Converts A rdata string to wireformat.
 *
 * \param text String containing A rdata.
 */
uint16_t *zparser_conv_a(const char *text);

/*!
 * \brief Converts AAAA rdata string to wireformat.
 *
 * \param text String containing AAAA rdata.
 */
uint16_t *zparser_conv_aaaa(const char *text);

/*!
 * \brief Converts text string to wireformat.
 *
 * \param text Text string.
 * \param len Length of string.
 */
uint16_t *zparser_conv_text(const char *text, size_t len);

/*!
 * \brief Converts domain name string to wireformat.
 *
 * \param name Domain name string.
 * \param len Length of string.
 */
uint16_t *zparser_conv_dns_name(const uint8_t* name, size_t len);

/*!
 * \brief Converts base32 encoded string to wireformat.
 * TODO consider replacing with our implementation.
 *
 * \param b32 Base32 encoded string.
 */
uint16_t *zparser_conv_b32(const char *b32);

/*!
 * \brief Converts base64 encoded string to wireformat.
 * TODO consider replacing with our implementation.
 *
 * \param b64 Base64 encoded string.
 */
uint16_t *zparser_conv_b64(const char *b64);

/*!
 * \brief Converts RR type string to wireformat.
 *
 * \param rr RR type string.
 */
uint16_t *zparser_conv_rrtype(const char *rr);

/*!
 * \brief
 *
 * \param nxtbits
 */
uint16_t *zparser_conv_nxt(uint8_t *nxtbits);

/*!
 * \brief
 *
 * \param nsecbits[][]
 */
uint16_t *zparser_conv_nsec(uint8_t nsecbits[NSEC_WINDOW_COUNT]
                                           [NSEC_WINDOW_BITS_SIZE]);
/*!
 * \brief
 *
 * \param str
 */
uint16_t *zparser_conv_loc(char *str);

/*!
 * \brief
 *
 * \param algstr
 */
uint16_t *zparser_conv_algorithm(const char *algstr);

/*!
 * \brief
 *
 * \param typestr
 */
uint16_t *zparser_conv_certificate_type(const char *typestr);

/*!
 * \brief
 *
 * \param str
 */
uint16_t *zparser_conv_apl_rdata(char *str);

/*!
 * \brief
 *
 * \param type
 * \param wireformat
 */
void parse_unknown_rdata(uint16_t type, uint16_t *wireformat);

/*!
 * \brief
 *
 * \param ttlstr
 * \param error
 */
uint32_t zparser_ttl2int(const char *ttlstr, int* error);

/*!
 * \brief
 *
 * \param data
 */
void zadd_rdata_wireformat(uint16_t *data);

/*!
 * \brief
 *
 * \param data
 * \param first
 */
void zadd_rdata_txt_wireformat(uint16_t *data, int first);

/*!
 * \brief
 *
 */
void zadd_rdata_txt_clean_wireformat();

/*!
 * \brief
 *
 * \param domain
 */
void zadd_rdata_domain(dnslib_dname_t *domain);

/*!
 * \brief
 *
 * \param bits[][]
 * \param index
 */
void set_bitnsec(uint8_t bits[NSEC_WINDOW_COUNT][NSEC_WINDOW_BITS_SIZE],
		 uint16_t index);

/*!
 * \brief
 *
 * \param data
 * \param size
 */
uint16_t *alloc_rdata_init(const void *data, size_t size);

/*!
 * \brief
 *
 * \param name
 * \param zonefile
 * \param outfile
 */
int zone_read(const char *name, const char *zonefile, const char *outfile);

/* zparser.y */
/*!
 * \brief
 *
 */
zparser_type *zparser_create();
/*!
 * \brief
 *
 * \param filename
 * \param ttl
 * \param rclass
 * \param origin
 */
void zparser_init(const char *filename, uint32_t ttl, uint16_t rclass,
		  dnslib_node_t *origin);

/*!
 * \brief
 *
 */
void zparser_free();

#endif /* _KNOT_ZONEPARSER_H_ */

/*! @} */
