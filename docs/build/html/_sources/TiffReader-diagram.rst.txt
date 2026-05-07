==========================
TiffReader Class Hierarchy
==========================

.. graphviz::

   digraph TiffReader {
       rankdir=TB
       node [shape=none, margin=0, fontname="Helvetica", fontsize=10]
       edge [fontname="Helvetica", fontsize=9]

       TiffReader [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#2E86C1" ALIGN="CENTER"><FONT COLOR="white"><B>TiffReader</B></FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _header : optional&lt;TIFFHeader&gt;<BR ALIGN="LEFT"/>
               - _ifds : optional&lt;TiffIFD&gt;<BR ALIGN="LEFT"/>
               - _strips : optional&lt;TiffReadStrips&gt;<BR ALIGN="LEFT"/>
               - _file : ifstream<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + read()<BR ALIGN="LEFT"/>
               + get_image() : ImageContainer<BR ALIGN="LEFT"/>
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
               - endian_handler : shared_ptr<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + parse_header()<BR ALIGN="LEFT"/>
               + get_idf_offset() : uint_t<BR ALIGN="LEFT"/>
               + get_endian_handler()<BR ALIGN="LEFT"/>
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
               - _endian_handler : shared_ptr<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + read()<BR ALIGN="LEFT"/>
               + getTag(TiffTagType) : TiffTag<BR ALIGN="LEFT"/>
               + read_tiff_tag(TiffTag) : TiffTagRead<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       TiffReadStrips [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#5DADE2" ALIGN="CENTER"><B>TiffReadStrips</B><BR/><FONT POINT-SIZE="8">«abstract»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _stream : istream&amp;<BR ALIGN="LEFT"/>
               - _ifd : TiffIFD&amp;<BR ALIGN="LEFT"/>
               - _endian_handler : shared_ptr<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + readStrips() : string<BR ALIGN="LEFT"/>
               + get_image() : ImageContainer<BR ALIGN="LEFT"/>
               + reformat_strip() «virtual»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       TiffReadStrips_RGB [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#85C1E9" ALIGN="CENTER"><B>TiffReadStrips_RGB</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(inherits all)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + reformat_strip() «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       VirtualEndianHandler [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#1E8449" ALIGN="CENTER"><FONT COLOR="white"><B>VirtualEndianHandler</B></FONT><BR/><FONT COLOR="white" POINT-SIZE="8">«abstract»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(no fields)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + convert(...) «pure virtual»<BR ALIGN="LEFT"/>
               + convert_to_array(...) «pure virtual»<BR ALIGN="LEFT"/>
               + get_endian_value() «pure virtual»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       LittleEndian_TIFF [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#52BE80" ALIGN="CENTER"><B>LittleEndian_TIFF</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(no fields)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + convert(...) «override»<BR ALIGN="LEFT"/>
               + convert_to_array(...) «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       ImageContainer [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#7D3C98" ALIGN="CENTER"><FONT COLOR="white"><B>ImageContainer&lt;PT&gt;</B></FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _img : vector&lt;PT&gt;<BR ALIGN="LEFT"/>
               - _row_length : size_t<BR ALIGN="LEFT"/>
               - _pixel_number_of_colors : size_t<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + at(row, col, px) : PT<BR ALIGN="LEFT"/>
               + get_width() : size_t<BR ALIGN="LEFT"/>
               + get_height() : size_t<BR ALIGN="LEFT"/>
               + append_row(container)<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       // Inheritance (hollow triangle arrowhead)
       TiffReadStrips_RGB -> TiffReadStrips [arrowhead=onormal, style=solid]
       LittleEndian_TIFF  -> VirtualEndianHandler [arrowhead=onormal, style=solid]

       // Composition: TiffReader owns its components
       TiffReader -> TIFFHeader      [arrowhead=open, style=dashed, label="uses"]
       TiffReader -> TiffIFD         [arrowhead=open, style=dashed, label="uses"]
       TiffReader -> TiffReadStrips  [arrowhead=open, style=dashed, label="uses"]
       TiffReader -> ImageContainer  [arrowhead=open, style=dashed, label="produces"]

       // Shared infrastructure
       TIFFHeader      -> VirtualEndianHandler [arrowhead=open, style=dashed]
       TiffIFD         -> VirtualEndianHandler [arrowhead=open, style=dashed]
       TiffReadStrips  -> VirtualEndianHandler [arrowhead=open, style=dashed]
   }
