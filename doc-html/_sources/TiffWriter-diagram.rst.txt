==========================
TiffWriter Class Hierarchy
==========================

.. graphviz::

   digraph TiffWriter {
       rankdir=TB
       node [shape=none, margin=0, fontname="Helvetica", fontsize=10]
       edge [fontname="Helvetica", fontsize=9]

       TiffWriter [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#922B21" ALIGN="CENTER"><FONT COLOR="white"><B>TiffWriter</B></FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               - _file : ofstream<BR ALIGN="LEFT"/>
               - _header : optional&lt;TiffWriterHeader&gt;<BR ALIGN="LEFT"/>
               - _writer : unique_ptr&lt;TiffWriteData&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + write()<BR ALIGN="LEFT"/>
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
               + write_header()<BR ALIGN="LEFT"/>
               + get_idf_offset() : uint_t<BR ALIGN="LEFT"/>
               + create_endian_handler()<BR ALIGN="LEFT"/>
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
               # _endian_handler : shared_ptr<BR ALIGN="LEFT"/>
               # _strip_byte_count : size_t<BR ALIGN="LEFT"/>
               # _offset_data : uint64_t<BR ALIGN="LEFT"/>
           </TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + write()<BR ALIGN="LEFT"/>
               + write_IFDs()<BR ALIGN="LEFT"/>
               + write_image_data()<BR ALIGN="LEFT"/>
               + compress(data)<BR ALIGN="LEFT"/>
               + apply_PhotometricInterpretation(data)<BR ALIGN="LEFT"/>
               + tags_to_set() «pure virtual»<BR ALIGN="LEFT"/>
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
               + tags_to_set() «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       GrayImage [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#FADBD8" ALIGN="CENTER"><B>GrayImage</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(inherits all)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + tags_to_set() «override»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       RGBImage [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#FADBD8" ALIGN="CENTER"><B>RGBImage</B></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(inherits all)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + tags_to_set() «override»<BR ALIGN="LEFT"/>
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
               + get_width() / get_height()<BR ALIGN="LEFT"/>
               + get_data() : vector&lt;PT&gt;<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       PackBits [label=<
           <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4">
           <TR><TD BGCOLOR="#616A6B" ALIGN="CENTER"><FONT COLOR="white"><B>PackBits</B></FONT><BR/><FONT COLOR="white" POINT-SIZE="8">«utility»</FONT></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT"><I>(no fields)</I></TD></TR>
           <TR><TD ALIGN="LEFT" BALIGN="LEFT">
               + compress(data) «static»<BR ALIGN="LEFT"/>
               + decompress(data) «static»<BR ALIGN="LEFT"/>
           </TD></TR>
           </TABLE>
       >]

       // Inheritance (hollow triangle)
       BitlevelImage     -> TiffWriteData    [arrowhead=onormal, style=solid]
       GrayImage         -> BitlevelImage    [arrowhead=onormal, style=solid]
       RGBImage          -> BitlevelImage    [arrowhead=onormal, style=solid]
       LittleEndian_TIFF -> VirtualEndianHandler [arrowhead=onormal, style=solid]

       // TiffWriter composes its parts
       TiffWriter -> TiffWriterHeader [arrowhead=open, style=dashed, label="uses"]
       TiffWriter -> TiffWriteData    [arrowhead=open, style=dashed, label="owns"]

       // TiffWriteData dependencies
       TiffWriteData -> VirtualEndianHandler [arrowhead=open, style=dashed]
       TiffWriteData -> ImageContainer       [arrowhead=open, style=dashed, label="reads"]
       TiffWriteData -> PackBits             [arrowhead=open, style=dashed, label="uses"]
   }
