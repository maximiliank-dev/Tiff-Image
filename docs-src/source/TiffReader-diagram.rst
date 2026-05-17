==========================
TiffReader Class Hierarchy
==========================

.. graphviz::

   digraph TiffReader {
       rankdir=TB
       node [shape=none, margin=0, fontname="Helvetica", fontsize=10]
       edge [fontname="Helvetica", fontsize=9]

       {rank=same; TIFFHeader; TiffIFD; TiffReadStrips}
       {rank=same; TiffReadStrips_RGB; VirtualEndianHandler}
       {rank=same; LittleEndian_TIFF; BigEndian_TIFF}
       {rank=same; LZW; PackBits; ImageContainer}

       TiffReader [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#2E86C1" ALIGN="CENTER"><FONT COLOR="white"><B>TiffReader</B></FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _header : optional&lt;TIFFHeader&gt;<BR ALIGN="LEFT"/>
               - _ifds : optional&lt;TiffIFD&gt;<BR ALIGN="LEFT"/>
               - _strips : optional&lt;TiffReadStrips&gt;<BR ALIGN="LEFT"/>
               - _file : ifstream<BR ALIGN="LEFT"/>
               - _data_valid : bool<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + TiffReader(path : filesystem::path)<BR ALIGN="LEFT"/>
               + read() : void<BR ALIGN="LEFT"/>
               + get_image() : ImageContainer&lt;uint8_t&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       TIFFHeader [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#5DADE2" ALIGN="CENTER"><B>TIFFHeader</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _stream : istream&amp;<BR ALIGN="LEFT"/>
               - magic_num : ushort_t<BR ALIGN="LEFT"/>
               - idf_offset : uint_t<BR ALIGN="LEFT"/>
               - endian_handler : shared_ptr&lt;VirtualEndianHandler&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + parse_header() : void<BR ALIGN="LEFT"/>
               + get_idf_offset() : uint_t<BR ALIGN="LEFT"/>
               + get_endian_handler() : shared_ptr&lt;VirtualEndianHandler&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       TiffIFD [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#5DADE2" ALIGN="CENTER"><B>TiffIFD</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _stream : istream&amp;<BR ALIGN="LEFT"/>
               - _address : uint_t<BR ALIGN="LEFT"/>
               - _tagList : vector&lt;TiffTag&gt;<BR ALIGN="LEFT"/>
               - _endian_handler : shared_ptr&lt;VirtualEndianHandler&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + read() : void<BR ALIGN="LEFT"/>
               + getTag(tag : TiffTagType) : TiffTag<BR ALIGN="LEFT"/>
               + read_tiff_tag(tag : TiffTag) : TiffTagRead<BR ALIGN="LEFT"/>
               + get_endian_handler() : shared_ptr&lt;VirtualEndianHandler&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       TiffReadStrips [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#5DADE2" ALIGN="CENTER"><B>TiffReadStrips</B><BR/><FONT POINT-SIZE="8">«abstract»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _stream : istream&amp;<BR ALIGN="LEFT"/>
               - _ifd : TiffIFD&amp;<BR ALIGN="LEFT"/>
               - _endian_handler : shared_ptr&lt;VirtualEndianHandler&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + readStrips() : string<BR ALIGN="LEFT"/>
               + get_image() : ImageContainer&lt;uint8_t&gt;<BR ALIGN="LEFT"/>
               + reformat_strip(strip : string&amp;) : void «virtual»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       TiffReadStrips_RGB [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#85C1E9" ALIGN="CENTER"><B>TiffReadStrips_RGB</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(inherits all)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + reformat_strip(strip : string&amp;) : void «override»<BR ALIGN="LEFT"/>
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
               + append_row(container) : void<BR ALIGN="LEFT"/>
               + get_data() : vector&lt;PT&gt;<BR ALIGN="LEFT"/>
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

       // Inheritance (hollow triangle arrowhead)
       TiffReadStrips_RGB -> TiffReadStrips      [arrowhead=onormal, style=solid]
       LittleEndian_TIFF  -> VirtualEndianHandler [arrowhead=onormal, style=solid]
       BigEndian_TIFF     -> VirtualEndianHandler [arrowhead=onormal, style=solid]

       // Composition: TiffReader owns its components (filled diamond at owner)
       TiffReader -> TIFFHeader     [arrowhead=none, arrowtail=diamond, dir=back]
       TiffReader -> TiffIFD        [arrowhead=none, arrowtail=diamond, dir=back]
       TiffReader -> TiffReadStrips [arrowhead=none, arrowtail=diamond, dir=back]

       // Dependencies (dashed open arrows)
       TiffReader     -> ImageContainer       [arrowhead=open, style=dashed, label="«creates»"]
       TIFFHeader     -> VirtualEndianHandler [arrowhead=open, style=dashed, label="«uses»"]
       TiffIFD        -> VirtualEndianHandler [arrowhead=open, style=dashed, label="«uses»"]
       TiffReadStrips -> VirtualEndianHandler [arrowhead=open, style=dashed, label="«uses»"]
       TiffReadStrips -> LZW                  [arrowhead=open, style=dashed, label="«uses»"]
       TiffReadStrips -> PackBits             [arrowhead=open, style=dashed, label="«uses»"]
       TiffReadStrips -> ImageContainer       [arrowhead=open, style=dashed, label="«creates»"]
   }
