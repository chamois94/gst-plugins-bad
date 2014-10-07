/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * The Original Code is Fluendo MPEG Demuxer plugin.
 *
 * The Initial Developer of the Original Code is Fluendo, S.L.
 * Portions created by Fluendo, S.L. are Copyright (C) 2005
 * Fluendo, S.L. All Rights Reserved.
 *
 * Contributor(s): Wim Taymans <wim@fluendo.com>
 */

#ifndef __GST_MPEG_DEFS_H__
#define __GST_MPEG_DEFS_H__
#include <glib/gprintf.h>

#define SAFE_FOURCC_FORMAT "02x%02x%02x%02x (%c%c%c%c)"
#define SAFE_CHAR(a) (g_ascii_isalnum((gchar) (a)) ? ((gchar)(a)) : '.')
#define SAFE_FOURCC_ARGS(a)				\
  ((guint8) ((a)>>24)),					\
    ((guint8) ((a) >> 16 & 0xff)),			\
    ((guint8) a >> 8 & 0xff),				\
    ((guint8) a & 0xff),				\
    SAFE_CHAR((a)>>24),					\
    SAFE_CHAR((a) >> 16 & 0xff),			\
    SAFE_CHAR((a) >> 8 & 0xff),				\
    SAFE_CHAR(a & 0xff)

#define CLOCK_BASE 9LL
#define CLOCK_FREQ (CLOCK_BASE * 10000)

/* Numerical values for second/millisecond in PCR units */
#define PCR_SECOND 27000000
#define PCR_MSECOND 27000

/* PCR_TO_GST calculation requires at least 10 extra bits.
 * Since maximum PCR value is coded with 42 bits, we are
 * safe to use direct calculation (10+42 < 63)*/
#define PCRTIME_TO_GSTTIME(t) (((t) * (guint64)1000) / 27)

/* MPEG_TO_GST calculation requires at least 17 extra bits (100000)
 * Since maximum PTS/DTS value is coded with 33bits, we are
 * safe to use direct calculation (17+33 < 63) */
#define MPEGTIME_TO_GSTTIME(t) ((t) * (guint64)100000 / 9)

#define GSTTIME_TO_MPEGTIME(time) (gst_util_uint64_scale ((time), \
            CLOCK_BASE, GST_MSECOND/10))
#define GSTTIME_TO_PCRTIME(time) (gst_util_uint64_scale ((time), \
            300 * CLOCK_BASE, GST_MSECOND/10))

#define MPEG_MUX_RATE_MULT      50

/* sync:4 == 00xx ! pts:3 ! 1 ! pts:15 ! 1 | pts:15 ! 1 */
#define READ_TS(data, target, lost_sync_label)          \
    if ((*data & 0x01) != 0x01) goto lost_sync_label;   \
    target  = ((guint64) (*data++ & 0x0E)) << 29;       \
    target |= ((guint64) (*data++       )) << 22;       \
    if ((*data & 0x01) != 0x01) goto lost_sync_label;   \
    target |= ((guint64) (*data++ & 0xFE)) << 14;       \
    target |= ((guint64) (*data++       )) << 7;        \
    if ((*data & 0x01) != 0x01) goto lost_sync_label;   \
    target |= ((guint64) (*data++ & 0xFE)) >> 1;

#endif /* __GST_MPEG_DEFS_H__ */
