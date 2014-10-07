/*
 * gstmpegtsdescriptor.c - 
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
 */
#include <stdlib.h>
#include <string.h>

#include "mpegts.h"
#include "gstmpegts-private.h"

/**
 * SECTION:gstmpegtsdescriptor
 * @title: Base MPEG-TS descriptors
 * @short_description: Descriptors for ITU H.222.0 | ISO/IEC 13818-1 
 * @include: gst/mpegts/mpegts.h
 *
 * These are the base descriptor types and methods.
 *
 * For more details, refer to the ITU H.222.0 or ISO/IEC 13818-1 specifications
 * and other specifications mentionned in the documentation.
 */

/* FIXME : Move this to proper file once we have a C file for ATSC/ISDB descriptors */
/**
 * SECTION:gst-atsc-descriptor
 * @title: ATSC variants of MPEG-TS descriptors
 * @short_description: Descriptors for the various ATSC specifications
 * @include: gst/mpegts/mpegts.h
 *
 */

/**
 * SECTION:gst-isdb-descriptor
 * @title: ISDB variants of MPEG-TS descriptors
 * @short_description: Descriptors for the various ISDB specifications
 * @include: gst/mpegts/mpegts.h
 *
 */


/*
 * TODO
 *
 * * Add common validation code for data presence and minimum/maximum expected
 *   size.
 */

#define MAX_KNOWN_ICONV 25

/* First column is the original encoding,
 * second column is the target encoding */

static GIConv __iconvs[MAX_KNOWN_ICONV][MAX_KNOWN_ICONV];

/* All these conversions will be to UTF8 */
typedef enum
{
  _ICONV_UNKNOWN = -1,
  _ICONV_ISO8859_1,
  _ICONV_ISO8859_2,
  _ICONV_ISO8859_3,
  _ICONV_ISO8859_4,
  _ICONV_ISO8859_5,
  _ICONV_ISO8859_6,
  _ICONV_ISO8859_7,
  _ICONV_ISO8859_8,
  _ICONV_ISO8859_9,
  _ICONV_ISO8859_10,
  _ICONV_ISO8859_11,
  _ICONV_ISO8859_12,
  _ICONV_ISO8859_13,
  _ICONV_ISO8859_14,
  _ICONV_ISO8859_15,
  _ICONV_UCS_2BE,
  _ICONV_EUC_KR,
  _ICONV_GB2312,
  _ICONV_UTF_16BE,
  _ICONV_ISO10646_UTF8,
  _ICONV_ISO6937,
  _ICONV_UTF8,
  /* Insert more here if needed */
  _ICONV_MAX
} LocalIconvCode;

static const gchar *iconvtablename[] = {
  "iso-8859-1",
  "iso-8859-2",
  "iso-8859-3",
  "iso-8859-4",
  "iso-8859-5",
  "iso-8859-6",
  "iso-8859-7",
  "iso-8859-8",
  "iso-8859-9",
  "iso-8859-10",
  "iso-8859-11",
  "iso-8859-12",
  "iso-8859-13",
  "iso-8859-14",
  "iso-8859-15",
  "UCS-2BE",
  "EUC-KR",
  "GB2312",
  "UTF-16BE",
  "ISO-10646/UTF8",
  "iso6937",
  "utf-8"
      /* Insert more here if needed */
};

void
__initialize_descriptors (void)
{
  guint i, j;

  /* Initialize converters */
  /* FIXME : How/when should we close them ??? */
  for (i = 0; i < MAX_KNOWN_ICONV; i++) {
    for (j = 0; j < MAX_KNOWN_ICONV; j++)
      __iconvs[i][j] = ((GIConv) - 1);
  }
}

/*
 * @text: The text you want to get the encoding from
 * @start_text: Location where the beginning of the actual text is stored
 * @is_multibyte: Location where information whether it's a multibyte encoding
 * or not is stored
 * @returns: GIconv for conversion or NULL
 */
static LocalIconvCode
get_encoding (const gchar * text, guint * start_text, gboolean * is_multibyte)
{
  LocalIconvCode encoding;
  guint8 firstbyte;

  *is_multibyte = FALSE;
  *start_text = 0;

  firstbyte = (guint8) text[0];

  /* A wrong value */
  g_return_val_if_fail (firstbyte != 0x00, _ICONV_UNKNOWN);

  if (firstbyte <= 0x0B) {
    /* 0x01 => iso 8859-5 */
    encoding = firstbyte + _ICONV_ISO8859_4;
    *start_text = 1;
    goto beach;
  }

  /* ETSI EN 300 468, "Selection of character table" */
  switch (firstbyte) {
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
      /* RESERVED */
      encoding = _ICONV_UNKNOWN;
      break;
    case 0x10:
    {
      guint16 table;

      table = GST_READ_UINT16_BE (text + 1);

      if (table < 17)
        encoding = _ICONV_UNKNOWN + table;
      else
        encoding = _ICONV_UNKNOWN;
      *start_text = 3;
      break;
    }
    case 0x11:
      encoding = _ICONV_UCS_2BE;
      *start_text = 1;
      *is_multibyte = TRUE;
      break;
    case 0x12:
      /*  EUC-KR implements KSX1001 */
      encoding = _ICONV_EUC_KR;
      *start_text = 1;
      *is_multibyte = TRUE;
      break;
    case 0x13:
      encoding = _ICONV_GB2312;
      *start_text = 1;
      break;
    case 0x14:
      encoding = _ICONV_UTF_16BE;
      *start_text = 1;
      *is_multibyte = TRUE;
      break;
    case 0x15:
      /* TODO : Where does this come from ?? */
      encoding = _ICONV_ISO10646_UTF8;
      *start_text = 1;
      break;
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
      /* RESERVED */
      encoding = _ICONV_UNKNOWN;
      break;
    default:
      encoding = _ICONV_ISO6937;
      break;
  }

beach:
  GST_DEBUG
      ("Found encoding %d, first byte is 0x%02x, start_text: %u, is_multibyte: %d",
      encoding, firstbyte, *start_text, *is_multibyte);

  return encoding;
}

static GIConv
_get_iconv (LocalIconvCode from, LocalIconvCode to)
{
  if (__iconvs[from][to] == (GIConv) - 1)
    __iconvs[from][to] = g_iconv_open (iconvtablename[to],
        iconvtablename[from]);
  return __iconvs[from][to];
}

static void
_encode_control_codes (gchar * text, gsize length, gboolean is_multibyte)
{
  gsize pos = 0;

  while (pos < length) {
    if (is_multibyte) {
      guint16 code = GST_READ_UINT16_BE (text + pos);
      if (code == 0x000A) {
        text[pos] = 0xE0;
        text[pos + 1] = 0x8A;
      }
      pos += 2;
    } else {
      guint8 code = text[pos];
      if (code == 0x0A)
        text[pos] = 0x8A;
      pos++;
    }
  }
}

/**
 * dvb_text_from_utf8:
 * @text: The text to convert. This should be in UTF-8 format
 * @out_size: (out): the byte length of the new text
 *
 * Converts UTF-8 strings to text characters compliant with EN 300 468.
 * The converted text can be used directly in DVB #GstMpegtsDescriptor
 *
 * The function will try different character maps until the string is
 * completely converted.
 *
 * The function tries the default ISO 6937 character map first.
 *
 * If no character map that contains all characters could be found, the
 * string is converted to ISO 6937 with unknown characters set to `?`.
 *
 * Returns: (transfer full): byte array of size @out_size
 */
guint8 *
dvb_text_from_utf8 (const gchar * text, gsize * out_size)
{
  GError *error = NULL;
  gchar *out_text;
  guint8 *out_buffer;
  guint encoding;
  GIConv giconv = (GIConv) - 1;

  /* We test character maps one-by-one. Start with the default */
  encoding = _ICONV_ISO6937;
  giconv = _get_iconv (_ICONV_UTF8, encoding);
  out_text = g_convert_with_iconv (text, -1, giconv, NULL, out_size, &error);

  if (out_text) {
    GST_DEBUG ("Using default ISO6937 encoding");
    goto out;
  }

  g_clear_error (&error);

  for (encoding = _ICONV_ISO8859_1; encoding <= _ICONV_ISO10646_UTF8;
      encoding++) {
    giconv = _get_iconv (_ICONV_UTF8, encoding);
    if (giconv == (GIConv) - 1)
      continue;
    out_text = g_convert_with_iconv (text, -1, giconv, NULL, out_size, &error);

    if (out_text) {
      GST_DEBUG ("Found suitable character map - %s", iconvtablename[encoding]);
      goto out;
    }

    g_clear_error (&error);
  }

  out_text = g_convert_with_fallback (text, -1, iconvtablename[_ICONV_ISO6937],
      iconvtablename[_ICONV_UTF8], "?", NULL, out_size, &error);

out:

  if (error) {
    GST_WARNING ("Could not convert from utf-8: %s", error->message);
    g_error_free (error);
    g_free (out_text);
    return NULL;
  }

  switch (encoding) {
    case _ICONV_ISO6937:
      /* Default encoding contains no selection bytes. */
      _encode_control_codes (out_text, *out_size, FALSE);
      return (guint8 *) out_text;
    case _ICONV_ISO8859_1:
    case _ICONV_ISO8859_2:
    case _ICONV_ISO8859_3:
    case _ICONV_ISO8859_4:
      /* These character sets requires 3 selection bytes */
      _encode_control_codes (out_text, *out_size, FALSE);
      out_buffer = g_malloc (*out_size + 3);
      out_buffer[0] = 0x10;
      out_buffer[1] = 0x00;
      out_buffer[2] = encoding - _ICONV_ISO8859_1 + 1;
      memcpy (out_buffer + 3, out_text, *out_size);
      *out_size += 3;
      g_free (out_text);
      return out_buffer;
    case _ICONV_ISO8859_5:
    case _ICONV_ISO8859_6:
    case _ICONV_ISO8859_7:
    case _ICONV_ISO8859_8:
    case _ICONV_ISO8859_9:
    case _ICONV_ISO8859_10:
    case _ICONV_ISO8859_11:
    case _ICONV_ISO8859_12:
    case _ICONV_ISO8859_13:
    case _ICONV_ISO8859_14:
    case _ICONV_ISO8859_15:
      /* These character sets requires 1 selection byte */
      _encode_control_codes (out_text, *out_size, FALSE);
      out_buffer = g_malloc (*out_size + 1);
      out_buffer[0] = encoding - _ICONV_ISO8859_5 + 1;
      memcpy (out_buffer + 1, out_text, *out_size);
      *out_size += 1;
      g_free (out_text);
      return out_buffer;
    case _ICONV_UCS_2BE:
    case _ICONV_EUC_KR:
    case _ICONV_UTF_16BE:
      /* These character sets requires 1 selection byte */
      _encode_control_codes (out_text, *out_size, TRUE);
      out_buffer = g_malloc (*out_size + 1);
      out_buffer[0] = encoding - _ICONV_UCS_2BE + 0x11;
      memcpy (out_buffer + 1, out_text, *out_size);
      *out_size += 1;
      g_free (out_text);
      return out_buffer;
    case _ICONV_GB2312:
    case _ICONV_ISO10646_UTF8:
      /* These character sets requires 1 selection byte */
      _encode_control_codes (out_text, *out_size, FALSE);
      out_buffer = g_malloc (*out_size + 1);
      out_buffer[0] = encoding - _ICONV_UCS_2BE + 0x11;
      memcpy (out_buffer + 1, out_text, *out_size);
      *out_size += 1;
      g_free (out_text);
      return out_buffer;
    default:
      g_free (out_text);
      return NULL;
  }
}

/*
 * @text: The text to convert. It may include pango markup (<b> and </b>)
 * @length: The length of the string -1 if it's nul-terminated
 * @start: Where to start converting in the text
 * @encoding: The encoding of text
 * @is_multibyte: Whether the encoding is a multibyte encoding
 * @error: The location to store the error, or NULL to ignore errors
 * @returns: UTF-8 encoded string
 *
 * Convert text to UTF-8.
 */
static gchar *
convert_to_utf8 (const gchar * text, gint length, guint start,
    GIConv giconv, gboolean is_multibyte, GError ** error)
{
  gchar *new_text;
  gchar *tmp, *pos;
  gint i;

  text += start;

  pos = tmp = g_malloc (length * 2);

  if (is_multibyte) {
    if (length == -1) {
      while (*text != '\0') {
        guint16 code = GST_READ_UINT16_BE (text);

        switch (code) {
          case 0xE086:         /* emphasis on */
          case 0xE087:         /* emphasis off */
            /* skip it */
            break;
          case 0xE08A:{
            pos[0] = 0x00;      /* 0x00 0x0A is a new line */
            pos[1] = 0x0A;
            pos += 2;
            break;
          }
          default:
            pos[0] = text[0];
            pos[1] = text[1];
            pos += 2;
            break;
        }

        text += 2;
      }
    } else {
      for (i = 0; i < length; i += 2) {
        guint16 code = GST_READ_UINT16_BE (text);

        switch (code) {
          case 0xE086:         /* emphasis on */
          case 0xE087:         /* emphasis off */
            /* skip it */
            break;
          case 0xE08A:{
            pos[0] = 0x00;      /* 0x00 0x0A is a new line */
            pos[1] = 0x0A;
            pos += 2;
            break;
          }
          default:
            pos[0] = text[0];
            pos[1] = text[1];
            pos += 2;
            break;
        }

        text += 2;
      }
    }
  } else {
    if (length == -1) {
      while (*text != '\0') {
        guint8 code = (guint8) (*text);

        switch (code) {
          case 0x86:           /* emphasis on */
          case 0x87:           /* emphasis off */
            /* skip it */
            break;
          case 0x8A:
            *pos = '\n';
            pos += 1;
            break;
          default:
            *pos = *text;
            pos += 1;
            break;
        }

        text++;
      }
    } else {
      for (i = 0; i < length; i++) {
        guint8 code = (guint8) (*text);

        switch (code) {
          case 0x86:           /* emphasis on */
          case 0x87:           /* emphasis off */
            /* skip it */
            break;
          case 0x8A:
            *pos = '\n';
            pos += 1;
            break;
          default:
            *pos = *text;
            pos += 1;
            break;
        }

        text++;
      }
    }
  }

  if (pos > tmp) {
    gsize bread = 0;
    new_text =
        g_convert_with_iconv (tmp, pos - tmp, giconv, &bread, NULL, error);
    GST_DEBUG ("Converted to : %s", new_text);
  } else {
    new_text = g_strdup ("");
  }

  g_free (tmp);

  return new_text;
}

gchar *
get_encoding_and_convert (const gchar * text, guint length)
{
  GError *error = NULL;
  gchar *converted_str;
  guint start_text = 0;
  gboolean is_multibyte;
  LocalIconvCode encoding;
  GIConv giconv = (GIConv) - 1;

  g_return_val_if_fail (text != NULL, NULL);

  if (text == NULL || length == 0)
    return g_strdup ("");

  encoding = get_encoding (text, &start_text, &is_multibyte);

  if (encoding > _ICONV_UNKNOWN && encoding < _ICONV_MAX) {
    GST_DEBUG ("Encoding %s", iconvtablename[encoding]);
    giconv = _get_iconv (encoding, _ICONV_UTF8);
  } else {
    GST_FIXME ("Could not detect encoding. Returning NULL string");
    converted_str = NULL;
    goto beach;
  }

  converted_str = convert_to_utf8 (text, length - start_text, start_text,
      giconv, is_multibyte, &error);
  if (error != NULL) {
    GST_WARNING ("Could not convert string: %s", error->message);
    g_free (converted_str);
    g_error_free (error);
    error = NULL;

    if (encoding >= _ICONV_ISO8859_2 && encoding <= _ICONV_ISO8859_15) {
      /* Sometimes using the standard 8859-1 set fixes issues */
      GST_DEBUG ("Encoding %s", iconvtablename[_ICONV_ISO8859_1]);
      giconv = _get_iconv (_ICONV_ISO8859_1, _ICONV_UTF8);

      GST_INFO ("Trying encoding ISO 8859-1");
      converted_str = convert_to_utf8 (text, length, 1, giconv, FALSE, &error);
      if (error != NULL) {
        GST_WARNING
            ("Could not convert string while assuming encoding ISO 8859-1: %s",
            error->message);
        g_error_free (error);
        goto failed;
      }
    } else if (encoding == _ICONV_ISO6937) {

      /* The first part of ISO 6937 is identical to ISO 8859-9, but
       * they differ in the second part. Some channels don't
       * provide the first byte that indicates ISO 8859-9 encoding.
       * If decoding from ISO 6937 failed, we try ISO 8859-9 here.
       */
      giconv = _get_iconv (_ICONV_ISO8859_9, _ICONV_UTF8);

      GST_INFO ("Trying encoding ISO 8859-9");
      converted_str = convert_to_utf8 (text, length, 0, giconv, FALSE, &error);
      if (error != NULL) {
        GST_WARNING
            ("Could not convert string while assuming encoding ISO 8859-9: %s",
            error->message);
        g_error_free (error);
        goto failed;
      }
    } else
      goto failed;
  }

beach:
  return converted_str;

failed:
  {
    text += start_text;
    return g_strndup (text, length - start_text);
  }
}

gchar *
convert_lang_code (guint8 * data)
{
  gchar *code;
  /* the iso language code and country code is always 3 byte long */
  code = g_malloc0 (4);
  memcpy (code, data, 3);

  return code;
}

void
_packetize_descriptor_array (GPtrArray * array, guint8 ** out_data)
{
  guint i;
  guint8 header_size;
  GstMpegtsDescriptor *descriptor;

  g_return_if_fail (out_data != NULL);
  g_return_if_fail (*out_data != NULL);

  if (array == NULL)
    return;

  for (i = 0; i < array->len; i++) {
    descriptor = g_ptr_array_index (array, i);

    if (descriptor->tag == GST_MTS_DESC_DVB_EXTENSION)
      header_size = 3;
    else
      header_size = 2;

    memcpy (*out_data, descriptor->data, descriptor->length + header_size);
    *out_data += descriptor->length + header_size;
  }
}

GstMpegtsDescriptor *
_new_descriptor (guint8 tag, guint8 length)
{
  GstMpegtsDescriptor *descriptor;
  guint8 *data;

  descriptor = g_slice_new (GstMpegtsDescriptor);

  descriptor->tag = tag;
  descriptor->tag_extension = 0;
  descriptor->length = length;

  descriptor->data = g_malloc (length + 2);

  data = descriptor->data;

  *data++ = descriptor->tag;
  *data = descriptor->length;

  return descriptor;
}

GstMpegtsDescriptor *
_new_descriptor_with_extension (guint8 tag, guint8 tag_extension, guint8 length)
{
  GstMpegtsDescriptor *descriptor;
  guint8 *data;

  descriptor = g_slice_new (GstMpegtsDescriptor);

  descriptor->tag = tag;
  descriptor->tag_extension = tag_extension;
  descriptor->length = length;

  descriptor->data = g_malloc (length + 3);

  data = descriptor->data;

  *data++ = descriptor->tag;
  *data++ = descriptor->tag_extension;
  *data = descriptor->length;

  return descriptor;
}

static GstMpegtsDescriptor *
_copy_descriptor (GstMpegtsDescriptor * desc)
{
  GstMpegtsDescriptor *copy;

  copy = g_slice_dup (GstMpegtsDescriptor, desc);
  copy->data = g_memdup (desc->data, desc->length + 2);

  return copy;
}

/**
 * gst_mpegts_descriptor_free:
 * @desc: The descriptor to free
 *
 * Frees @desc
 */
void
gst_mpegts_descriptor_free (GstMpegtsDescriptor * desc)
{
  g_free ((gpointer) desc->data);
  g_slice_free (GstMpegtsDescriptor, desc);
}

G_DEFINE_BOXED_TYPE (GstMpegtsDescriptor, gst_mpegts_descriptor,
    (GBoxedCopyFunc) _copy_descriptor,
    (GBoxedFreeFunc) gst_mpegts_descriptor_free);

/**
 * gst_mpegts_parse_descriptors:
 * @buffer: (transfer none): descriptors to parse
 * @buf_len: Size of @buffer
 *
 * Parses the descriptors present in @buffer and returns them as an
 * array.
 *
 * Note: The data provided in @buffer will not be copied.
 *
 * Returns: (transfer full) (element-type GstMpegtsDescriptor): an
 * array of the parsed descriptors or %NULL if there was an error.
 * Release with #g_array_unref when done with it.
 */
GPtrArray *
gst_mpegts_parse_descriptors (guint8 * buffer, gsize buf_len)
{
  GPtrArray *res;
  guint8 length;
  guint8 *data;
  guint i, nb_desc = 0;

  /* fast-path */
  if (buf_len == 0)
    return g_ptr_array_new ();

  data = buffer;

  GST_MEMDUMP ("Full descriptor array", buffer, buf_len);

  while (data - buffer < buf_len) {
    data++;                     /* skip tag */
    length = *data++;

    if (data - buffer > buf_len) {
      GST_WARNING ("invalid descriptor length %d now at %d max %"
          G_GSIZE_FORMAT, length, (gint) (data - buffer), buf_len);
      return NULL;
    }

    data += length;
    nb_desc++;
  }

  GST_DEBUG ("Saw %d descriptors, read %" G_GSIZE_FORMAT " bytes",
      nb_desc, (gsize) (data - buffer));

  if (data - buffer != buf_len) {
    GST_WARNING ("descriptors size %d expected %" G_GSIZE_FORMAT,
        (gint) (data - buffer), buf_len);
    return NULL;
  }

  res =
      g_ptr_array_new_full (nb_desc + 1,
      (GDestroyNotify) gst_mpegts_descriptor_free);

  data = buffer;

  for (i = 0; i < nb_desc; i++) {
    GstMpegtsDescriptor *desc = g_slice_new0 (GstMpegtsDescriptor);

    desc->data = data;
    desc->tag = *data++;
    desc->length = *data++;
    /* Copy the data now that we known the size */
    desc->data = g_memdup (desc->data, desc->length + 2);
    GST_LOG ("descriptor 0x%02x length:%d", desc->tag, desc->length);
    GST_MEMDUMP ("descriptor", desc->data + 2, desc->length);
    /* extended descriptors */
    if (G_UNLIKELY (desc->tag == 0x7f))
      desc->tag_extension = *data;

    data += desc->length;

    /* Set the descriptor in the array */
    g_ptr_array_index (res, i) = desc;
  }

  res->len = nb_desc;

  return res;
}

/**
 * gst_mpegts_find_descriptor:
 * @descriptors: (element-type GstMpegtsDescriptor) (transfer none): an array
 * of #GstMpegtsDescriptor
 * @tag: the tag to look for
 *
 * Finds the first descriptor of type @tag in the array.
 *
 * Note: To look for descriptors that can be present more than once in an
 * array of descriptors, iterate the #GArray manually.
 *
 * Returns: (transfer none): the first descriptor matchin @tag, else %NULL.
 */
const GstMpegtsDescriptor *
gst_mpegts_find_descriptor (GPtrArray * descriptors, guint8 tag)
{
  guint i, nb_desc;

  g_return_val_if_fail (descriptors != NULL, NULL);

  nb_desc = descriptors->len;
  for (i = 0; i < nb_desc; i++) {
    GstMpegtsDescriptor *desc = g_ptr_array_index (descriptors, i);
    if (desc->tag == tag)
      return (const GstMpegtsDescriptor *) desc;
  }
  return NULL;
}

/* GST_MTS_DESC_VIDEO_STREAM (0x02) */

/**
 * gst_mpegts_descriptor_parse_video_stream:
 * @descriptor: a %GST_MTS_DESC_VIDEO_STREAM #GstMpegtsDescriptor
 * @res: (out) (transfer none): the #GstMpegtsVideoStreamDescriptor to fill
 *
 * Extracts the video stream information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean
gst_mpegts_descriptor_parse_video_stream (const GstMpegtsDescriptor *descriptor,
        GstMpegtsVideoStreamDescriptor *res)
{
    guint8 *data;

    /* Size should be at least 1 byte */
    __common_desc_checks (descriptor, GST_MTS_DESC_VIDEO_STREAM, 1, FALSE);

    data = (guint8 *) descriptor->data + 2;

    res->multiple_frame_rate = ((*data & 0x80) == 0x80);
    res->frame_rate_code = ((*data & 0x38) >> 3);
    res->mpeg1_only = ((*data & 0x04) == 0x04);
    res->constrained_parameter = ((*data & 0x02) == 0x02);
    res->still_picture = ((*data & 0x01) == 0x01);

    if ( res->mpeg1_only && descriptor->length == 3) {
        data++;
        res->profile_and_level_indication = *data;
        data++;
        res->chroma_format = ((*data & 0xc0) >> 6);
        res->frame_rate_extension = ((*data & 0x20) == 0x20);
    }

    return TRUE;
}

/* GST_MTS_DESC_AUDIO_STREAM (0x03) */

/**
 * gst_mpegts_descriptor_parse_audio_stream:
 * @descriptor: a %GST_MTS_DESC_AUDIO_STREAM #GstMpegtsDescriptor
 * @free_format : (out) (transfer none): audio stream may contain one or more audio frame with bitrate_index to '0000'
 * @id : (out) (transfer none): ID field set to '1' in each frame
 * @layer : (out) (transfer none): layer field
 * @variable_rate_audio_indicator : (out) (transfer none): encoded value of the bit rate shall not change
 *
 * Extracts the audio stream information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean
gst_mpegts_descriptor_parse_audio_stream (const GstMpegtsDescriptor *descriptor,
        gboolean    *free_format,
        gboolean    *id,
        guint8      *layer,
        gboolean    *variable_rate_audio_indicator)
{
    guint8 *data;

    g_return_val_if_fail (free_format != NULL && id != NULL &&
            layer != NULL && variable_rate_audio_indicator != NULL, FALSE);
    /* Size should be 1 byte */
    __common_desc_checks (descriptor, GST_MTS_DESC_AUDIO_STREAM, 1, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *free_format = ((*data & 0x80) == 0x80);
    *id = ((*data & 0x40) == 0x40);
    *layer = ((*data & 0x30) >> 4);
    *variable_rate_audio_indicator = ((*data & 0x08) == 0x08);

    return TRUE;
}

/* GST_MTS_DESC_HIERARCHY (0x04) */

/**
 * gst_mpegts_descriptor_parse_hierarchy:
 * @descriptor: a %GST_MTS_DESC_HIERARCHY #GstMpegtsDescriptor
 * @hierarchy_type: (out) (transfer none): the hierarchical relation layer and embedded layer
 * @hierarchy_layer_index:  (out) (transfer none): 6-bit index of the associated program element in
 * a table of coding layer hierarchies
 * @hierarchy_embedded_layer_index:  (out) (transfer none): 6-bit hierarchy table index of the PE
 * that needs to be accessed before decoding the ES associated with this descriptor
 * @hierarchy_channel:  (out) (transfer none): 6-bit channel number for the associated program element
 *
 * Extracts the hierarchy information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean
gst_mpegts_descriptor_parse_hierarchy (const GstMpegtsDescriptor *descriptor,
        guint8  *hierarchy_type,
        guint8  *hierarchy_layer_index,
        guint8  *hierarchy_embedded_layer_index,
        guint8  *hierarchy_channel)
{
    guint8 *data;

    g_return_val_if_fail (hierarchy_type != NULL && hierarchy_layer_index != NULL &&
            hierarchy_embedded_layer_index != NULL && hierarchy_channel != NULL, FALSE);
    /* Size should be 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_HIERARCHY, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *hierarchy_type = ((*data & 0x0f));
    data++;
    *hierarchy_layer_index = ((*data & 0x3f));
    data++;
    *hierarchy_embedded_layer_index = ((*data & 0x3f));
    data++;
    *hierarchy_channel = ((*data & 0x3f));

    return TRUE;
}

/* GST_MTS_DESC_REGISTRATION (0x05) */

/**
 * gst_mpegts_descriptor_parse_registration:
 * @descriptor: a %GST_MTS_DESC_CA #GstMpegtsDescriptor
 * @format_identifier: (out) : the 32-bit value obtained from a Registration Authority (SMPTE)
 * @additional_identification_info: (out) (allow-none): defined by the assignee of the format_identifier
 * @additional_identification_info_size: (out) (allow-none): The size of @additional_identification_info in bytes
 *
 * Extracts the registration information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean
gst_mpegts_descriptor_parse_registration (const GstMpegtsDescriptor *descriptor,
                      guint32 *format_identifier,
                      guint8 **additional_identification_info,
                      gsize *additional_identification_info_size)
{
    guint8 *data;

    g_return_val_if_fail (descriptor != NULL && format_identifier != NULL, FALSE);
    /* Size should be at least 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_REGISTRATION, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *format_identifier = GST_READ_UINT32_BE(data);

    data += 4;
    if (additional_identification_info && additional_identification_info_size) {
      *additional_identification_info = data;
      *additional_identification_info_size = descriptor->length - 4;
    }

    return TRUE;
}

/**
 * gst_mpegts_descriptor_from_registration:
 * @format_identifier: (transfer none): a 4 character format identifier string
 * @additional_info: (transfer none) (allow-none): pointer to optional additional info
 * @additional_info_length: (allow-none): length of the optional @additional_info
 *
 * Creates a %GST_MTS_DESC_REGISTRATION #GstMpegtsDescriptor
 *
 * Return: #GstMpegtsDescriptor, %NULL on failure
 */
GstMpegtsDescriptor *
gst_mpegts_descriptor_from_registration (const gchar * format_identifier,
    guint8 * additional_info, gsize additional_info_length)
{
  GstMpegtsDescriptor *descriptor;

  g_return_val_if_fail (format_identifier != NULL, NULL);

  descriptor = _new_descriptor (GST_MTS_DESC_REGISTRATION,
      4 + additional_info_length);

  memcpy (descriptor->data + 2, format_identifier, 4);
  if (additional_info && (additional_info_length > 0))
    memcpy (descriptor->data + 6, additional_info, additional_info_length);

  return descriptor;
}

/**
 * gst_mpegts_descriptor_from_registration_int:
 * @format_identifier: (transfer none): a 32-bit unsigned format identifier
 * @additional_info: (transfer none) (allow-none): pointer to optional additional info
 * @additional_info_length: length of the optional @additional_info
 *
 * Creates a %GST_MTS_DESC_REGISTRATION #GstMpegtsDescriptor
 *
 * Return: #GstMpegtsDescriptor, %NULL on failure
 */
GstMpegtsDescriptor *
gst_mpegts_descriptor_from_registration_int (guint32 format_identifier,
    guint8 * additional_info, gsize additional_info_length)
{
    return gst_mpegts_descriptor_from_registration((const gchar*)&format_identifier,additional_info,additional_info_length);
}

/* GST_MTS_DESC_DATA_STREAM_ALIGNMENT (0x06) */

/**
 * gst_mpegts_descriptor_parse_data_stream_alignment:
 * @descriptor: a %GST_MTS_DESC_DATA_STREAM_ALIGNMENT #GstMpegtsDescriptor
 * @alignment_type: (out) : alignment type for video stream
 *
 * Extracts the data stream alignment information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_data_stream_alignment (const GstMpegtsDescriptor *descriptor,
                      guint8 *alignment_type)
{
    guint8 *data;

    g_return_val_if_fail (alignment_type != NULL, FALSE);
    /* Size should be 1 byte */
    __common_desc_checks (descriptor, GST_MTS_DESC_DATA_STREAM_ALIGNMENT, 1, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *alignment_type = *data;

    return TRUE;
}

/* GST_MTS_DESC_TARGET_BACKGROUND_GRID (0x07) */

/**
 * gst_mpegts_descriptor_parse_background_grid:
 * @descriptor: a %GST_MTS_DESC_TARGET_BACKGROUND_GRID #GstMpegtsDescriptor
 * @horizontal_size: (out) : horizontal size of the target background
 * @vertical_size: (out) : vertical size of the target background
 * @aspect_ratio_information: (out) : sample or display aspect ratio
 *
 * Extracts the background grid information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_background_grid (const GstMpegtsDescriptor *descriptor,
                      guint16 *horizontal_size,
                      guint16 *vertical_size,
                      guint8  *aspect_ratio_information)
{
    guint8 *data;

    g_return_val_if_fail (horizontal_size != NULL && vertical_size != NULL && aspect_ratio_information != NULL, FALSE);
    /* Size should be 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_DATA_STREAM_ALIGNMENT, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *horizontal_size = (GST_READ_UINT16_BE (data) >> 2);
    *vertical_size = ((GST_READ_UINT32_BE (data) & 0x0003fff0) >> 4);
    data+= 3;
    *aspect_ratio_information = (*data & 0x0f);

    return TRUE;
}

/* GST_MTS_DESC_VIDEO_WINDOW (0x08) */

/**
 * gst_mpegts_descriptor_parse_video_window:
 * @descriptor: a %GST_MTS_DESC_VIDEO_WINDOW #GstMpegtsDescriptor
 * @horizontal_offset: (out) : horizontal offset of the video window
 * @vertical_offset: (out) : vertical offset of the video window
 * @window_priority: (out) : window priority
 *
 * Extracts the video window information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_video_window (const GstMpegtsDescriptor *descriptor,
                      guint16 *horizontal_offset,
                      guint16 *vertical_offset,
                      guint8  *window_priority)
{
    guint8 *data;

    g_return_val_if_fail (horizontal_offset != NULL && vertical_offset != NULL && window_priority != NULL, FALSE);
    /* Size should be 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_DATA_STREAM_ALIGNMENT, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *horizontal_offset = (GST_READ_UINT16_BE (data) >> 2);
    *vertical_offset = ((GST_READ_UINT32_BE (data) & 0x0003fff0) >> 4);
    data+= 3;
    *window_priority = (*data & 0x0f);

    return TRUE;
}

/* GST_MTS_DESC_CA (0x09) */

/**
 * gst_mpegts_descriptor_parse_ca:
 * @descriptor: a %GST_MTS_DESC_CA #GstMpegtsDescriptor
 * @ca_system_id: (out): the type of CA system used
 * @ca_pid: (out): The PID containing ECM or EMM data
 * @private_data: (out) (allow-none): The private data
 * @private_data_size: (out) (allow-none): The size of @private_data in bytes
 *
 * Extracts the Conditional Access information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */

gboolean
gst_mpegts_descriptor_parse_ca (GstMpegtsDescriptor * descriptor,
    guint16 * ca_system_id, guint16 * ca_pid,
    const guint8 ** private_data, gsize * private_data_size)
{
  guint8 *data;

  g_return_val_if_fail (descriptor != NULL && ca_system_id != NULL
      && ca_pid != NULL, FALSE);
  /* The smallest CA is 4 bytes (though not having any private data
   * sounds a bit ... weird) */
  __common_desc_checks (descriptor, GST_MTS_DESC_CA, 4, FALSE);

  data = (guint8 *) descriptor->data + 2;
  *ca_system_id = GST_READ_UINT16_BE (data);
  data += 2;
  *ca_pid = GST_READ_UINT16_BE (data) & 0x1fff;
  data += 2;
  if (private_data && private_data_size) {
    *private_data = data;
    *private_data_size = descriptor->length - 4;
  }

  return TRUE;
}

/* GST_MTS_DESC_ISO_639_LANGUAGE (0x0A) */
static GstMpegtsISO639LanguageDescriptor *
_gst_mpegts_iso_639_language_descriptor_copy (GstMpegtsISO639LanguageDescriptor
    * source)
{
  GstMpegtsISO639LanguageDescriptor *copy;
  guint i;

  copy = g_slice_dup (GstMpegtsISO639LanguageDescriptor, source);

  for (i = 0; i < source->nb_language; i++) {
    copy->language[i] = g_strdup (source->language[i]);
  }

  return copy;
}

void
gst_mpegts_iso_639_language_descriptor_free (GstMpegtsISO639LanguageDescriptor
    * desc)
{
  guint i;

  for (i = 0; i < desc->nb_language; i++) {
    g_free (desc->language[i]);
  }
  g_slice_free (GstMpegtsISO639LanguageDescriptor, desc);
}

G_DEFINE_BOXED_TYPE (GstMpegtsISO639LanguageDescriptor,
    gst_mpegts_iso_639_language,
    (GBoxedCopyFunc) _gst_mpegts_iso_639_language_descriptor_copy,
    (GFreeFunc) gst_mpegts_iso_639_language_descriptor_free);

/**
 * gst_mpegts_descriptor_parse_iso_639_language:
 * @descriptor: a %GST_MTS_DESC_ISO_639_LANGUAGE #GstMpegtsDescriptor
 * @res: (out) (transfer full): the #GstMpegtsISO639LanguageDescriptor to fill
 *
 * Extracts the iso 639-2 language information from @descriptor.
 *
 * Note: Use #gst_tag_get_language_code if you want to get the the
 * ISO 639-1 language code from the returned ISO 639-2 one.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean
gst_mpegts_descriptor_parse_iso_639_language (const GstMpegtsDescriptor *
    descriptor, GstMpegtsISO639LanguageDescriptor ** desc)
{
  guint i;
  guint8 *data;
  GstMpegtsISO639LanguageDescriptor *res;

  g_return_val_if_fail (descriptor != NULL && desc != NULL, FALSE);
  /* This descriptor can be empty, no size check needed */
  __common_desc_check_base (descriptor, GST_MTS_DESC_ISO_639_LANGUAGE, FALSE);

  data = (guint8 *) descriptor->data + 2;

  res = g_slice_new0 (GstMpegtsISO639LanguageDescriptor);

  /* Each language is 3 + 1 bytes */
  res->nb_language = descriptor->length / 4;
  for (i = 0; i < res->nb_language; i++) {
    res->language[i] = convert_lang_code (data);
    res->audio_type[i] = data[3];
    data += 4;
  }

  *desc = res;

  return TRUE;

}

/**
 * gst_mpegts_descriptor_parse_iso_639_language_idx:
 * @descriptor: a %GST_MTS_DESC_ISO_639_LANGUAGE #GstMpegtsDescriptor
 * @idx: Table id of the language to parse
 * @lang: (out) (transfer full): 4-byte gchar array to hold the language code
 * @audio_type: (out) (transfer none) (allow-none): the #GstMpegtsIso639AudioType to set
 *
 * Extracts the iso 639-2 language information from specific table id in @descriptor.
 *
 * Note: Use #gst_tag_get_language_code if you want to get the the
 * ISO 639-1 language code from the returned ISO 639-2 one.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean
gst_mpegts_descriptor_parse_iso_639_language_idx (const GstMpegtsDescriptor *
    descriptor, guint idx, gchar ** lang, GstMpegtsIso639AudioType * audio_type)
{
  guint8 *data;

  g_return_val_if_fail (descriptor != NULL && lang != NULL, FALSE);
  /* This descriptor can be empty, no size check needed */
  __common_desc_check_base (descriptor, GST_MTS_DESC_ISO_639_LANGUAGE, FALSE);

  if (descriptor->length / 4 <= idx)
    return FALSE;

  data = (guint8 *) descriptor->data + 2 + idx * 4;

  *lang = convert_lang_code (data);

  data += 3;

  if (audio_type)
    *audio_type = *data;

  return TRUE;
}

/**
 * gst_mpegts_descriptor_parse_iso_639_language_nb:
 * @descriptor: a %GST_MTS_DESC_ISO_639_LANGUAGE #GstMpegtsDescriptor
 *
 * Returns: The number of languages in @descriptor
 */
guint
gst_mpegts_descriptor_parse_iso_639_language_nb (const GstMpegtsDescriptor *
    descriptor)
{
  g_return_val_if_fail (descriptor != NULL, 0);
  /* This descriptor can be empty, no size check needed */
  __common_desc_check_base (descriptor, GST_MTS_DESC_ISO_639_LANGUAGE, FALSE);

  return descriptor->length / 4;
}

/* GST_MTS_DESC_SYSTEM_CLOCK (0x0B) */

/**
 * gst_mpegts_descriptor_parse_system_clock:
 * @descriptor: a %GST_MTS_DESC_SYSTEM_CLOCK #GstMpegtsDescriptor
 * @external_clock_reference_indicator: (out) :
 * @clock_accuracy_integer: (out) :
 * @clock_accuracy_exponent: (out) :
 *
 * Extracts the video window information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_system_clock (const GstMpegtsDescriptor *descriptor,
                      gboolean *external_clock_reference_indicator,
                      guint8   *clock_accuracy_integer,
                      guint8   *clock_accuracy_exponent)
{
    guint8 *data;

    g_return_val_if_fail (external_clock_reference_indicator != NULL && clock_accuracy_integer != NULL && clock_accuracy_exponent != NULL, FALSE);
    /* Size should be 2 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_DATA_STREAM_ALIGNMENT, 2, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *external_clock_reference_indicator = ((*data & 0x80) == 0x80);
    *clock_accuracy_integer = (*data & 0x3f);
    data++;
    *clock_accuracy_exponent = ((*data & 0xe0) >> 5);

    return TRUE;
}

/* GST_MTS_DESC_MULTIPLEX_BUFFER_UTILIZATION (0x0C) */

/**
 * gst_mpegts_descriptor_parse_multiplex_buffer_utilization:
 * @descriptor: a %GST_MTS_DESC_MULTIPLEX_BUFFER_UTILIZATION #GstMpegtsDescriptor
 * @bound_valid: (out) :
 * @LTW_offset_lower_bound: (out) :
 * @LTW_offset_upper_bound: (out) :
 *
 * Extracts the multiplex buffer utilization information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_multiplex_buffer_utilization (const GstMpegtsDescriptor *descriptor,
                      gboolean *bound_valid,
                      guint16   *LTW_offset_lower_bound,
                      guint16   *LTW_offset_upper_bound)
{
    guint8 *data;

    g_return_val_if_fail (bound_valid != NULL && LTW_offset_lower_bound != NULL && LTW_offset_upper_bound != NULL, FALSE);
    /* Size should be 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_DATA_STREAM_ALIGNMENT, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *bound_valid = ((*data & 0x80) == 0x80);
    *LTW_offset_lower_bound = (GST_READ_UINT16_BE (data) & 0x7fff);
    data += 2;
    *LTW_offset_upper_bound = (GST_READ_UINT16_BE (data) & 0x7fff);

    return TRUE;
}

/* GST_MTS_DESC_COPYRIGHT (0x0D) */

/**
 * gst_mpegts_descriptor_parse_copyright:
 * @descriptor: a %GST_MTS_DESC_COPYRIGHT #GstMpegtsDescriptor
 * @copyright_identifier: (out) :
 * @additional_copyright_info: (out) (allow-none) :
 * @additional_copyright_info_size: (out) (allow-none) :
 *
 * Extracts the copyright information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_copyright (const GstMpegtsDescriptor *descriptor,
                      guint32  *copyright_identifier,
                      const guint8 **additional_copyright_info,
                      gsize *additional_copyright_info_size)
{
    guint8 *data;

    g_return_val_if_fail (copyright_identifier != NULL, FALSE);
    /* Size should be 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_COPYRIGHT, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *copyright_identifier = GST_READ_UINT32_BE (data);

    data += 4;
    if (additional_copyright_info && additional_copyright_info_size) {
      *additional_copyright_info = data;
      *additional_copyright_info_size = descriptor->length - 4;
    }

    return TRUE;
}

/* GST_MTS_DESC_MAXIMUM_BITRATE (0x0E) */

/**
 * gst_mpegts_descriptor_parse_maximum_bitrate:
 * @descriptor: a %GST_MTS_DESC_MAXIMUM_BITRATE #GstMpegtsDescriptor
 * @maximum_bitrate: (out) : maximum_bitrate
 *
 * Extracts the maximum bitrate from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_maximum_bitrate (const GstMpegtsDescriptor *descriptor,
                      guint32  *maximum_bitrate)
{
    guint8 *data;

    g_return_val_if_fail (maximum_bitrate != NULL, FALSE);
    /* Size should be 3 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_MAXIMUM_BITRATE, 3, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *maximum_bitrate = (((((guint32)*data) & 0x3f) << 16) |
                       GST_READ_UINT16_BE (data+1));

    return TRUE;
}

/* GST_MTS_DESC_PRIVATE_DATA_INDICATOR (0x0F) */

/**
 * gst_mpegts_descriptor_parse_private_data_indicator:
 * @descriptor: a %GST_MTS_DESC_PRIVATE_DATA_INDICATOR #GstMpegtsDescriptor
 * @indicator: (out) :
 *
 * Extracts the private data indicator information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_private_data_indicator (const GstMpegtsDescriptor *descriptor,
                      guint32  *indicator)
{
    guint8 *data;

    g_return_val_if_fail (indicator != NULL, FALSE);
    /* Size should be 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_PRIVATE_DATA_INDICATOR, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *indicator = GST_READ_UINT32_BE (data);

    return TRUE;
}

/* GST_MTS_DESC_SMOOTHING_BUFFER (0x10) */

/**
 * gst_mpegts_descriptor_parse_smoothing_buffer:
 * @descriptor: a %GST_MTS_DESC_SMOOTHING_BUFFER #GstMpegtsDescriptor
 * @sb_leak_rate: (out) :
 * @sb_size: (out) :
 *
 * Extracts the smoothing buffer information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_smoothing_buffer (const GstMpegtsDescriptor *descriptor,
                      guint32  *sb_leak_rate,
                      guint32  *sb_size)
{
    guint8 *data;

    g_return_val_if_fail (sb_leak_rate != NULL && sb_size != NULL, FALSE);
    /* Size should be 6 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_SMOOTHING_BUFFER, 6, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *sb_leak_rate = (((((guint32)*data) & 0x3f) << 16) |
                       GST_READ_UINT16_BE (data+1));

    data += 3;
    *sb_size = (((((guint32)*data) & 0x3f) << 16) |
                       GST_READ_UINT16_BE (data+1));

    return TRUE;
}

/* GST_MTS_DESC_STD (0x11) */

/**
 * gst_mpegts_descriptor_parse_std:
 * @descriptor: a %GST_MTS_DESC_STD #GstMpegtsDescriptor
 * @leak_valid: (out) :
 *
 * Extracts the std information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_std (const GstMpegtsDescriptor *descriptor,
                      gboolean  *leak_valid)
{
    guint8 *data;

    g_return_val_if_fail (leak_valid != NULL, FALSE);
    /* Size should be 1 byte */
    __common_desc_checks (descriptor, GST_MTS_DESC_STD, 1, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *leak_valid = ((*data & 0x01) == 0x01);

    return TRUE;
}

/* GST_MTS_DESC_IBP (0x12) */

/**
 * gst_mpegts_descriptor_parse_ibp:
 * @descriptor: a %GST_MTS_DESC_IBP #GstMpegtsDescriptor
 * @closed_gop: (out) :
 * @identical_gop: (out) :
 * @max_gop_length: (out) :
 *
 * Extracts the IBP information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_ibp (const GstMpegtsDescriptor *descriptor,
                      gboolean  *closed_gop,
                      gboolean  *identical_gop,
                      guint16   *max_gop_length)
{
    guint8 *data;

    g_return_val_if_fail (closed_gop != NULL && identical_gop != NULL && max_gop_length != NULL, FALSE);
    /* Size should be 2 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_IBP, 2, FALSE);

    data = (guint8 *) descriptor->data + 2;

    *closed_gop = ((*data & 0x80) == 0x80);
    *identical_gop = ((*data & 0x40) == 0x40);
    *max_gop_length = (GST_READ_UINT16_BE (data) & 0x3fff);

    return TRUE;
}

/* GST_MTS_DESC_CONTENT_LABELING (0x24) */

void gst_mpegts_content_labeling_descriptor_free (GstMpegtsContentLabelingDescriptor * desc)
{
    g_slice_free (GstMpegtsContentLabelingDescriptor, desc);
}

/**
 * gst_mpegts_descriptor_parse_content_labeling:
 * @descriptor: a %GST_MTS_DESC_CONTENT_LABELING #GstMpegtsDescriptor
 * @res: (out) (transfer full): the #GstMpegtsContentLabelingDescriptor to fill
 *
 * Extracts the content labeling information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_content_labeling (const GstMpegtsDescriptor *descriptor,
                      GstMpegtsContentLabelingDescriptor **desc)
{
    guint8 *data;
    GstMpegtsContentLabelingDescriptor *res;

    g_return_val_if_fail (desc != NULL, FALSE);
    /* Size should be at least 2 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_CONTENT_LABELING, 2, FALSE);

    data = (guint8 *) descriptor->data + 2;

    res = g_slice_new0 (GstMpegtsContentLabelingDescriptor);

    res->application_format = (GST_READ_UINT16_BE (data));
    data += 2;

    if ( res->application_format == 0xFFFF && descriptor->length < 6 )
        goto error;
    else {
        res->application_format_identifier = (GST_READ_UINT32_BE (data));
        data += 4;
    }

    *desc = res;

    return TRUE;

error:
    g_slice_free(GstMpegtsContentLabelingDescriptor, res);
    return FALSE;
}

/* GST_MTS_DESC_METADATA_POINTER (0x25) */

void gst_mpegts_metadata_pointer_descriptor_free (GstMpegtsMetadataPointerDescriptor * desc)
{
    g_slice_free (GstMpegtsMetadataPointerDescriptor, desc);
}

/**
 * gst_mpegts_descriptor_metadata_pointer:
 * @descriptor: a %GST_MTS_DESC_METADATA_POINTER #GstMpegtsDescriptor
 * @res: (out) (transfer full): the #GstMpegtsMetadataPointerDescriptor to fill
 *
 * Extracts the metadata pointer information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_metadata_pointer (const GstMpegtsDescriptor *descriptor,
        GstMpegtsMetadataPointerDescriptor **desc)
{
    guint8 *data;
    GstMpegtsMetadataPointerDescriptor *res;
    guint32 length;

    g_return_val_if_fail (desc != NULL, FALSE);
    /* Size should be at least 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_METADATA_POINTER, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;
    length = descriptor->length;

    res = g_slice_new0 (GstMpegtsMetadataPointerDescriptor);

    res->application_format = (GST_READ_UINT16_BE (data));
    data += 2;
    length -= 2;

    if ( res->application_format == 0xFFFF ) {
        if ( length < 4) {
            goto error;
        } else {
            res->application_format_identifier = (GST_READ_UINT32_BE (data));
            data += 4;
            length -= 4;
        }
    }

    if ( length < 1 ) goto error;
    res->format = *data;
    data++;
    length--;

    if ( res->format == 0xFF ) {
        if ( length < 4 ) {
            goto error;
        } else {
            res->format_identifier = (GST_READ_UINT32_BE (data));
            data += 4;
            length -= 4;
        }
    }

    if ( length < 1 ) goto error;
    res->service_id = *data;

    *desc = res;

    return TRUE;

error:
    g_slice_free(GstMpegtsMetadataPointerDescriptor, res);
    return FALSE;
}

/* GST_MTS_DESC_METADATA (0x26) */

void gst_mpegts_metadata_descriptor_free (GstMpegtsMetadataDescriptor * desc)
{
    g_slice_free (GstMpegtsMetadataDescriptor, desc);
}

/**
 * gst_mpegts_descriptor_metadata:
 * @descriptor: a %GST_MTS_DESC_METADATA #GstMpegtsDescriptor
 * @res: (out) (transfer full): the #GstMpegtsMetadataDescriptor to fill
 *
 * Extracts the metadata information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_metadata (const GstMpegtsDescriptor *descriptor,
                      GstMpegtsMetadataDescriptor **desc)
{
    guint8 *data;
    GstMpegtsMetadataDescriptor *res;
    guint32 length;

    g_return_val_if_fail (desc != NULL, FALSE);
    /* Size should be at least 4 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_METADATA, 4, FALSE);

    data = (guint8 *) descriptor->data + 2;
    length = descriptor->length;

    res = g_slice_new0 (GstMpegtsMetadataDescriptor);

    res->application_format = (GST_READ_UINT16_BE (data));
    data += 2;
    length -= 2;

    if ( res->application_format == 0xFFFF ) {
        if ( length < 4) {
            goto error;
        } else {
            res->application_format_identifier = (GST_READ_UINT32_BE (data));
            data += 4;
            length -= 4;
        }
    }

    if ( length < 1 ) goto error;
    res->format = *data;
    data++;
    length--;

    if ( res->format == 0xFF ) {
        if ( length < 4 ) {
            goto error;
        } else {
            res->format_identifier = (GST_READ_UINT32_BE (data));
            data += 4;
            length -= 4;
        }
    }

    if ( length < 1 ) goto error;
    res->service_id = *data;

    *desc = res;

    return TRUE;

error:
    g_slice_free(GstMpegtsMetadataDescriptor, res);
    return FALSE;
}

/* GST_MTS_DESC_METADATA_STD (0x27) */

void gst_mpegts_metadata_STD_descriptor_free (GstMpegtsMetadataSTDDescriptor * desc)
{
    g_slice_free (GstMpegtsMetadataSTDDescriptor, desc);
}

/**
 * gst_mpegts_descriptor_parse_metadata_STD:
 * @descriptor: a %GST_MTS_DESC_METADATA_STD #GstMpegtsDescriptor
 * @res: (out) (transfer full): the #GstMpegtsMetadataSTDDescriptor to fill
 *
 * Extracts the metadata STD information from @descriptor.
 *
 * Returns: %TRUE if parsing succeeded, else %FALSE.
 */
gboolean  gst_mpegts_descriptor_parse_metadata_STD (const GstMpegtsDescriptor *descriptor,
                      GstMpegtsMetadataSTDDescriptor **desc)
{
    guint8 *data;
    GstMpegtsMetadataSTDDescriptor *res;

    g_return_val_if_fail (desc != NULL, FALSE);
    /* Size should be at least 9 bytes */
    __common_desc_checks (descriptor, GST_MTS_DESC_METADATA_STD, 9, FALSE);

    data = (guint8 *) descriptor->data + 2;

    res = g_slice_new0 (GstMpegtsMetadataSTDDescriptor);

    res->input_leak_rate = (((((guint32)*data) & 0x3f) << 16) |
                       GST_READ_UINT16_BE (data+1));

    data += 3;
    res->buffer_size = (((((guint32)*data) & 0x3f) << 16) |
                       GST_READ_UINT16_BE (data+1));

    data += 3;
    res->output_leak_rate = (((((guint32)*data) & 0x3f) << 16) |
                       GST_READ_UINT16_BE (data+1));

    *desc = res;

    return TRUE;
}

/**
 * gst_mpegts_descriptor_from_custom:
 * @tag: descriptor tag
 * @data: (transfer none): descriptor data (after tag and length field)
 * @length: length of @data
 *
 * Creates a #GstMpegtsDescriptor with custom @tag and @data
 *
 * Returns: #GstMpegtsDescriptor
 */
GstMpegtsDescriptor *
gst_mpegts_descriptor_from_custom (guint8 tag, const guint8 * data,
    gsize length)
{
  GstMpegtsDescriptor *descriptor;

  descriptor = _new_descriptor (tag, length);

  if (data && (length > 0))
    memcpy (descriptor->data + 2, data, length);

  return descriptor;
}
