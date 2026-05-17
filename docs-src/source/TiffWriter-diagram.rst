==========================
TiffWriter Class Hierarchy
==========================

.. graphviz::

   digraph TiffWriter {
       rankdir=TB
       node [shape=none, margin=0, fontname="Helvetica", fontsize=10]
       edge [fontname="Helvetica", fontsize=9]

       {rank=same; TiffWriterHeader; TiffWriteData}
       {rank=same; BitlevelImage; VirtualEndianHandler}
       {rank=same; GrayImage; RGBImage; LittleEndian_TIFF; BigEndian_TIFF}
       {rank=same; ImageContainer; PackBits; LZW}

       TiffWriter [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#922B21" ALIGN="CENTER"><FONT COLOR="white"><B>TiffWriter</B></FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _file : ofstream<BR ALIGN="LEFT"/>
               - _header : optional&lt;TiffWriterHeader&gt;<BR ALIGN="LEFT"/>
               - _writer : unique_ptr&lt;TiffWriteData&lt;uint8_t&gt;&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + TiffWriter(path, type, img, compression, endian)<BR ALIGN="LEFT"/>
               + write() : void<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       TiffWriterHeader [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#E74C3C" ALIGN="CENTER"><FONT COLOR="white"><B>TiffWriterHeader</B></FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _stream : ostream&amp;<BR ALIGN="LEFT"/>
               - _magic_num : ushort_t<BR ALIGN="LEFT"/>
               - _endian : tiff_header_endian<BR ALIGN="LEFT"/>
               - _idf_offset : uint_t<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + write_header() : void<BR ALIGN="LEFT"/>
               + get_idf_offset() : uint_t<BR ALIGN="LEFT"/>
               + create_endian_handler() : shared_ptr&lt;VirtualEndianHandler&gt;<BR ALIGN="LEFT"/>
               + get_stream() : ostream&amp;<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       TiffWriteData [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#E74C3C" ALIGN="CENTER"><FONT COLOR="white"><B>TiffWriteData&lt;TP&gt;</B></FONT><BR/><FONT COLOR="white" POINT-SIZE="8">«abstract template»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               # _img : ImageContainer&lt;TP&gt;*<BR ALIGN="LEFT"/>
               # _stream : ostream&amp;<BR ALIGN="LEFT"/>
               # _values : map&lt;TiffTagType, TiffDataVariant&gt;<BR ALIGN="LEFT"/>
               # _endian_handler : shared_ptr&lt;VirtualEndianHandler&gt;<BR ALIGN="LEFT"/>
               # _strip_byte_count : size_t<BR ALIGN="LEFT"/>
               # _offset_data : uint64_t<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + write() : void<BR ALIGN="LEFT"/>
               + write_IFDs() : void<BR ALIGN="LEFT"/>
               + write_image_data() : void<BR ALIGN="LEFT"/>
               + compress(data) : void<BR ALIGN="LEFT"/>
               + apply_PhotometricInterpretation(data) : void<BR ALIGN="LEFT"/>
               + tags_to_set() : void «pure virtual»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       BitlevelImage [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#F1948A" ALIGN="CENTER"><B>BitlevelImage</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _config : TiffWriterConfig<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + BitlevelImage(header, img, stream, config)<BR ALIGN="LEFT"/>
               + tags_to_set() : void «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       GrayImage [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#FADBD8" ALIGN="CENTER"><B>GrayImage</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(inherits all)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + tags_to_set() : void «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       RGBImage [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#FADBD8" ALIGN="CENTER"><B>RGBImage</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(inherits all)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + tags_to_set() : void «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       VirtualEndianHandler [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#1E8449" ALIGN="CENTER"><FONT COLOR="white"><B>VirtualEndianHandler</B></FONT><BR/><FONT COLOR="white" POINT-SIZE="8">«abstract»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(no fields)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + convert(...) : T «pure virtual»<BR ALIGN="LEFT"/>
               + convert_to_array(...) : array «pure virtual»<BR ALIGN="LEFT"/>
               + get_endian_value() : tiff_header_endian «pure virtual»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       LittleEndian_TIFF [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#52BE80" ALIGN="CENTER"><B>LittleEndian_TIFF</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(no fields)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + convert(...) : T «override»<BR ALIGN="LEFT"/>
               + convert_to_array(...) : array «override»<BR ALIGN="LEFT"/>
               + get_endian_value() : tiff_header_endian «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       BigEndian_TIFF [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#52BE80" ALIGN="CENTER"><B>BigEndian_TIFF</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(no fields)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + convert(...) : T «override»<BR ALIGN="LEFT"/>
               + convert_to_array(...) : array «override»<BR ALIGN="LEFT"/>
               + get_endian_value() : tiff_header_endian «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       ImageContainer [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#7D3C98" ALIGN="CENTER"><FONT COLOR="white"><B>ImageContainer&lt;PT&gt;</B></FONT><BR/><FONT COLOR="white" POINT-SIZE="8">«template»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _img : vector&lt;PT&gt;<BR ALIGN="LEFT"/>
               - _row_length : size_t<BR ALIGN="LEFT"/>
               - _pixel_number_of_colors : size_t<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + at(row, col, px) : PT<BR ALIGN="LEFT"/>
               + get_width() : size_t<BR ALIGN="LEFT"/>
               + get_height() : size_t<BR ALIGN="LEFT"/>
               + get_data() : vector&lt;PT&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       PackBits [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#616A6B" ALIGN="CENTER"><FONT COLOR="white"><B>PackBits</B></FONT><BR/><FONT COLOR="white" POINT-SIZE="8">«utility»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(no fields)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + compress(data) : vector&lt;uint8_t&gt; «static»<BR ALIGN="LEFT"/>
               + decompress(data) : vector&lt;uint8_t&gt; «static»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       LZW [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#616A6B" ALIGN="CENTER"><FONT COLOR="white"><B>LZW</B></FONT><BR/><FONT COLOR="white" POINT-SIZE="8">«utility»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - compression_data_left : uint8_t<BR ALIGN="LEFT"/>
               - compression_number_bits_left : size_t<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + compress(data) : vector&lt;uint16_t&gt; «static»<BR ALIGN="LEFT"/>
               + decompress(data) : vector&lt;uint8_t&gt; «static»<BR ALIGN="LEFT"/>
               + compress_tiff(data) : vector&lt;uint8_t&gt;<BR ALIGN="LEFT"/>
               + decompress_tiff(data) : vector&lt;uint8_t&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       // Inheritance (hollow triangle arrowhead)
       BitlevelImage     -> TiffWriteData        [arrowhead=onormal, style=solid]
       GrayImage         -> BitlevelImage         [arrowhead=onormal, style=solid]
       RGBImage          -> BitlevelImage         [arrowhead=onormal, style=solid]
       LittleEndian_TIFF -> VirtualEndianHandler  [arrowhead=onormal, style=solid]
       BigEndian_TIFF    -> VirtualEndianHandler  [arrowhead=onormal, style=solid]

       // Composition: TiffWriter owns its components (filled diamond at owner)
       TiffWriter -> TiffWriterHeader [arrowhead=none, arrowtail=diamond, dir=back]
       TiffWriter -> TiffWriteData    [arrowhead=none, arrowtail=diamond, dir=back]

       // Dependencies (dashed open arrows)
       TiffWriterHeader -> VirtualEndianHandler [arrowhead=open, style=dashed, label="«creates»"]
       TiffWriteData    -> VirtualEndianHandler [arrowhead=open, style=dashed, label="«uses»"]
       TiffWriteData    -> ImageContainer       [arrowhead=open, style=dashed, label="«reads»"]
       TiffWriteData    -> PackBits             [arrowhead=open, style=dashed, label="«uses»"]
       TiffWriteData    -> LZW                  [arrowhead=open, style=dashed, label="«uses»"]
   }
