/* hash.c

   Routines for manipulating hash tables... */

/*
 * Copyright (c) 1995-2000 Internet Software Consortium.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of The Internet Software Consortium nor the names
 *    of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INTERNET SOFTWARE CONSORTIUM AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNET SOFTWARE CONSORTIUM OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This software has been written for the Internet Software Consortium
 * by Ted Lemon in cooperation with Vixie Enterprises and Nominum, Inc.
 * To learn more about the Internet Software Consortium, see
 * ``http://www.isc.org/''.  To learn more about Vixie Enterprises,
 * see ``http://www.vix.com''.   To learn more about Nominum, Inc., see
 * ``http://www.nominum.com''.
 */

#ifndef lint
static char copyright[] =
"$Id: hash.c,v 1.19 2000/03/18 02:15:36 mellon Exp $ Copyright (c) 1995-2000 The Internet Software Consortium.  All rights reserved.\n";
#endif /* not lint */

#include "dhcpd.h"
#include <ctype.h>

static int do_hash PROTO ((const unsigned char *, unsigned, unsigned));
static int do_case_hash PROTO ((const unsigned char *, unsigned, unsigned));

struct hash_table *new_hash (hash_reference referencer,
			     hash_dereference dereferencer,
			     int casep)
{
	struct hash_table *rv = new_hash_table (DEFAULT_HASH_SIZE, MDL);
	if (!rv)
		return rv;
	memset (&rv -> buckets [0], 0,
		DEFAULT_HASH_SIZE * sizeof (struct hash_bucket *));
	rv -> referencer = referencer;
	rv -> dereferencer = dereferencer;
	if (casep) {
		rv -> cmp = casecmp;
		rv -> do_hash = do_case_hash;
	} else {
		rv -> cmp = memcmp;
		rv -> do_hash = do_hash;
	}
	return rv;
}

static int do_case_hash (name, len, size)
	const unsigned char *name;
	unsigned len;
	unsigned size;
{
	register int accum = 0;
	register const unsigned char *s = (const unsigned char *)name;
	int i = len;
	register unsigned c;

	while (i--) {
		/* Make the hash case-insensitive. */
		c = *s++;
		if (isascii (c) && isupper (c))
			c = tolower (c);

		/* Add the character in... */
		accum += *s++;
		/* Add carry back in... */
		while (accum > 255) {
			accum = (accum & 255) + (accum >> 8);
		}
	}
	return accum % size;
}

static int do_hash (name, len, size)
	const unsigned char *name;
	unsigned len;
	unsigned size;
{
	register int accum = 0;
	register const unsigned char *s = (const unsigned char *)name;
	int i = len;

	while (i--) {
		/* Add the character in... */
		accum += *s++;
		/* Add carry back in... */
		while (accum > 255) {
			accum = (accum & 255) + (accum >> 8);
		}
	}
	return accum % size;
}

void add_hash (table, name, len, pointer)
	struct hash_table *table;
	unsigned len;
	const unsigned char *name;
	void *pointer;
{
	int hashno;
	struct hash_bucket *bp;
	void *foo;

	if (!table)
		return;

	if (!len)
		len = strlen ((const char *)name);

	hashno = (*table -> do_hash) (name, len, table -> hash_count);
	bp = new_hash_bucket (MDL);

	if (!bp) {
		log_error ("Can't add %s to hash table.", name);
		return;
	}
	bp -> name = name;
	if (table -> referencer) {
		foo = &bp -> value;
		(*(table -> referencer)) (foo, pointer, MDL);
	} else
		bp -> value = pointer;
	bp -> next = table -> buckets [hashno];
	bp -> len = len;
	table -> buckets [hashno] = bp;
}

void delete_hash_entry (table, name, len)
	struct hash_table *table;
	unsigned len;
	const unsigned char *name;
{
	int hashno;
	struct hash_bucket *bp, *pbp = (struct hash_bucket *)0;
	void *foo;

	if (!table)
		return;

	if (!len)
		len = strlen ((const char *)name);

	hashno = (*table -> do_hash) (name, len, table -> hash_count);

	/* Go through the list looking for an entry that matches;
	   if we find it, delete it. */
	for (bp = table -> buckets [hashno]; bp; bp = bp -> next) {
		if ((!bp -> len &&
		     !strcmp ((const char *)bp -> name, (const char *)name)) ||
		    (bp -> len == len &&
		     !(*table -> cmp) (bp -> name, name, len))) {
			if (pbp) {
				pbp -> next = bp -> next;
			} else {
				table -> buckets [hashno] = bp -> next;
			}
			if (table -> dereferencer) {
				foo = &bp -> value;
				(*(table -> dereferencer)) (foo, MDL);
			}
			free_hash_bucket (bp, MDL);
			break;
		}
		pbp = bp;	/* jwg, 9/6/96 - nice catch! */
	}
}

void *hash_lookup (table, name, len)
	struct hash_table *table;
	const unsigned char *name;
	unsigned len;
{
	int hashno;
	struct hash_bucket *bp;

	if (!table)
		return (unsigned char *)0;
	if (!len)
		len = strlen ((const char *)name);

	hashno = (*table -> do_hash) (name, len, table -> hash_count);

	for (bp = table -> buckets [hashno]; bp; bp = bp -> next) {
		if (len == bp -> len
		    && !(*table -> cmp) (bp -> name, name, len))
			return bp -> value;
	}
	return (unsigned char *)0;
}

int casecmp (const void *v1, const void *v2, unsigned len)
{
	unsigned i;
	const char *s = v1;
	const char *t = v2;
	
	for (i = 0; i < len; i++)
	{
		int c1, c2;
		if (isascii (s [i]) && isupper (s [i]))
			c1 = tolower (s [i]);
		else
			c1 = s [i];
		
		if (isascii (t [i]) && isupper (t [i]))
			c2 = tolower (t [i]);
		else
			c2 = t [i];
		
		if (c1 < c2)
			return -1;
		if (c1 > c2)
			return 1;
	}
	return 0;
}
