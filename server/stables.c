/* stables.c

   Tables of information only used by server... */

/*
 * Copyright (c) 1996-1999 Internet Software Consortium.
 * Use is subject to license terms which appear in the file named
 * ISC-LICENSE that should have accompanied this file when you
 * received it.   If a file named ISC-LICENSE did not accompany this
 * file, or you are not sure the one you have is correct, you may
 * obtain an applicable copy of the license at:
 *
 *             http://www.isc.org/isc-license-1.0.html. 
 *
 * This file is part of the ISC DHCP distribution.   The documentation
 * associated with this file is listed in the file DOCUMENTATION,
 * included in the top-level directory of this release.
 *
 * Support and other services are available for ISC products - see
 * http://www.isc.org for more information.
 */

#ifndef lint
static char copyright[] =
"$Id: stables.c,v 1.3 1999/11/23 22:24:49 mellon Exp $ Copyright (c) 1995, 1996, 1998, 1999 The Internet Software Consortium.  All rights reserved.\n";
#endif /* not lint */

#include "dhcpd.h"

#if defined (FAILOVER_PROTOCOL)

/* This is used to indicate some kind of failure when generating a
   failover option. */
failover_option_t null_failover_option = { 0, 0 };

/* Information about failover options, for printing, encoding
   and decoding. */
struct failover_option_info ft_options [] =
{
	{ 0, "unused", FT_UNDEF, 0, 0, 0 },
	{ FTO_BINDING_STATUS, "binding-status",
	  FT_UINT8, 1, FM_OFFSET (binding_status), FTB_BINDING_STATUS },
	{ FTO_ASSIGNED_IP_ADDRESS, "assigned-IP-address",
	  FT_IPADDR, 1, FM_OFFSET (assigned_addr), FTB_ASSIGNED_IP_ADDRESS },
	{ FTO_SERVER_ADDR, "sending-server-IP-address",
	  FT_IPADDR, 1, FM_OFFSET (server_addr), FTB_SERVER_ADDR },
	{ FTO_ADDRESSES_TRANSFERRED, "addresses-transferred",
	  FT_UINT32, 1, FM_OFFSET (addresses_transferred),
	  FTB_ADDRESSES_TRANSFERRED },
	{ FTO_CLIENT_IDENTIFIER, "client-identifier",
	  FT_BYTES, 0, FM_OFFSET (client_identifier), FTB_CLIENT_IDENTIFIER },
	{ FTO_CHADDR, "client-hardware-address",
	  FT_BYTES, 0, FM_OFFSET (chaddr), FTB_CHADDR },
	{ FTO_DDNS, "DDNS",
	  FT_DDNS, 1, FM_OFFSET (ddns), FTB_DDNS },
	{ FTO_REJECT_REASON, "reject-reason",
	  FT_UINT8, 1, FM_OFFSET (reject_reason), FTB_REJECT_REASON },
	{ FTO_MESSAGE, "message",
	  FT_TEXT, 0, FM_OFFSET (message), FTB_MESSAGE },
	{ FTO_MCLT, "MCLT",
	  FT_UINT32, 1, FM_OFFSET (mclt), FTB_MCLT },
	{ FTO_VENDOR_CLASS, "vendor-class-identifier",
	  FT_TEXT_OR_BYTES, 0, FM_OFFSET (vendor_class), FTB_VENDOR_CLASS },
	{ 12, "undefined", FT_UNDEF, 0, 0, 0 },
	{ FTO_LEASE_EXPIRY, "lease-expiration-time",
	  FT_UINT32, 1, FM_OFFSET (expiry), FTB_LEASE_EXPIRY },
	{ FTO_POTENTIAL_EXPIRY, "potential-expiration-time",
	  FT_UINT32, 1, FM_OFFSET (potential_expiry), FTB_POTENTIAL_EXPIRY },
	{ FTO_GRACE_EXPIRY, "grace-expiration-time",
	  FT_UINT32, 1, FM_OFFSET (grace_expiry), FTB_GRACE_EXPIRY },
	{ FTO_CLTT, "client-last-transaction-time",
	  FT_UINT32, 1, FM_OFFSET (client_ltt), FTB_CLTT },
	{ FTO_STOS, "start-time-of-state",
	  FT_UINT32, 1, FM_OFFSET (stos), FTB_STOS },
	{ FTO_SERVER_STATE, "server-state",
	  FT_UINT8, 1, FM_OFFSET (server_state), FTB_SERVER_STATE },
	{ FTO_SERVER_FLAGS, "server-flags",
	  FT_UINT8, 1, FM_OFFSET (server_flags), FTB_SERVER_FLAGS },
	{ FTO_VENDOR_OPTIONS, "vendor-specific-options",
	  FT_BYTES, 0, FM_OFFSET (vendor_options), FTB_VENDOR_OPTIONS },
	{ FTO_MAX_UNACKED, "max-unacked-bndupd",
	  FT_UINT32, 1, FM_OFFSET (max_unacked), FTB_MAX_UNACKED },
	{ 22, "undefined", FT_UNDEF, 0, 0 },
	{ FTO_RECEIVE_TIMER, "receive-timer",
	  FT_UINT32, 1, FM_OFFSET (receive_timer), FTB_RECEIVE_TIMER },
	{ FTO_HBA, "hash-bucket-assignment",
	  FT_BYTES, 0, FM_OFFSET (hba), FTB_HBA },
	{ FTO_MESSAGE_DIGEST, "message-digest",
	  FT_DIGEST, 0, 0, FTB_MESSAGE_DIGEST },
	{ FTO_PROTOCOL_VERSION, "protocol-version", 
	  FT_UINT8, 1, FM_OFFSET (protocol_version), FTB_PROTOCOL_VERSION },
	{ FTO_TLS_REQUEST, "TLS-request",
	  FT_UINT8, 2, FM_OFFSET (tls_request), FTB_TLS_REQUEST },
	{ FTO_TLS_REPLY, "TLS-reply",
	  FT_BYTES, 1, FM_OFFSET (tls_reply ), FTB_TLS_REPLY },
	{ FTO_REQUEST_OPTIONS, "client-request-options",
	  FT_BYTES, 0, FM_OFFSET (request_options), FTB_REQUEST_OPTIONS },
	{ FTO_REPLY_OPTIONS, "client-reply-options",
	  FT_BYTES, 0, FM_OFFSET (reply_options), FTB_REPLY_OPTIONS }
};

/* These are really options that make sense for a particular request - if
   some other option comes in, we're not going to use it, so we can just
   discard it.  Note that the message-digest option is allowed for all
   message types, but is not saved - it's just used to validate the message
   and then discarded - so it's not mentioned here. */

u_int32_t fto_allowed [] = {
	0,	/* 0 unused */
	0,	/* 1 POOLREQ */
	FTB_ADDRESSES_TRANSFERRED, /* 2 POOLRESP */
	(FTB_ASSIGNED_IP_ADDRESS | FTB_BINDING_STATUS | FTB_CLIENT_IDENTIFIER |
	 FTB_CHADDR | FTB_LEASE_EXPIRY | FTB_POTENTIAL_EXPIRY | FTB_STOS |
	 FTB_CLTT | FTB_REQUEST_OPTIONS | FTB_REPLY_OPTIONS), /* 3 BNDUPD */
	(FTB_ASSIGNED_IP_ADDRESS | FTB_BINDING_STATUS | FTB_CLIENT_IDENTIFIER |
	 FTB_CHADDR | FTB_LEASE_EXPIRY | FTB_POTENTIAL_EXPIRY | FTB_STOS |
	 FTB_CLTT | FTB_REQUEST_OPTIONS | FTB_REPLY_OPTIONS), /* 4 BNDACK */
	(FTB_SERVER_ADDR | FTB_MAX_UNACKED | FTB_RECEIVE_TIMER |
	 FTB_VENDOR_CLASS | FTB_PROTOCOL_VERSION | FTB_TLS_REQUEST |
	 FTB_MCLT | FTB_HBA), /* 5 CONNECT */
	(FTB_SERVER_ADDR | FTB_MAX_UNACKED | FTB_RECEIVE_TIMER |
	 FTB_VENDOR_CLASS | FTB_PROTOCOL_VERSION | FTB_TLS_REPLY |
	 FTB_REJECT_REASON | FTB_MESSAGE), /* CONNECTACK */
	0,	/* 7 UPDREQ */
	0,	/* 8 UPDDONE */
	0,	/* 9 UPDREQALL */
	(FTB_SERVER_STATE | FTB_SERVER_FLAGS | FTB_STOS), /* 10 STATE */
	0,	/* 11 CONTACT */
	(FTB_REJECT_REASON | FTB_MESSAGE) /* 12 DISCONNECT */
};

/* Sizes of the various types. */
int ft_sizes [] = {
	1, /* FT_UINT8 */
	4, /* FT_IPADDR */
	4, /* FT_UINT32 */
	1, /* FT_BYTES */
	1, /* FT_TEXT_OR_BYTES */
	0, /* FT_DDNS */
	0, /* FT_DDNS1 */
	2, /* FT_UINT16 */
	1, /* FT_TEXT */
	0, /* FT_UNDEF */
	0, /* FT_DIGEST */
};

/* Names of the various failover link states. */
const char *dhcp_flink_state_names [] = {
	"invalid state 0",
	"startup",
	"message length wait",
	"message wait",
	"disconnected"
};
#endif /* FAILOVER_PROTOCOL */

struct universe agent_universe;
struct option agent_options [256] = {
	{ "pad", "",					&agent_universe, 0 },
	{ "circuit-id", "X",				&agent_universe, 1 },
	{ "remote-id", "X",				&agent_universe, 2 },
	{ "agent-id", "I",				&agent_universe, 3 },
	{ "option-4", "X",				&agent_universe, 4 },
	{ "option-5", "X",				&agent_universe, 5 },
	{ "option-6", "X",				&agent_universe, 6 },
	{ "option-7", "X",				&agent_universe, 7 },
	{ "option-8", "X",				&agent_universe, 8 },
	{ "option-9", "X",				&agent_universe, 9 },
	{ "option-10", "X",				&agent_universe, 10 },
	{ "option-11", "X",				&agent_universe, 11 },
	{ "option-12", "X",				&agent_universe, 12 },
	{ "option-13", "X",				&agent_universe, 13 },
	{ "option-14", "X",				&agent_universe, 14 },
	{ "option-15", "X",				&agent_universe, 15 },
	{ "option-16", "X",				&agent_universe, 16 },
	{ "option-17", "X",				&agent_universe, 17 },
	{ "option-18", "X",				&agent_universe, 18 },
	{ "option-19", "X",				&agent_universe, 19 },
	{ "option-20", "X",				&agent_universe, 20 },
	{ "option-21", "X",				&agent_universe, 21 },
	{ "option-22", "X",				&agent_universe, 22 },
	{ "option-23", "X",				&agent_universe, 23 },
	{ "option-24", "X",				&agent_universe, 24 },
	{ "option-25", "X",				&agent_universe, 25 },
	{ "option-26", "X",				&agent_universe, 26 },
	{ "option-27", "X",				&agent_universe, 27 },
	{ "option-28", "X",				&agent_universe, 28 },
	{ "option-29", "X",				&agent_universe, 29 },
	{ "option-30", "X",				&agent_universe, 30 },
	{ "option-31", "X",				&agent_universe, 31 },
	{ "option-32", "X",				&agent_universe, 32 },
	{ "option-33", "X",				&agent_universe, 33 },
	{ "option-34", "X",				&agent_universe, 34 },
	{ "option-35", "X",				&agent_universe, 35 },
	{ "option-36", "X",				&agent_universe, 36 },
	{ "option-37", "X",				&agent_universe, 37 },
	{ "option-38", "X",				&agent_universe, 38 },
	{ "option-39", "X",				&agent_universe, 39 },
	{ "option-40", "X",				&agent_universe, 40 },
	{ "option-41", "X",				&agent_universe, 41 },
	{ "option-42", "X",				&agent_universe, 42 },
	{ "option-43", "X",				&agent_universe, 43 },
	{ "option-44", "X",				&agent_universe, 44 },
	{ "option-45", "X",				&agent_universe, 45 },
	{ "option-46", "X",				&agent_universe, 46 },
	{ "option-47", "X",				&agent_universe, 47 },
	{ "option-48", "X",				&agent_universe, 48 },
	{ "option-49", "X",				&agent_universe, 49 },
	{ "option-50", "X",				&agent_universe, 50 },
	{ "option-51", "X",				&agent_universe, 51 },
	{ "option-52", "X",				&agent_universe, 52 },
	{ "option-53", "X",				&agent_universe, 53 },
	{ "option-54", "X",				&agent_universe, 54 },
	{ "option-55", "X",				&agent_universe, 55 },
	{ "option-56", "X",				&agent_universe, 56 },
	{ "option-57", "X",				&agent_universe, 57 },
	{ "option-58", "X",				&agent_universe, 58 },
	{ "option-59", "X",				&agent_universe, 59 },
	{ "option-60", "X",				&agent_universe, 60 },
	{ "option-61", "X",				&agent_universe, 61 },
	{ "option-62", "X",				&agent_universe, 62 },
	{ "option-63", "X",				&agent_universe, 63 },
	{ "option-64", "X",				&agent_universe, 64 },
	{ "option-65", "X",				&agent_universe, 65 },
	{ "option-66", "X",				&agent_universe, 66 },
	{ "option-67", "X",				&agent_universe, 67 },
	{ "option-68", "X",				&agent_universe, 68 },
	{ "option-69", "X",				&agent_universe, 69 },
	{ "option-70", "X",				&agent_universe, 70 },
	{ "option-71", "X",				&agent_universe, 71 },
	{ "option-72", "X",				&agent_universe, 72 },
	{ "option-73", "X",				&agent_universe, 73 },
	{ "option-74", "X",				&agent_universe, 74 },
	{ "option-75", "X",				&agent_universe, 75 },
	{ "option-76", "X",				&agent_universe, 76 },
	{ "option-77", "X",				&agent_universe, 77 },
	{ "option-78", "X",				&agent_universe, 78 },
	{ "option-79", "X",				&agent_universe, 79 },
	{ "option-80", "X",				&agent_universe, 80 },
	{ "option-81", "X",				&agent_universe, 81 },
	{ "option-82", "X",				&agent_universe, 82 },
	{ "option-83", "X",				&agent_universe, 83 },
	{ "option-84", "X",				&agent_universe, 84 },
	{ "option-85", "X",				&agent_universe, 85 },
	{ "option-86", "X",				&agent_universe, 86 },
	{ "option-87", "X",				&agent_universe, 87 },
	{ "option-88", "X",				&agent_universe, 88 },
	{ "option-89", "X",				&agent_universe, 89 },
	{ "option-90", "X",				&agent_universe, 90 },
	{ "option-91", "X",				&agent_universe, 91 },
	{ "option-92", "X",				&agent_universe, 92 },
	{ "option-93", "X",				&agent_universe, 93 },
	{ "option-94", "X",				&agent_universe, 94 },
	{ "option-95", "X",				&agent_universe, 95 },
	{ "option-96", "X",				&agent_universe, 96 },
	{ "option-97", "X",				&agent_universe, 97 },
	{ "option-98", "X",				&agent_universe, 98 },
	{ "option-99", "X",				&agent_universe, 99 },
	{ "option-100", "X",				&agent_universe, 100 },
	{ "option-101", "X",				&agent_universe, 101 },
	{ "option-102", "X",				&agent_universe, 102 },
	{ "option-103", "X",				&agent_universe, 103 },
	{ "option-104", "X",				&agent_universe, 104 },
	{ "option-105", "X",				&agent_universe, 105 },
	{ "option-106", "X",				&agent_universe, 106 },
	{ "option-107", "X",				&agent_universe, 107 },
	{ "option-108", "X",				&agent_universe, 108 },
	{ "option-109", "X",				&agent_universe, 109 },
	{ "option-110", "X",				&agent_universe, 110 },
	{ "option-111", "X",				&agent_universe, 111 },
	{ "option-112", "X",				&agent_universe, 112 },
	{ "option-113", "X",				&agent_universe, 113 },
	{ "option-114", "X",				&agent_universe, 114 },
	{ "option-115", "X",				&agent_universe, 115 },
	{ "option-116", "X",				&agent_universe, 116 },
	{ "option-117", "X",				&agent_universe, 117 },
	{ "option-118", "X",				&agent_universe, 118 },
	{ "option-119", "X",				&agent_universe, 119 },
	{ "option-120", "X",				&agent_universe, 120 },
	{ "option-121", "X",				&agent_universe, 121 },
	{ "option-122", "X",				&agent_universe, 122 },
	{ "option-123", "X",				&agent_universe, 123 },
	{ "option-124", "X",				&agent_universe, 124 },
	{ "option-125", "X",				&agent_universe, 125 },
	{ "option-126", "X",				&agent_universe, 126 },
	{ "option-127", "X",				&agent_universe, 127 },
	{ "option-128", "X",				&agent_universe, 128 },
	{ "option-129", "X",				&agent_universe, 129 },
	{ "option-130", "X",				&agent_universe, 130 },
	{ "option-131", "X",				&agent_universe, 131 },
	{ "option-132", "X",				&agent_universe, 132 },
	{ "option-133", "X",				&agent_universe, 133 },
	{ "option-134", "X",				&agent_universe, 134 },
	{ "option-135", "X",				&agent_universe, 135 },
	{ "option-136", "X",				&agent_universe, 136 },
	{ "option-137", "X",				&agent_universe, 137 },
	{ "option-138", "X",				&agent_universe, 138 },
	{ "option-139", "X",				&agent_universe, 139 },
	{ "option-140", "X",				&agent_universe, 140 },
	{ "option-141", "X",				&agent_universe, 141 },
	{ "option-142", "X",				&agent_universe, 142 },
	{ "option-143", "X",				&agent_universe, 143 },
	{ "option-144", "X",				&agent_universe, 144 },
	{ "option-145", "X",				&agent_universe, 145 },
	{ "option-146", "X",				&agent_universe, 146 },
	{ "option-147", "X",				&agent_universe, 147 },
	{ "option-148", "X",				&agent_universe, 148 },
	{ "option-149", "X",				&agent_universe, 149 },
	{ "option-150", "X",				&agent_universe, 150 },
	{ "option-151", "X",				&agent_universe, 151 },
	{ "option-152", "X",				&agent_universe, 152 },
	{ "option-153", "X",				&agent_universe, 153 },
	{ "option-154", "X",				&agent_universe, 154 },
	{ "option-155", "X",				&agent_universe, 155 },
	{ "option-156", "X",				&agent_universe, 156 },
	{ "option-157", "X",				&agent_universe, 157 },
	{ "option-158", "X",				&agent_universe, 158 },
	{ "option-159", "X",				&agent_universe, 159 },
	{ "option-160", "X",				&agent_universe, 160 },
	{ "option-161", "X",				&agent_universe, 161 },
	{ "option-162", "X",				&agent_universe, 162 },
	{ "option-163", "X",				&agent_universe, 163 },
	{ "option-164", "X",				&agent_universe, 164 },
	{ "option-165", "X",				&agent_universe, 165 },
	{ "option-166", "X",				&agent_universe, 166 },
	{ "option-167", "X",				&agent_universe, 167 },
	{ "option-168", "X",				&agent_universe, 168 },
	{ "option-169", "X",				&agent_universe, 169 },
	{ "option-170", "X",				&agent_universe, 170 },
	{ "option-171", "X",				&agent_universe, 171 },
	{ "option-172", "X",				&agent_universe, 172 },
	{ "option-173", "X",				&agent_universe, 173 },
	{ "option-174", "X",				&agent_universe, 174 },
	{ "option-175", "X",				&agent_universe, 175 },
	{ "option-176", "X",				&agent_universe, 176 },
	{ "option-177", "X",				&agent_universe, 177 },
	{ "option-178", "X",				&agent_universe, 178 },
	{ "option-179", "X",				&agent_universe, 179 },
	{ "option-180", "X",				&agent_universe, 180 },
	{ "option-181", "X",				&agent_universe, 181 },
	{ "option-182", "X",				&agent_universe, 182 },
	{ "option-183", "X",				&agent_universe, 183 },
	{ "option-184", "X",				&agent_universe, 184 },
	{ "option-185", "X",				&agent_universe, 185 },
	{ "option-186", "X",				&agent_universe, 186 },
	{ "option-187", "X",				&agent_universe, 187 },
	{ "option-188", "X",				&agent_universe, 188 },
	{ "option-189", "X",				&agent_universe, 189 },
	{ "option-190", "X",				&agent_universe, 190 },
	{ "option-191", "X",				&agent_universe, 191 },
	{ "option-192", "X",				&agent_universe, 192 },
	{ "option-193", "X",				&agent_universe, 193 },
	{ "option-194", "X",				&agent_universe, 194 },
	{ "option-195", "X",				&agent_universe, 195 },
	{ "option-196", "X",				&agent_universe, 196 },
	{ "option-197", "X",				&agent_universe, 197 },
	{ "option-198", "X",				&agent_universe, 198 },
	{ "option-199", "X",				&agent_universe, 199 },
	{ "option-200", "X",				&agent_universe, 200 },
	{ "option-201", "X",				&agent_universe, 201 },
	{ "option-202", "X",				&agent_universe, 202 },
	{ "option-203", "X",				&agent_universe, 203 },
	{ "option-204", "X",				&agent_universe, 204 },
	{ "option-205", "X",				&agent_universe, 205 },
	{ "option-206", "X",				&agent_universe, 206 },
	{ "option-207", "X",				&agent_universe, 207 },
	{ "option-208", "X",				&agent_universe, 208 },
	{ "option-209", "X",				&agent_universe, 209 },
	{ "option-210", "X",				&agent_universe, 210 },
	{ "option-211", "X",				&agent_universe, 211 },
	{ "option-212", "X",				&agent_universe, 212 },
	{ "option-213", "X",				&agent_universe, 213 },
	{ "option-214", "X",				&agent_universe, 214 },
	{ "option-215", "X",				&agent_universe, 215 },
	{ "option-216", "X",				&agent_universe, 216 },
	{ "option-217", "X",				&agent_universe, 217 },
	{ "option-218", "X",				&agent_universe, 218 },
	{ "option-219", "X",				&agent_universe, 219 },
	{ "option-220", "X",				&agent_universe, 220 },
	{ "option-221", "X",				&agent_universe, 221 },
	{ "option-222", "X",				&agent_universe, 222 },
	{ "option-223", "X",				&agent_universe, 223 },
	{ "option-224", "X",				&agent_universe, 224 },
	{ "option-225", "X",				&agent_universe, 225 },
	{ "option-226", "X",				&agent_universe, 226 },
	{ "option-227", "X",				&agent_universe, 227 },
	{ "option-228", "X",				&agent_universe, 228 },
	{ "option-229", "X",				&agent_universe, 229 },
	{ "option-230", "X",				&agent_universe, 230 },
	{ "option-231", "X",				&agent_universe, 231 },
	{ "option-232", "X",				&agent_universe, 232 },
	{ "option-233", "X",				&agent_universe, 233 },
	{ "option-234", "X",				&agent_universe, 234 },
	{ "option-235", "X",				&agent_universe, 235 },
	{ "option-236", "X",				&agent_universe, 236 },
	{ "option-237", "X",				&agent_universe, 237 },
	{ "option-238", "X",				&agent_universe, 238 },
	{ "option-239", "X",				&agent_universe, 239 },
	{ "option-240", "X",				&agent_universe, 240 },
	{ "option-241", "X",				&agent_universe, 241 },
	{ "option-242", "X",				&agent_universe, 242 },
	{ "option-243", "X",				&agent_universe, 243 },
	{ "option-244", "X",				&agent_universe, 244 },
	{ "option-245", "X",				&agent_universe, 245 },
	{ "option-246", "X",				&agent_universe, 246 },
	{ "option-247", "X",				&agent_universe, 247 },
	{ "option-248", "X",				&agent_universe, 248 },
	{ "option-249", "X",				&agent_universe, 249 },
	{ "option-250", "X",				&agent_universe, 250 },
	{ "option-251", "X",				&agent_universe, 251 },
	{ "option-252", "X",				&agent_universe, 252 },
	{ "option-253", "X",				&agent_universe, 253 },
	{ "option-254", "X",				&agent_universe, 254 },
	{ "option-end", "e",				&agent_universe, 255 },
};

struct universe server_universe;
struct option server_options [256] = {
	{ "pad", "",				&server_universe, 0 },
	{ "default-lease-time", "L",		&server_universe, 1 },
	{ "max-lease-time", "L",		&server_universe, 2 },
	{ "min-lease-time", "L",		&server_universe, 3 },
	{ "dynamic-bootp-lease-cutoff", "L",	&server_universe, 4 },
	{ "dynamic-bootp-lease-length", "L",	&server_universe, 5 },
	{ "boot-unknown-clients", "f",		&server_universe, 6 },
	{ "dynamic-bootp", "f",			&server_universe, 7 },
	{ "allow-bootp", "f",			&server_universe, 8 },
	{ "allow-booting", "f",			&server_universe, 9 },
	{ "one-lease-per-client", "f",		&server_universe, 10 },
	{ "get-lease-hostnames", "f",		&server_universe, 11 },
	{ "use-host-decl-names", "f",		&server_universe, 12 },
	{ "use-lease-addr-for-default-route", "f", &server_universe, 13 },
	{ "min-secs", "B",			&server_universe, 14 },
	{ "filename", "t",			&server_universe, 15 },
	{ "server-name", "t",			&server_universe, 16 },
	{ "next-server", "I",			&server_universe, 17 },
	{ "authoritative", "f",			&server_universe, 18 },
	{ "vendor-option-space", "U",		&server_universe, 19 },
	{ "always-reply-rfc1048", "f",		&server_universe, 20 },
	{ "site-option-space", "X",		&server_universe, 21 },
	{ "always-broadcast", "f",		&server_universe, 22 },
	{ "ddns-domainname", "t",		&server_universe, 23 },
	{ "ddns-hostname", "t",			&server_universe, 24 },
	{ "ddns-rev-domainname", "t",		&server_universe, 25 },
	{ "lease-file-name", "t",		&server_universe, 26 },
	{ "pid-file-name", "t",			&server_universe, 27 },
	{ "duplicates", "f",			&server_universe, 28 },
	{ "declines", "f",			&server_universe, 29 },
	{ "option-30", "X",			&server_universe, 30 },
	{ "option-31", "X",			&server_universe, 31 },
	{ "option-32", "X",			&server_universe, 32 },
	{ "option-33", "X",			&server_universe, 33 },
	{ "option-34", "X",			&server_universe, 34 },
	{ "option-35", "X",			&server_universe, 35 },
	{ "option-36", "X",			&server_universe, 36 },
	{ "option-37", "X",			&server_universe, 37 },
	{ "option-38", "X",			&server_universe, 38 },
	{ "option-39", "X",			&server_universe, 39 },
	{ "option-40", "X",			&server_universe, 40 },
	{ "option-41", "X",			&server_universe, 41 },
	{ "option-42", "X",			&server_universe, 42 },
	{ "option-43", "X",			&server_universe, 43 },
	{ "option-44", "X",			&server_universe, 44 },
	{ "option-45", "X",			&server_universe, 45 },
	{ "option-46", "X",			&server_universe, 46 },
	{ "option-47", "X",			&server_universe, 47 },
	{ "option-48", "X",			&server_universe, 48 },
	{ "option-49", "X",			&server_universe, 49 },
	{ "option-50", "X",			&server_universe, 50 },
	{ "option-51", "X",			&server_universe, 51 },
	{ "option-52", "X",			&server_universe, 52 },
	{ "option-53", "X",			&server_universe, 53 },
	{ "option-54", "X",			&server_universe, 54 },
	{ "option-55", "X",			&server_universe, 55 },
	{ "option-56", "X",			&server_universe, 56 },
	{ "option-57", "X",			&server_universe, 57 },
	{ "option-58", "X",			&server_universe, 58 },
	{ "option-59", "X",			&server_universe, 59 },
	{ "option-60", "X",			&server_universe, 60 },
	{ "option-61", "X",			&server_universe, 61 },
	{ "option-62", "X",			&server_universe, 62 },
	{ "option-63", "X",			&server_universe, 63 },
	{ "option-64", "X",			&server_universe, 64 },
	{ "option-65", "X",			&server_universe, 65 },
	{ "option-66", "X",			&server_universe, 66 },
	{ "option-67", "X",			&server_universe, 67 },
	{ "option-68", "X",			&server_universe, 68 },
	{ "option-69", "X",			&server_universe, 69 },
	{ "option-70", "X",			&server_universe, 70 },
	{ "option-71", "X",			&server_universe, 71 },
	{ "option-72", "X",			&server_universe, 72 },
	{ "option-73", "X",			&server_universe, 73 },
	{ "option-74", "X",			&server_universe, 74 },
	{ "option-75", "X",			&server_universe, 75 },
	{ "option-76", "X",			&server_universe, 76 },
	{ "option-77", "X",			&server_universe, 77 },
	{ "option-78", "X",			&server_universe, 78 },
	{ "option-79", "X",			&server_universe, 79 },
	{ "option-80", "X",			&server_universe, 80 },
	{ "option-81", "X",			&server_universe, 81 },
	{ "option-82", "X",			&server_universe, 82 },
	{ "option-83", "X",			&server_universe, 83 },
	{ "option-84", "X",			&server_universe, 84 },
	{ "option-85", "X",			&server_universe, 85 },
	{ "option-86", "X",			&server_universe, 86 },
	{ "option-87", "X",			&server_universe, 87 },
	{ "option-88", "X",			&server_universe, 88 },
	{ "option-89", "X",			&server_universe, 89 },
	{ "option-90", "X",			&server_universe, 90 },
	{ "option-91", "X",			&server_universe, 91 },
	{ "option-92", "X",			&server_universe, 92 },
	{ "option-93", "X",			&server_universe, 93 },
	{ "option-94", "X",			&server_universe, 94 },
	{ "option-95", "X",			&server_universe, 95 },
	{ "option-96", "X",			&server_universe, 96 },
	{ "option-97", "X",			&server_universe, 97 },
	{ "option-98", "X",			&server_universe, 98 },
	{ "option-99", "X",			&server_universe, 99 },
	{ "option-100", "X",			&server_universe, 100 },
	{ "option-101", "X",			&server_universe, 101 },
	{ "option-102", "X",			&server_universe, 102 },
	{ "option-103", "X",			&server_universe, 103 },
	{ "option-104", "X",			&server_universe, 104 },
	{ "option-105", "X",			&server_universe, 105 },
	{ "option-106", "X",			&server_universe, 106 },
	{ "option-107", "X",			&server_universe, 107 },
	{ "option-108", "X",			&server_universe, 108 },
	{ "option-109", "X",			&server_universe, 109 },
	{ "option-110", "X",			&server_universe, 110 },
	{ "option-111", "X",			&server_universe, 111 },
	{ "option-112", "X",			&server_universe, 112 },
	{ "option-113", "X",			&server_universe, 113 },
	{ "option-114", "X",			&server_universe, 114 },
	{ "option-115", "X",			&server_universe, 115 },
	{ "option-116", "X",			&server_universe, 116 },
	{ "option-117", "X",			&server_universe, 117 },
	{ "option-118", "X",			&server_universe, 118 },
	{ "option-119", "X",			&server_universe, 119 },
	{ "option-120", "X",			&server_universe, 120 },
	{ "option-121", "X",			&server_universe, 121 },
	{ "option-122", "X",			&server_universe, 122 },
	{ "option-123", "X",			&server_universe, 123 },
	{ "option-124", "X",			&server_universe, 124 },
	{ "option-125", "X",			&server_universe, 125 },
	{ "option-126", "X",			&server_universe, 126 },
	{ "option-127", "X",			&server_universe, 127 },
	{ "option-128", "X",			&server_universe, 128 },
	{ "option-129", "X",			&server_universe, 129 },
	{ "option-130", "X",			&server_universe, 130 },
	{ "option-131", "X",			&server_universe, 131 },
	{ "option-132", "X",			&server_universe, 132 },
	{ "option-133", "X",			&server_universe, 133 },
	{ "option-134", "X",			&server_universe, 134 },
	{ "option-135", "X",			&server_universe, 135 },
	{ "option-136", "X",			&server_universe, 136 },
	{ "option-137", "X",			&server_universe, 137 },
	{ "option-138", "X",			&server_universe, 138 },
	{ "option-139", "X",			&server_universe, 139 },
	{ "option-140", "X",			&server_universe, 140 },
	{ "option-141", "X",			&server_universe, 141 },
	{ "option-142", "X",			&server_universe, 142 },
	{ "option-143", "X",			&server_universe, 143 },
	{ "option-144", "X",			&server_universe, 144 },
	{ "option-145", "X",			&server_universe, 145 },
	{ "option-146", "X",			&server_universe, 146 },
	{ "option-147", "X",			&server_universe, 147 },
	{ "option-148", "X",			&server_universe, 148 },
	{ "option-149", "X",			&server_universe, 149 },
	{ "option-150", "X",			&server_universe, 150 },
	{ "option-151", "X",			&server_universe, 151 },
	{ "option-152", "X",			&server_universe, 152 },
	{ "option-153", "X",			&server_universe, 153 },
	{ "option-154", "X",			&server_universe, 154 },
	{ "option-155", "X",			&server_universe, 155 },
	{ "option-156", "X",			&server_universe, 156 },
	{ "option-157", "X",			&server_universe, 157 },
	{ "option-158", "X",			&server_universe, 158 },
	{ "option-159", "X",			&server_universe, 159 },
	{ "option-160", "X",			&server_universe, 160 },
	{ "option-161", "X",			&server_universe, 161 },
	{ "option-162", "X",			&server_universe, 162 },
	{ "option-163", "X",			&server_universe, 163 },
	{ "option-164", "X",			&server_universe, 164 },
	{ "option-165", "X",			&server_universe, 165 },
	{ "option-166", "X",			&server_universe, 166 },
	{ "option-167", "X",			&server_universe, 167 },
	{ "option-168", "X",			&server_universe, 168 },
	{ "option-169", "X",			&server_universe, 169 },
	{ "option-170", "X",			&server_universe, 170 },
	{ "option-171", "X",			&server_universe, 171 },
	{ "option-172", "X",			&server_universe, 172 },
	{ "option-173", "X",			&server_universe, 173 },
	{ "option-174", "X",			&server_universe, 174 },
	{ "option-175", "X",			&server_universe, 175 },
	{ "option-176", "X",			&server_universe, 176 },
	{ "option-177", "X",			&server_universe, 177 },
	{ "option-178", "X",			&server_universe, 178 },
	{ "option-179", "X",			&server_universe, 179 },
	{ "option-180", "X",			&server_universe, 180 },
	{ "option-181", "X",			&server_universe, 181 },
	{ "option-182", "X",			&server_universe, 182 },
	{ "option-183", "X",			&server_universe, 183 },
	{ "option-184", "X",			&server_universe, 184 },
	{ "option-185", "X",			&server_universe, 185 },
	{ "option-186", "X",			&server_universe, 186 },
	{ "option-187", "X",			&server_universe, 187 },
	{ "option-188", "X",			&server_universe, 188 },
	{ "option-189", "X",			&server_universe, 189 },
	{ "option-190", "X",			&server_universe, 190 },
	{ "option-191", "X",			&server_universe, 191 },
	{ "option-192", "X",			&server_universe, 192 },
	{ "option-193", "X",			&server_universe, 193 },
	{ "option-194", "X",			&server_universe, 194 },
	{ "option-195", "X",			&server_universe, 195 },
	{ "option-196", "X",			&server_universe, 196 },
	{ "option-197", "X",			&server_universe, 197 },
	{ "option-198", "X",			&server_universe, 198 },
	{ "option-199", "X",			&server_universe, 199 },
	{ "option-200", "X",			&server_universe, 200 },
	{ "option-201", "X",			&server_universe, 201 },
	{ "option-202", "X",			&server_universe, 202 },
	{ "option-203", "X",			&server_universe, 203 },
	{ "option-204", "X",			&server_universe, 204 },
	{ "option-205", "X",			&server_universe, 205 },
	{ "option-206", "X",			&server_universe, 206 },
	{ "option-207", "X",			&server_universe, 207 },
	{ "option-208", "X",			&server_universe, 208 },
	{ "option-209", "X",			&server_universe, 209 },
	{ "option-210", "X",			&server_universe, 210 },
	{ "option-211", "X",			&server_universe, 211 },
	{ "option-212", "X",			&server_universe, 212 },
	{ "option-213", "X",			&server_universe, 213 },
	{ "option-214", "X",			&server_universe, 214 },
	{ "option-215", "X",			&server_universe, 215 },
	{ "option-216", "X",			&server_universe, 216 },
	{ "option-217", "X",			&server_universe, 217 },
	{ "option-218", "X",			&server_universe, 218 },
	{ "option-219", "X",			&server_universe, 219 },
	{ "option-220", "X",			&server_universe, 220 },
	{ "option-221", "X",			&server_universe, 221 },
	{ "option-222", "X",			&server_universe, 222 },
	{ "option-223", "X",			&server_universe, 223 },
	{ "option-224", "X",			&server_universe, 224 },
	{ "option-225", "X",			&server_universe, 225 },
	{ "option-226", "X",			&server_universe, 226 },
	{ "option-227", "X",			&server_universe, 227 },
	{ "option-228", "X",			&server_universe, 228 },
	{ "option-229", "X",			&server_universe, 229 },
	{ "option-230", "X",			&server_universe, 230 },
	{ "option-231", "X",			&server_universe, 231 },
	{ "option-232", "X",			&server_universe, 232 },
	{ "option-233", "X",			&server_universe, 233 },
	{ "option-234", "X",			&server_universe, 234 },
	{ "option-235", "X",			&server_universe, 235 },
	{ "option-236", "X",			&server_universe, 236 },
	{ "option-237", "X",			&server_universe, 237 },
	{ "option-238", "X",			&server_universe, 238 },
	{ "option-239", "X",			&server_universe, 239 },
	{ "option-240", "X",			&server_universe, 240 },
	{ "option-241", "X",			&server_universe, 241 },
	{ "option-242", "X",			&server_universe, 242 },
	{ "option-243", "X",			&server_universe, 243 },
	{ "option-244", "X",			&server_universe, 244 },
	{ "option-245", "X",			&server_universe, 245 },
	{ "option-246", "X",			&server_universe, 246 },
	{ "option-247", "X",			&server_universe, 247 },
	{ "option-248", "X",			&server_universe, 248 },
	{ "option-249", "X",			&server_universe, 249 },
	{ "option-250", "X",			&server_universe, 250 },
	{ "option-251", "X",			&server_universe, 251 },
	{ "option-252", "X",			&server_universe, 252 },
	{ "option-253", "X",			&server_universe, 253 },
	{ "option-254", "X",			&server_universe, 254 },
	{ "option-end", "e",			&server_universe, 255 },
};

void initialize_server_option_spaces()
{
	int i;

	/* Set up the Relay Agent Information Option suboption space... */
	agent_universe.name = "agent";
	agent_universe.option_state_dereference =
		agent_option_state_dereference;
	agent_universe.get_func = agent_option_get;
	agent_universe.index = universe_count++;
	agent_universe.length_size = 1;
	agent_universe.tag_size = 1;
	agent_universe.store_tag = putUChar;
	agent_universe.store_length = putUChar;
	universes [agent_universe.index] = &agent_universe;
	agent_universe.hash = new_hash ();
	if (!agent_universe.hash)
		log_fatal ("Can't allocate agent option hash table.");
	for (i = 0; i < 256; i++) {
		agent_universe.options [i] = &agent_options [i];
		add_hash (agent_universe.hash,
			  (const unsigned char *)agent_options [i].name, 0,
			  (unsigned char *)&agent_options [i]);
	}

	/* Set up the server option universe... */
	server_universe.name = "server";
	server_universe.lookup_func = lookup_hashed_option;
	server_universe.option_state_dereference =
		hashed_option_state_dereference;
	server_universe.get_func = hashed_option_get;
	server_universe.set_func = hashed_option_set;
	server_universe.save_func = save_hashed_option;
	server_universe.delete_func = delete_hashed_option;
	server_universe.encapsulate = hashed_option_space_encapsulate;
	server_universe.length_size = 1;
	server_universe.tag_size = 1;
	server_universe.store_tag = putUChar;
	server_universe.store_length = putUChar;
	server_universe.index = universe_count++;
	universes [server_universe.index] = &server_universe;
	server_universe.hash = new_hash ();
	if (!server_universe.hash)
		log_fatal ("Can't allocate server option hash table.");
	for (i = 0; i < 256; i++) {
		server_universe.options [i] = &server_options [i];
		add_hash (server_universe.hash,
			  (const unsigned char *)server_options [i].name, 0,
			  (unsigned char *)&server_options [i]);
	}

	/* Add the server and agent option spaces to the option space hash. */
	add_hash (&universe_hash,
		  (const unsigned char *)agent_universe.name, 0,
		  (unsigned char *)&agent_universe);
	add_hash (&universe_hash,
		  (const unsigned char *)server_universe.name, 0,
		  (unsigned char *)&server_universe);
}