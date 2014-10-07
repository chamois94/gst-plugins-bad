/*
 * gstmpegtsdescriptor.h - 
 * Copyright (C) 2013 Edward Hervey
 * 
 * Authors:
 *   Edward Hervey <edward@collabora.com>
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
 * Some parts of this code come from the Fluendo MPEG Demuxer plugin.
 *
 * The Initial Developer of the Original Code is Fluendo, S.L.
 * Portions created by Fluendo, S.L. are Copyright (C) 2005
 * Fluendo, S.L. All Rights Reserved.
 *
 * Contributor(s): Wim Taymans <wim@fluendo.com>
 */

#ifndef GST_MPEGTS_DESCRIPTOR_H
#define GST_MPEGTS_DESCRIPTOR_H

#include <gst/gst.h>

G_BEGIN_DECLS

/*
 * descriptor_tag TS  PS                      Identification
 *        0       n/a n/a Reserved
 *        1       n/a n/a Reserved
 *        2        X   X  video_stream_descriptor
 *        3        X   X  audio_stream_descriptor
 *        4        X   X  hierarchy_descriptor
 *        5        X   X  registration_descriptor
 *        6        X   X  data_stream_alignment_descriptor
 *        7        X   X  target_background_grid_descriptor
 *        8        X   X  video_window_descriptor
 *        9        X   X  CA_descriptor
 *       10        X   X  ISO_639_language_descriptor
 *       11        X   X  system_clock_descriptor
 *       12        X   X  multiplex_buffer_utilization_descriptor
 *       13        X   X  copyright_descriptor
 *       14        X      maximum bitrate descriptor
 *       15        X   X  private data indicator descriptor
 *       16        X   X  smoothing buffer descriptor
 *       17        X      STD_descriptor
 *       18        X   X  IBP descriptor
 *      19-63     n/a n/a ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved
 *     64-255     n/a n/a User Private
 */

/**
 * GstMpegtsDescriptorType:
 *
 * The type of #GstMpegtsDescriptor
 *
 * These values correspond to the registered descriptor type from
 * the base MPEG-TS specifications (ITU H.222.0 | ISO/IEC 13818-1).
 *
 * Consult the relevant specifications for more details.
 */
typedef enum {
  /* 0-18 ISO/IEC 13818-1 (H222.0 06/2012) */
  GST_MTS_DESC_RESERVED_00                      = 0x00,
  GST_MTS_DESC_RESERVED_01                      = 0x01,
  GST_MTS_DESC_VIDEO_STREAM                     = 0x02,
  GST_MTS_DESC_AUDIO_STREAM                     = 0x03,
  GST_MTS_DESC_HIERARCHY                        = 0x04,
  GST_MTS_DESC_REGISTRATION                     = 0x05,
  GST_MTS_DESC_DATA_STREAM_ALIGNMENT            = 0x06,
  GST_MTS_DESC_TARGET_BACKGROUND_GRID           = 0x07,
  GST_MTS_DESC_VIDEO_WINDOW                     = 0x08,
  GST_MTS_DESC_CA                               = 0x09,
  GST_MTS_DESC_ISO_639_LANGUAGE                 = 0x0A,
  GST_MTS_DESC_SYSTEM_CLOCK                     = 0x0B,
  GST_MTS_DESC_MULTIPLEX_BUFFER_UTILISATION     = 0x0C,
  GST_MTS_DESC_COPYRIGHT                        = 0x0D,
  GST_MTS_DESC_MAXIMUM_BITRATE                  = 0x0E,
  GST_MTS_DESC_PRIVATE_DATA_INDICATOR           = 0x0F,
  GST_MTS_DESC_SMOOTHING_BUFFER                 = 0x10,
  GST_MTS_DESC_STD                              = 0x11,
  GST_MTS_DESC_IBP                              = 0x12,

  /* 19-26 Defined in ISO/IEC 13818-6 (Extensions for DSM-CC) */
  GST_MTS_DESC_DSMCC_CAROUSEL_IDENTIFIER        = 0x13,
  GST_MTS_DESC_DSMCC_ASSOCIATION_TAG            = 0x14,
  GST_MTS_DESC_DSMCC_DEFERRED_ASSOCIATION_TAG   = 0x15,
  /* 0x16 is reserved (so far) */
  GST_MTS_DESC_DSMCC_NPT_REFERENCE              = 0x17,
  GST_MTS_DESC_DSMCC_NPT_ENDPOINT               = 0x18,
  GST_MTS_DESC_DSMCC_STREAM_MODE                = 0x19,
  GST_MTS_DESC_DSMCC_STREAM_EVENT               = 0x1A,

  /* 27-54 Later additions to ISO/IEC 13818-1 (H222.0 06/2012) */
  GST_MTS_DESC_MPEG4_VIDEO                      = 0x1B,
  GST_MTS_DESC_MPEG4_AUDIO                      = 0x1C,
  GST_MTS_DESC_IOD                              = 0x1D,
  GST_MTS_DESC_SL                               = 0x1E,
  GST_MTS_DESC_FMC                              = 0x1F,
  GST_MTS_DESC_EXTERNAL_ES_ID                   = 0x20,
  GST_MTS_DESC_MUX_CODE                         = 0x21,
  GST_MTS_DESC_FMX_BUFFER_SIZE                  = 0x22,
  GST_MTS_DESC_MULTIPLEX_BUFFER                 = 0x23,
  GST_MTS_DESC_CONTENT_LABELING                 = 0x24,
  GST_MTS_DESC_METADATA_POINTER                 = 0x25,
  GST_MTS_DESC_METADATA                         = 0x26,
  GST_MTS_DESC_METADATA_STD                     = 0x27,
  GST_MTS_DESC_AVC_VIDEO                        = 0x28,
  /* defined in ISO/IEC 13818-11, MPEG-2 IPMP */
  GST_MTS_DESC_IPMP                             = 0x29,
  GST_MTS_DESC_AVC_TIMING_AND_HRD               = 0x2A,
  GST_MTS_DESC_MPEG2_AAC_AUDIO                  = 0x2B,
  GST_MTS_DESC_FLEX_MUX_TIMING                  = 0x2C,
  GST_MTS_DESC_MPEG4_TEXT                       = 0x2D,
  GST_MTS_DESC_MPEG4_AUDIO_EXTENSION            = 0x2E,
  GST_MTS_DESC_AUXILIARY_VIDEO_STREAM           = 0x2F,
  GST_MTS_DESC_SVC_EXTENSION                    = 0x30,
  GST_MTS_DESC_MVC_EXTENSION                    = 0x31,
  GST_MTS_DESC_J2K_VIDEO                        = 0x32,
  GST_MTS_DESC_MVC_OPERATION_POINT              = 0x33,
  GST_MTS_DESC_MPEG2_STEREOSCOPIC_VIDEO_FORMAT  = 0x34,
  GST_MTS_DESC_STEREOSCOPIC_PROGRAM_INFO        = 0x35,
  GST_MTS_DESC_STEREOSCOPIC_VIDEO_INFO          = 0x36,

  /* 55-63 ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved */
} GstMpegtsDescriptorType;

/**
 * GstMpegtsMiscDescriptorType:
 *
 * The type of #GstMpegtsDescriptor
 *
 * These values correspond to miscellaneous descriptor types that are
 * not yet identified from known specifications.
 */
typedef enum {
  /* 0x80 - 0xFE are user defined */
  GST_MTS_DESC_AC3_AUDIO_STREAM                 = 0x81,
  GST_MTS_DESC_DTG_LOGICAL_CHANNEL              = 0x83,    /* from DTG D-Book */
} GstMpegtsMiscDescriptorType;

typedef struct _GstMpegtsDescriptor GstMpegtsDescriptor;

#define GST_TYPE_MPEGTS_DESCRIPTOR (gst_mpegts_descriptor_get_type())
GType gst_mpegts_descriptor_get_type (void);

/**
 * GstMpegtsDescriptor:
 * @tag: the type of descriptor
 * @tag_extension: the extended type (if @descriptor_tag is 0x7f)
 * @length: the length of the descriptor content (excluding tag/length field)
 * @data: the full descriptor data (including tag, extension, length). The first
 * two bytes are the @tag and @tag_extension.
 *
 * Mpeg-TS descriptor (ISO/IEC 13818-1).
 */
struct _GstMpegtsDescriptor
{
  guint8 tag;
  guint8 tag_extension;
  guint8 length;
  guint8 *data;

  /*< private >*/
  /* Padding for future extension */
  gpointer _gst_reserved[GST_PADDING];
};

void       gst_mpegts_descriptor_free (GstMpegtsDescriptor *desc);

GPtrArray *gst_mpegts_parse_descriptors (guint8 * buffer, gsize buf_len);

const GstMpegtsDescriptor * gst_mpegts_find_descriptor (GPtrArray *descriptors,
							guint8 tag);

/* GST_MTS_DESC_VIDEO_STREAM (0x02) */
/**
 * GstMpegtsVideoStreamDescriptor:
 * @multiple_frame_rate : multiple frame rates present
 * @frame_rate_code : define frame rate
 * @mpeg1_only : only MPEG-1 only video data
 * @constrained_parameter : shall not contain unconstrained ISO/IEC 11172-2 video data
 * @still_picture : only still pictures
 * @profile_and_level_indication : higher profile and level in the stream
 * @chroma_format : chroma format
 * @frame_rate_extension : any frame rarte extension field in the stream
 *
 * Mpeg-TS video stream descriptor (ISO/IEC 13818-1).
 */
typedef struct _GstMpegtsVideoStreamDescriptor GstMpegtsVideoStreamDescriptor;
struct _GstMpegtsVideoStreamDescriptor
{
    gboolean    multiple_frame_rate;
    guint8      frame_rate_code;
    gboolean    mpeg1_only;
    gboolean    constrained_parameter;
    gboolean    still_picture;

    /* if (MPEG_1_only_flag == 1) */
    guint8      profile_and_level_indication;
    guint8      chroma_format;
    gboolean    frame_rate_extension;
};

gboolean  gst_mpegts_descriptor_parse_video_stream (const GstMpegtsDescriptor *descriptor,
        GstMpegtsVideoStreamDescriptor *res);

/* GST_MTS_DESC_AUDIO_STREAM (0x03) */
gboolean  gst_mpegts_descriptor_parse_audio_stream (const GstMpegtsDescriptor *descriptor,
                        gboolean    *free_format,
                        gboolean    *id,
                        guint8      *layer,
                        gboolean    *variable_rate_audio_indicator);

/* GST_MTS_DESC_HIERARCHY (0x04) */
gboolean  gst_mpegts_descriptor_parse_hierarchy (const GstMpegtsDescriptor *descriptor,
                        guint8  *hierarchy_type,
                        guint8  *hierarchy_layer_index,
                        guint8  *hierarchy_embedded_layer_index,
                        guint8  *hierarchy_channel);

/* GST_MTS_DESC_REGISTRATION (0x05) */

GstMpegtsDescriptor *gst_mpegts_descriptor_from_registration (
    const gchar *format_identifier,
    guint8 *additional_info, gsize additional_info_length);

/* GST_MTS_DESC_DATA_STREAM_ALIGNMENT (0x06) */
gboolean  gst_mpegts_descriptor_parse_data_stream_alignment (const GstMpegtsDescriptor *descriptor,
                      guint8 *alignment_type);

/* GST_MTS_DESC_TARGET_BACKGROUND_GRID (0x07) */
gboolean  gst_mpegts_descriptor_parse_background_grid (const GstMpegtsDescriptor *descriptor,
                      guint16 *horizontal_size,
                      guint16 *vertical_size,
                      guint8  *aspect_ratio_information);

/* GST_MTS_DESC_VIDEO_WINDOW (0x08) */
gboolean  gst_mpegts_descriptor_parse_video_window (const GstMpegtsDescriptor *descriptor,
                      guint16 *horizontal_offset,
                      guint16 *vertical_offset,
                      guint8  *window_priority);

/* GST_MTS_DESC_CA (0x09) */
gboolean  gst_mpegts_descriptor_parse_ca (GstMpegtsDescriptor *descriptor,
					  guint16 *ca_system_id,
					  guint16 *ca_pid,
					  const guint8 **private_data,
					  gsize *private_data_size);

/* GST_MTS_DESC_ISO_639_LANGUAGE (0x0A) */
/**
 * GstMpegtsISO639AudioType:
 *
 * Type of audio streams
 *
 * Defined in ITU H.222.0 Table 2-60
 */
typedef enum {
  GST_MPEGTS_AUDIO_TYPE_UNDEFINED = 0,
  GST_MPEGTS_AUDIO_TYPE_CLEAN_EFFECTS,
  GST_MPEGTS_AUDIO_TYPE_HEARING_IMPAIRED,
  GST_MPEGTS_AUDIO_TYPE_VISUAL_IMPAIRED_COMMENTARY
} GstMpegtsIso639AudioType;

typedef struct _GstMpegtsISO639LanguageDescriptor GstMpegtsISO639LanguageDescriptor;
struct _GstMpegtsISO639LanguageDescriptor
{
  guint                    nb_language;
  gchar                    *language[64];
  GstMpegtsIso639AudioType audio_type[64];
};

#define GST_TYPE_MPEGTS_ISO_639_LANGUAGE (gst_mpegts_iso_639_language_get_type ())
GType gst_mpegts_iso_639_language_get_type (void);
void gst_mpegts_iso_639_language_descriptor_free (GstMpegtsISO639LanguageDescriptor * desc);
gboolean gst_mpegts_descriptor_parse_iso_639_language (const GstMpegtsDescriptor *descriptor,
						       GstMpegtsISO639LanguageDescriptor **res);
gboolean gst_mpegts_descriptor_parse_iso_639_language_idx (const GstMpegtsDescriptor *descriptor,
                                                           guint idx, gchar **lang,
                                                           GstMpegtsIso639AudioType *audio_type);
guint gst_mpegts_descriptor_parse_iso_639_language_nb (const GstMpegtsDescriptor *descriptor);

/* GST_MTS_DESC_SYSTEM_CLOCK (0x0B) */
gboolean  gst_mpegts_descriptor_parse_system_clock (const GstMpegtsDescriptor *descriptor,
                      gboolean *external_clock_reference_indicator,
                      guint8   *clock_accuracy_integer,
                      guint8   *clock_accuracy_exponent);

/* GST_MTS_DESC_MULTIPLEX_BUFFER_UTILIZATION (0x0C) */
gboolean  gst_mpegts_descriptor_parse_multiplex_buffer_utilization (const GstMpegtsDescriptor *descriptor,
                      gboolean *bound_valid,
                      guint16   *LTW_offset_lower_bound,
                      guint16   *LTW_offset_upper_bound);

/* GST_MTS_DESC_COPYRIGHT (0x0D) */
gboolean  gst_mpegts_descriptor_parse_copyright (const GstMpegtsDescriptor *descriptor,
                      guint32  *copyright_identifier,
                      const guint8 **additional_copyright_info,
                      gsize *additional_copyright_info_size);

/* GST_MTS_DESC_MAXIMUM_BITRATE (0x0E) */
gboolean  gst_mpegts_descriptor_parse_maximum_bitrate (const GstMpegtsDescriptor *descriptor,
                      guint32  *maximum_bitrate);

/* GST_MTS_DESC_PRIVATE_DATA_INDICATOR (0x0F) */
gboolean  gst_mpegts_descriptor_parse_private_data_indicator (const GstMpegtsDescriptor *descriptor,
                      guint32  *indicator);

/* GST_MTS_DESC_SMOOTHING_BUFFER (0x10) */
gboolean  gst_mpegts_descriptor_parse_smoothing_buffer (const GstMpegtsDescriptor *descriptor,
                      guint32  *sb_leak_rate,
                      guint32  *sb_size);

/* GST_MTS_DESC_STD (0x11) */
gboolean  gst_mpegts_descriptor_parse_std (const GstMpegtsDescriptor *descriptor,
                      gboolean  *leak_valid);

/* GST_MTS_DESC_IBP (0x12) */
gboolean  gst_mpegts_descriptor_parse_ibp (const GstMpegtsDescriptor *descriptor,
                      gboolean  *closed_gop,
                      gboolean  *identical_gop,
                      guint16   *max_gop_length);


GstMpegtsDescriptor *
gst_mpegts_descriptor_from_custom (guint8 tag, const guint8 *data, gsize length);

G_END_DECLS

#endif				/* GST_MPEGTS_DESCRIPTOR_H */
