<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	xmlns:a="urn:dfranklin:bbxml:lookup" 
	version="1.1">

    <xsl:param name="signAddress">00</xsl:param>
    <xsl:param name="defaultLabel">A</xsl:param>
    <xsl:param name="century">20</xsl:param>

    <xsl:variable name="runPage">
        <xsl:choose>
            <xsl:when test="/signboard/text">
	            <xsl:choose>
	                <xsl:when test="/signboard/text/@label">
    	                <xsl:choose>
		                    <xsl:when test="string( number(/signboard/text/@label) ) = 'NaN'">
		                        <xsl:value-of select="/signboard/text/@label"/>
		                    </xsl:when>
		                    <xsl:otherwise>
		                        <xsl:value-of select="$defaultLabel"/>
		                    </xsl:otherwise>
		                </xsl:choose>
	                </xsl:when>
	                <xsl:otherwise>
	                    <xsl:value-of select="$defaultLabel"/>
	                </xsl:otherwise>
	            </xsl:choose>
            </xsl:when>
	    </xsl:choose>
    </xsl:variable>

    <!-- May also try hex code 0x0D(CR=&#13;) 0x0A(LF&#10;) -->
    <xsl:variable name="CRLF">
		<xsl:text>&#13;&#10;</xsl:text>
    </xsl:variable>
    
    <xsl:variable name="CRLF">
		<xsl:text>&#13;&#10;</xsl:text>
    </xsl:variable>

    <xsl:output method="text"/>
    <xsl:strip-space elements="*"/>
	<xsl:preserve-space elements="msg"/>

<!-- speed: <SPDX> where X is int -->	

	<xsl:template match="/signboard">
		<xsl:choose>
			<xsl:when test="/signboard/text">
				<xsl:text>&lt;ID10&gt;</xsl:text>
				<xsl:value-of select="$CRLF"/>
				<xsl:text>&lt;ID</xsl:text>
			    <xsl:choose>
			        <xsl:when test="@signAddress">
			            <xsl:value-of select="@signAddress"/>
			        </xsl:when>
			        <xsl:otherwise>
			            <xsl:value-of select="$signAddress"/>
			        </xsl:otherwise>
			    </xsl:choose>
			    <xsl:text>&gt;</xsl:text>
				<xsl:for-each select="text">
					<xsl:call-template name="text"/>
				</xsl:for-each>
			</xsl:when>
		</xsl:choose>
		<xsl:for-each select="*">
			<xsl:apply-templates select="."/>
		</xsl:for-each>
		<xsl:value-of select="$CRLF"/>
        <xsl:if test="$runPage != ''">
			<xsl:text>&lt;ID</xsl:text>
			<xsl:choose>
				<xsl:when test="@signAddress">
					<xsl:value-of select="@signAddress"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$signAddress"/>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text>&gt;</xsl:text>
            <xsl:text>&lt;RP</xsl:text>
            <xsl:value-of select="$runPage"/>
            <xsl:text>&gt;</xsl:text>
            <xsl:value-of select="$CRLF"/>
        </xsl:if>
	</xsl:template>

	<xsl:template name="text">
		<xsl:text>&lt;P</xsl:text>
		<xsl:value-of select="$runPage" />
		<xsl:text>&gt;</xsl:text>
		<xsl:if test="@alert">
			<xsl:text>&lt;M</xsl:text>
			<xsl:choose>
				<!-- 2X Fast beep, red flashing indicator lights -->
				<xsl:when test="@alert = 'priority'">
					<xsl:text>D</xsl:text>
				</xsl:when>
				<!-- Fast beep, red flashing indicator lights -->
				<xsl:when test="@alert = 'urgent'">
					<xsl:text>C</xsl:text>
				</xsl:when>
				<!-- Standard beep, red flashing indicator lights -->
				<xsl:when test="@alert = 'attention'">
					<xsl:text>B</xsl:text>
				</xsl:when>
				<!-- Standard beep, no flashing indicator lights -->
				<xsl:otherwise> 
					<xsl:text>A</xsl:text> 
				</xsl:otherwise>
			</xsl:choose>
			<xsl:text>&gt;</xsl:text>
		</xsl:if>		
	</xsl:template>

	<!--  COLORS  -->

	<xsl:template match="dimred">
		<xsl:text>&lt;CA&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="red">
		<xsl:text>&lt;CB&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="brightred">
		<xsl:text>&lt;CC&gt;</xsl:text>
	</xsl:template>	

	<xsl:template match="orange">
		<xsl:text>&lt;CD&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="brightorange">
		<xsl:text>&lt;CE&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="amber">
		<xsl:text>&lt;CE&gt;</xsl:text>
	</xsl:template>	
	
	<xsl:template match="dimyellow">
		<xsl:text>&lt;CF&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="brown">
		<xsl:text>&lt;CF&gt;</xsl:text>
	</xsl:template>
		
	<xsl:template match="yellow">
		<xsl:text>&lt;CG&gt;</xsl:text>
	</xsl:template>		
	
	<xsl:template match="brightyellow">
		<xsl:text>&lt;CH&gt;</xsl:text>
	</xsl:template>	
	
	<xsl:template match="green">
		<xsl:text>&lt;CM&gt;</xsl:text>
	</xsl:template>

	<xsl:template match="dimlime">
		<xsl:text>&lt;CI&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="lime">
		<xsl:text>&lt;CJ&gt;</xsl:text>
	</xsl:template>
		
	<xsl:template match="brightlime">
		<xsl:text>&lt;CK&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="brightgreen">
		<xsl:text>&lt;CL&gt;</xsl:text>
	</xsl:template>			

	<xsl:template match="green">
		<xsl:text>&lt;CM&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="dimgreen">
		<xsl:text>&lt;CN&gt;</xsl:text>
	</xsl:template>

	<xsl:template match="rainbow1">
		<xsl:text>&lt;CP&gt;</xsl:text>
	</xsl:template>

	<xsl:template match="rainbow2">
		<xsl:text>&lt;CO&gt;</xsl:text>
	</xsl:template>

	<xsl:template match="mode">
		<xsl:value-of select="document('')//a:modeLookup/a:mode[@name=current()/@display]/@value"/>
		<xsl:if test="@size">
			<xsl:choose>
				<xsl:when test="@size = 'small'">
					<xsl:text>&lt;SJ&gt;</xsl:text>
				</xsl:when>
			</xsl:choose>
		</xsl:if>
	</xsl:template>	

	<xsl:template match="msg">
		<xsl:apply-templates/>
	</xsl:template>

	<xsl:template match="hardReset">
		<xsl:text>&lt;RST&gt;</xsl:text>
	</xsl:template>
	
	<xsl:template match="blank">
		<xsl:text>                </xsl:text>
	</xsl:template>
	
	<xsl:template match="softReset">
		<xsl:text>&lt;SA&gt;&lt;SI&gt;&lt;CP&gt;</xsl:text>
	</xsl:template>	

	<xsl:template name="timeFormat">
		<xsl:param name="format"/>
		<xsl:choose>
			<xsl:when test="$format = 'am-pm'">
				<xsl:text>0</xsl:text>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>1</xsl:text>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="setDateTime">
		<xsl:text>&lt;T</xsl:text>
		<xsl:call-template name="calendarDate">
			<xsl:with-param name="month" select="/signboard/setDateTime/calendarDate/@month"/>
			<xsl:with-param name="day" select="/signboard/setDateTime/calendarDate/@day"/>
			<xsl:with-param name="year" select="/signboard/setDateTime/calendarDate/@year"/>
		</xsl:call-template>
		<xsl:call-template name="dayOfWeek">
			<xsl:with-param name="dayOfWeek" select="/signboard/setDateTime/dayOfWeek/@day"/>
		</xsl:call-template>
		<xsl:call-template name="timeOfDay">
			<xsl:with-param name="hour" select="/signboard/setDateTime/timeOfDay/@hour"/>
			<xsl:with-param name="minute" select="/signboard/setDateTime/timeOfDay/@minute"/>
			<xsl:with-param name="second" select="/signboard/setDateTime/timeOfDay/@second"/>
		</xsl:call-template>
		<xsl:call-template name="timeFormat">
			<xsl:with-param name="format" select="/signboard/setDateTime/timeFormat/@format"/>
		</xsl:call-template>
		<xsl:text>&gt;</xsl:text>
	</xsl:template>

	<xsl:template name="timeOfDay">
		<xsl:param name="hour"/>
		<xsl:param name="minute"/>
		<xsl:param name="second"/>
		<xsl:call-template name="lpad">
			<xsl:with-param name="str" select="$hour"/>
			<xsl:with-param name="len" select="2"/>
			<xsl:with-param name="chr" select="0"/>
		</xsl:call-template>
		<xsl:call-template name="lpad">
			<xsl:with-param name="str" select="$minute"/>
			<xsl:with-param name="len" select="2"/>
			<xsl:with-param name="chr" select="0"/>
		</xsl:call-template>
        <xsl:call-template name="lpad">
            <xsl:with-param name="str" select="$second"/>
            <xsl:with-param name="len" select="2"/>
            <xsl:with-param name="chr" select="0"/>
        </xsl:call-template>
	</xsl:template>

	<xsl:template name="dayOfWeek">
		<xsl:param name="dayOfWeek"/>
		<xsl:value-of select="document('')//a:dayLookup/a:schedule[@name=$dayOfWeek]/@value"/>
	</xsl:template>

	<xsl:template name="calendarDate">
		<xsl:param name="month"/>
		<xsl:param name="day"/>
		<xsl:param name="year"/>
		<xsl:value-of select="$century"/>
		<xsl:call-template name="lpad">
			<xsl:with-param name="str" select="$year"/>
			<xsl:with-param name="len" select="2"/>
			<xsl:with-param name="chr" select="0"/>
		</xsl:call-template>
		<xsl:call-template name="lpad">
			<xsl:with-param name="str" select="$month"/>
			<xsl:with-param name="len" select="2"/>
			<xsl:with-param name="chr" select="0"/>
		</xsl:call-template>
		<xsl:call-template name="lpad">
			<xsl:with-param name="str" select="$day"/>
			<xsl:with-param name="len" select="2"/>
			<xsl:with-param name="chr" select="0"/>
		</xsl:call-template>
	</xsl:template>

	<!-- Display Time -->

	<xsl:template match="time">
		<xsl:text>&lt;FT&gt;</xsl:text>
	</xsl:template>

	<xsl:template match="date">
		<xsl:text>&lt;FD&gt;</xsl:text>
	</xsl:template>

	<xsl:template match="datetime">
		<xsl:text>&lt;FD&gt; &lt;FT&gt;</xsl:text>
	</xsl:template>

	<xsl:template match="beep">
		<xsl:text>&lt;M</xsl:text>
		<xsl:choose>
			<xsl:when test="@type">
				<xsl:choose>
					<xsl:when test="@type='long'">
						<xsl:text>D</xsl:text>
					</xsl:when>
					<xsl:otherwise>
						<xsl:text>C</xsl:text>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>B</xsl:text>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:text>&gt;</xsl:text>
	</xsl:template>

	<!-- pad $str on left with $chr to size $len -->
	<xsl:template name="lpad">
		<xsl:param name="str"/>
		<xsl:param name="len"/>
		<xsl:param name="chr"/>
		<xsl:choose>
			<xsl:when test="string-length($str) &lt; $len">
				<xsl:call-template name="lpad">
					<xsl:with-param name="str" select="concat($chr, $str)"/>
					<xsl:with-param name="len" select="$len"/>
					<xsl:with-param name="chr" select="$chr"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$str"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- Color Lookup Table -->

	<a:modeLookup>
	    <a:mode name="rotate" value="&lt;SA&gt;"/>
	    <a:mode name="hold" value="&lt;FQ&gt;"/>
	    <a:mode name="flash" value="&lt;FQ&gt;&lt;SE&gt;"/>
	    <a:mode name="rollUp" value="&lt;FI&gt;"/>
	    <a:mode name="rollDown" value="&lt;FJ&gt;"/>
	    <a:mode name="rollLeft" value="&lt;FF&gt;"/>
	    <a:mode name="rollRight" value="&lt;FG&gt;"/>
	    <a:mode name="wipeUp" value="&lt;FI&gt;"/>
	    <a:mode name="wipeDown" value="&lt;FL&gt;"/>
	    <a:mode name="wipeLeft" value="&lt;FF&gt;"/>
	    <a:mode name="wipeRight" value="&lt;FG&gt;"/>
	    <a:mode name="scroll" value="&lt;FS&gt;"/>
	    <a:mode name="automode" value="&lt;FA&gt;"/>
	    <a:mode name="rollIn" value="&lt;FB&gt;"/>
	    <a:mode name="rollOut" value="&lt;FH&gt;"/>
	    <a:mode name="wipeIn" value="&lt;FH&gt;"/>
	    <a:mode name="wipeOut" value="&lt;FC&gt;"/>
	    <a:mode name="compressedRotate" value="&lt;SA&gt;&lt;SJ&gt;"/> <!-- Small Font -->
	    <a:mode name="cycleColors" value="&lt;FE&gt;"/>
	    <a:mode name="welcome" value="&lt;FW&gt;"/>
	    <a:mode name="thankYou" value="&lt;FV&gt;"/>
	</a:modeLookup>

	<!-- Day of Week Lookup Table -->

	<a:dayLookup>
		<a:schedule name="Sunday" value="0"/>
		<a:schedule name="Monday" value="1"/>
		<a:schedule name="Tuesday" value="2"/>
		<a:schedule name="Wednesday" value="3"/>
		<a:schedule name="Thursday" value="4"/>
		<a:schedule name="Friday" value="5"/>
		<a:schedule name="Saturday" value="6"/>
	</a:dayLookup>

	<!-- Hour/Min Lookup Table -->

	<a:timeLookup>
		<a:time name="00:00" value="00"/>
		<a:time name="00:10" value="01"/>
		<a:time name="00:20" value="02"/>
		<a:time name="00:30" value="03"/>
		<a:time name="00:40" value="04"/>
		<a:time name="00:50" value="05"/>
		<a:time name="01:00" value="06"/>
		<a:time name="01:10" value="07"/>
		<a:time name="01:20" value="08"/>
		<a:time name="01:30" value="09"/>
		<a:time name="01:40" value="0A"/>
		<a:time name="01:50" value="0B"/>
		<a:time name="02:00" value="0C"/>
		<a:time name="02:10" value="0D"/>
		<a:time name="02:20" value="0E"/>
		<a:time name="02:30" value="0F"/>
		<a:time name="02:40" value="10"/>
		<a:time name="02:50" value="11"/>
		<a:time name="03:00" value="12"/>
		<a:time name="03:10" value="13"/>
		<a:time name="03:20" value="14"/>
		<a:time name="03:30" value="15"/>
		<a:time name="03:40" value="16"/>
		<a:time name="03:50" value="17"/>
		<a:time name="04:00" value="18"/>
		<a:time name="04:10" value="19"/>
		<a:time name="04:20" value="1A"/>
		<a:time name="04:30" value="1B"/>
		<a:time name="04:40" value="1C"/>
		<a:time name="04:50" value="1D"/>
		<a:time name="05:00" value="1E"/>
		<a:time name="05:10" value="1F"/>
		<a:time name="05:20" value="20"/>
		<a:time name="05:30" value="21"/>
		<a:time name="05:40" value="22"/>
		<a:time name="05:50" value="23"/>
		<a:time name="06:00" value="24"/>
		<a:time name="06:10" value="25"/>
		<a:time name="06:20" value="26"/>
		<a:time name="06:30" value="27"/>
		<a:time name="06:40" value="28"/>
		<a:time name="06:50" value="29"/>
		<a:time name="07:00" value="2A"/>
		<a:time name="07:10" value="2B"/>
		<a:time name="07:20" value="2C"/>
		<a:time name="07:30" value="2D"/>
		<a:time name="07:40" value="2E"/>
		<a:time name="07:50" value="2F"/>
		<a:time name="08:00" value="30"/>
		<a:time name="08:10" value="31"/>
		<a:time name="08:20" value="32"/>
		<a:time name="08:30" value="33"/>
		<a:time name="08:40" value="34"/>
		<a:time name="08:50" value="35"/>
		<a:time name="09:00" value="36"/>
		<a:time name="09:10" value="37"/>
		<a:time name="09:20" value="38"/>
		<a:time name="09:30" value="39"/>
		<a:time name="09:40" value="3A"/>
		<a:time name="09:50" value="3B"/>
		<a:time name="10:00" value="3C"/>
		<a:time name="10:10" value="3D"/>
		<a:time name="10:20" value="3E"/>
		<a:time name="10:30" value="3F"/>
		<a:time name="10:40" value="40"/>
		<a:time name="10:50" value="41"/>
		<a:time name="11:00" value="42"/>
		<a:time name="11:10" value="43"/>
		<a:time name="11:20" value="44"/>
		<a:time name="11:30" value="45"/>
		<a:time name="11:40" value="46"/>
		<a:time name="11:50" value="47"/>
		<a:time name="12:00" value="48"/>
		<a:time name="12:10" value="49"/>
		<a:time name="12:20" value="4A"/>
		<a:time name="12:30" value="4B"/>
		<a:time name="12:40" value="4C"/>
		<a:time name="12:50" value="4D"/>
		<a:time name="13:00" value="4E"/>
		<a:time name="13:10" value="4F"/>
		<a:time name="13:20" value="50"/>
		<a:time name="13:30" value="51"/>
		<a:time name="13:40" value="52"/>
		<a:time name="13:50" value="53"/>
		<a:time name="14:00" value="54"/>
		<a:time name="14:10" value="55"/>
		<a:time name="14:20" value="56"/>
		<a:time name="14:30" value="57"/>
		<a:time name="14:40" value="58"/>
		<a:time name="14:50" value="59"/>
		<a:time name="15:00" value="5A"/>
		<a:time name="15:10" value="5B"/>
		<a:time name="15:20" value="5C"/>
		<a:time name="15:30" value="5D"/>
		<a:time name="15:40" value="5E"/>
		<a:time name="15:50" value="5F"/>
		<a:time name="16:00" value="60"/>
		<a:time name="16:10" value="61"/>
		<a:time name="16:20" value="62"/>
		<a:time name="16:30" value="63"/>
		<a:time name="16:40" value="64"/>
		<a:time name="16:50" value="65"/>
		<a:time name="17:00" value="66"/>
		<a:time name="17:10" value="67"/>
		<a:time name="17:20" value="68"/>
		<a:time name="17:30" value="69"/>
		<a:time name="17:40" value="6A"/>
		<a:time name="17:50" value="6B"/>
		<a:time name="18:00" value="6C"/>
		<a:time name="18:10" value="6D"/>
		<a:time name="18:20" value="6E"/>
		<a:time name="18:30" value="6F"/>
		<a:time name="18:40" value="70"/>
		<a:time name="18:50" value="71"/>
		<a:time name="19:00" value="72"/>
		<a:time name="19:10" value="73"/>
		<a:time name="19:20" value="74"/>
		<a:time name="19:30" value="75"/>
		<a:time name="19:40" value="76"/>
		<a:time name="19:50" value="77"/>
		<a:time name="20:00" value="78"/>
		<a:time name="20:10" value="79"/>
		<a:time name="20:20" value="7A"/>
		<a:time name="20:30" value="7B"/>
		<a:time name="20:40" value="7C"/>
		<a:time name="20:50" value="7D"/>
		<a:time name="21:00" value="7E"/>
		<a:time name="21:10" value="7F"/>
		<a:time name="21:20" value="80"/>
		<a:time name="21:30" value="81"/>
		<a:time name="21:40" value="82"/>
		<a:time name="21:50" value="83"/>
		<a:time name="22:00" value="84"/>
		<a:time name="22:10" value="85"/>
		<a:time name="22:20" value="86"/>
		<a:time name="22:30" value="87"/>
		<a:time name="22:40" value="88"/>
		<a:time name="22:50" value="89"/>
		<a:time name="23:00" value="8A"/>
		<a:time name="23:10" value="8B"/>
		<a:time name="23:20" value="8C"/>
		<a:time name="23:30" value="8D"/>
		<a:time name="23:40" value="8E"/>
		<a:time name="23:50" value="8F"/>
	</a:timeLookup>
</xsl:stylesheet>