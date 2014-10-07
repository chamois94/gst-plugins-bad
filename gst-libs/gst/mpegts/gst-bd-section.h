/*
 * gst-bd-section.h -
 * Copyright (C) 2014, LANGLOIS Florian
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
 */

#ifndef GST_BD_SECTION_H
#define GST_BD_SECTION_H

#include <gst/gst.h>

G_BEGIN_DECLS

/**
 * GstMpegtsBDStreamType:
 *
 * Type of Bluray related (registration: 'HDMV') mpeg-ts stream type.
 *
 * Consult "System Description Blu-ray Disc Read-Only Format part 3 Audio Visual Basic Specifications".
 */
typedef enum {
  GST_MPEGTS_BD_STREAM_TYPE_AUDIO_LPCM                = 0x80,
  GST_MPEGTS_BD_STREAM_TYPE_AUDIO_AC3                 = 0x81,
  GST_MPEGTS_BD_STREAM_TYPE_AUDIO_DTS                 = 0x82,
  GST_MPEGTS_BD_STREAM_TYPE_AUDIO_AC3_TRUE_HD         = 0x83,
  GST_MPEGTS_BD_STREAM_TYPE_AUDIO_AC3_PLUS            = 0x84,
  GST_MPEGTS_BD_STREAM_TYPE_AUDIO_DTS_HD              = 0x85,
  GST_MPEGTS_BD_STREAM_TYPE_AUDIO_DTS_HD_MASTER_AUDIO = 0x86,
  GST_MPEGTS_BD_STREAM_TYPE_AUDIO_EAC3                = 0x87,
  GST_MPEGTS_BD_STREAM_TYPE_PGS_SUBPICTURE            = 0x90,
  GST_MPEGTS_BD_STREAM_TYPE_IGS                       = 0x91,
  GST_MPEGTS_BD_STREAM_TYPE_SUBTITLE                  = 0x92,
  GST_MPEGTS_BD_STREAM_TYPE_SECONDARY_AC3_PLUS        = 0xa1,
  GST_MPEGTS_BD_STREAM_TYPE_SECONDARY_DTS_HD          = 0xa2
} GstMpegtsBDStreamType;

G_END_DECLS

#endif  /* GST_BD_SECTION_H */

