/*
 * gst-hdv-section.h -
 *
 * The Initial Developer of the Original Code is Fluendo, S.L.
 * Portions created by Fluendo, S.L. are Copyright (C) 2005
 * Fluendo, S.L. All Rights Reserved.
 *
 * Copyright (C) 2014 Florian Langlois
 *
 * Authors:
 *   Wim Taymans <wim@fluendo.com>
 *   Florian Langlois <chamois94@gmail.com>
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

#ifndef GST_HDV_SECTION_H
#define GST_HDV_SECTION_H

#include <gst/gst.h>

G_BEGIN_DECLS

/**
 * GstMpegtsHDVStreamType:
 *
 * Type of HDV (registration: 'TSMV' or 'TSHV') related mpeg-ts stream type.
 *
 * Consult ISO/IEC specifications : ISO/IEC 61834-11
 */
typedef enum {
  GST_MPEGTS_HDV_STREAM_TYPE_AUX_A = 0xa0,
  GST_MPEGTS_HDV_STREAM_TYPE_AUX_V = 0xa1
} GstMpegtsHDVStreamType;

G_END_DECLS

#endif  /* GST_HDV_SECTION_H */

