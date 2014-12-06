<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.1">
	<xsl:template match="/">
		<xsl:variable name="Agency" select="//AlertQueue/Agency"/>
		<xsl:variable name="BoxArea" select="//AlertQueue/BoxArea"/>
		<xsl:variable name="UnformattedBoxArea">
		  <xsl:call-template name="string-replace-all">
		    <xsl:with-param name="text" select="$BoxArea" />
		    <xsl:with-param name="replace" select="concat($Agency,'-')" />
		    <xsl:with-param name="by" select="''" />
		  </xsl:call-template>
		</xsl:variable>		
		<xsl:variable name="CityCode" select="//AlertQueue/CityCode"/>
		<xsl:variable name="Location">
		  <xsl:call-template name="string-replace-all">
		    <xsl:with-param name="text" select="//AlertQueue/Location" />
		    <xsl:with-param name="replace" select="concat(', ', $CityCode)" />
		    <xsl:with-param name="by" select="''" />
		  </xsl:call-template>
		</xsl:variable>		
		<AlertQueue>
			<BoxArea>
				<Formatted><xsl:value-of select="substring($UnformattedBoxArea, 1, 1)"/>-<xsl:value-of select="substring($UnformattedBoxArea, 2)"/></Formatted>
				<UnFormatted><xsl:value-of select="$UnformattedBoxArea" /></UnFormatted>
				<FullBox><xsl:value-of select="$BoxArea" /></FullBox>
			</BoxArea>
			<Station></Station>
			<Location><xsl:value-of select="$Location"/></Location>
		</AlertQueue>
	</xsl:template>
	<xsl:template name="string-replace-all">
	  <xsl:param name="text" />
	  <xsl:param name="replace" />
	  <xsl:param name="by" />
	  <xsl:choose>
	    <xsl:when test="contains($text, $replace)">
	      <xsl:value-of select="substring-before($text,$replace)" />
	      <xsl:value-of select="$by" />
	      <xsl:call-template name="string-replace-all">
	        <xsl:with-param name="text" select="substring-after($text,$replace)" />
	        <xsl:with-param name="replace" select="$replace" />
	        <xsl:with-param name="by" select="$by" />
	      </xsl:call-template>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$text" />
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:template>	
</xsl:stylesheet>			