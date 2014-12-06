<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:fhwm="http://fhwm.net/xsd/1.2/ICadDispatch"
	xmlns:ns1="http://niem.gov/niem/structures/2.0"
	xmlns:ns2="http://niem.gov/niem/domains/emergencyManagement/2.0"
	xmlns:ns3="http://niem.gov/niem/niem-core/2.0"
	xmlns:ns4="http://fhwm.net/xsd/ICadDispatch"
	xmlns:ns6="http://niem.gov/niem/domains/jxdm/4.0"
	xmlns:ns7="http://niem.gov/niem/ansi-nist/2.0"
	version="1.0">
	<xsl:template match="/">
		<xsl:variable name="EventNo" select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns3:ActivityIdentification/ns3:IdentificationID"/>
		<xsl:variable name="IncidentNo" select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallAugmentation/ns4:IncidentId/ns3:IdentificationID"/>
		<xsl:variable name="BoxArea" select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallResponseLocation/ns4:ServiceCallResponseLocationAugmentation/ns4:Firebox"/>
		<xsl:variable name="StationGrid" select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallResponseLocation/ns4:ServiceCallResponseLocationAugmentation/ns4:StationGrid"/>		
		<WatchmanAlerting>
			<IncidentNo><xsl:value-of select="$IncidentNo"/></IncidentNo>
			<EventNo><xsl:value-of select="$EventNo"/></EventNo>
			<Priority><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallAugmentation/ns4:CallPriorityText"/></Priority>
			<RadioTac><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallAugmentation/ns4:Staging/ns3:ContactRadioChannelText"/></RadioTac>
			<DispatchDate><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns6:ServiceCallDispatchedDate/ns3:DateTime"/></DispatchDate>
			<IncidentStatus>
				<CurrentStatus><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallAugmentation/ns4:CurrentStatus"/></CurrentStatus>
				<xsl:for-each select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns3:ActivityStatus">
					<xsl:if test="./ns3:StatusText = 'ENTRY'">
						<Entry><xsl:value-of select="./ns3:StatusDate/ns3:DateTime"/></Entry>
					</xsl:if>
					<xsl:if test="./ns3:StatusText = 'OPEN'">
						<Open><xsl:value-of select="./ns3:StatusDate/ns3:DateTime"/></Open>
					</xsl:if>
					<xsl:if test="./ns3:StatusText = 'DISPATCH'">
						<Dispatch><xsl:value-of select="./ns3:StatusDate/ns3:DateTime"/></Dispatch>
					</xsl:if>
					<xsl:if test="./ns3:StatusText = 'ENROUTE'">
						<Enroute><xsl:value-of select="./ns3:StatusDate/ns3:DateTime"/></Enroute>
					</xsl:if>
					<xsl:if test="./ns3:StatusText = 'ONSCENE'">
						<Onscene><xsl:value-of select="./ns3:StatusDate/ns3:DateTime"/></Onscene>
					</xsl:if>
					<xsl:if test="./ns3:StatusText = 'CLOSE'">
						<Close><xsl:value-of select="./ns3:StatusDate/ns3:DateTime"/></Close>
					</xsl:if>
				</xsl:for-each>
			</IncidentStatus>
			<CallType>
				<Nature><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns3:ActivityDescriptionText"/></Nature>
				<TypeCode><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallAugmentation/ns4:CallTypeText"/></TypeCode>
			</CallType>
			<IncidentLocation>
				<BoxArea><xsl:value-of select="substring($BoxArea,1,2)"/>-<xsl:value-of select="substring($BoxArea,3)"/></BoxArea>
				<Station><xsl:value-of select="$StationGrid"/></Station>
				<StationGrid><xsl:value-of select="$StationGrid"/></StationGrid>
				<MapGrid><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallResponseLocation/ns4:ServiceCallResponseLocationAugmentation/ns4:MapGrid"/></MapGrid>
				<Location>
					<xsl:for-each select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallResponseLocation/ns3:LocationAddress/ns3:StructuredAddress/ns3:LocationStreet/ns3:StreetName">
						<StreetName><xsl:value-of select="."/></StreetName>
					</xsl:for-each>
				</Location>
				<CrossStreet>
					<xsl:for-each select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallResponseLocation/ns4:ServiceCallResponseLocationAugmentation/ns4:LocationCrossStreet/ns3:StreetName">
						<StreetName><xsl:value-of select="."/></StreetName>
					</xsl:for-each>
				</CrossStreet>
				<GPSCoords>
					<GPSLatitude><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallResponseLocation/ns4:ServiceCallResponseLocationAugmentation/ns4:GPSLatitudeDecimal"/></GPSLatitude>
					<GPSLongitude><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallResponseLocation/ns4:ServiceCallResponseLocationAugmentation/ns4:GPSLongitudeDecimal"/></GPSLongitude>
				</GPSCoords>
			</IncidentLocation>
			<UnitDispatch>
                <UnitList><xsl:value-of select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns3:ActivityReasonText"/></UnitList>
				<xsl:for-each select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCallAssignedUnit">
					<xsl:variable name="__Unit" select="./ns3:OrganizationIdentification/ns3:IdentificationID"/>
					<Assigned><xsl:value-of select="$__Unit"/></Assigned>
				</xsl:for-each>
			</UnitDispatch>
			<IncidentComments>
				<xsl:for-each select="//fhwm:ICadDispatch/ns4:Payload/ns4:ServiceCall/ns4:ServiceCallAugmentation/ns4:Comment">
					<xsl:variable name="NoteTime" select="./ns4:CommentDateTime/ns3:DateTime"/>
					<xsl:variable name="NoteID" select="./ns3:SourceIDText"/>
					<Comment>
						<EntryTime><xsl:value-of select="./ns4:CommentDateTime/ns3:DateTime"/></EntryTime>
						<EntryFDID><xsl:value-of select="./ns6:ServiceCallOperator/ns3:PersonName/ns3:PersonNamePrefixText"/></EntryFDID>
						<EntryOperator><xsl:value-of select="./ns6:ServiceCallOperator/ns3:PersonName/ns3:PersonFullName"/></EntryOperator>
						<EntryID><xsl:value-of select="./ns3:SourceIDText"/></EntryID>
						<Text><xsl:value-of select="./ns3:CommentText"/></Text>
					</Comment>
				</xsl:for-each>
			</IncidentComments>
			<VoiceAlert>
				<TTSKeyId><xsl:value-of select="//fhwm:ICadDispatch/ns4:VoiceAlertData/ns4:VoiceAlertS3Uri"/></TTSKeyId>
			</VoiceAlert>
			<Metadata>
				<DispatchService><xsl:value-of select="//fhwm:ICadDispatch/ns4:ExchangeMetadata/ns4:DataSubmitterMetadata/ns3:OrganizationIdentification/ns3:IdentificationID"/></DispatchService>
				<Jurisdiction><xsl:value-of select="//fhwm:ICadDispatch/ns4:ExchangeMetadata/ns4:DataSubmitterMetadata/ns3:OrganizationName"/></Jurisdiction>
				<TransID><xsl:value-of select="//fhwm:ICadDispatch/ns4:ExchangeMetadata/ns4:TransactionMetadata/ns4:MetadataAugmentation/ns4:MessageSequenceNumber/ns3:IdentificationID" /></TransID>
				<TransTimestamp><xsl:value-of select="//fhwm:ICadDispatch/ns4:ExchangeMetadata/ns4:TransactionMetadata/ns4:MetadataAugmentation/ns4:SubmissionDateTime/ns3:DateTime" /></TransTimestamp>
			</Metadata>
		</WatchmanAlerting>
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
	<xsl:output indent="yes" method="xml" />
</xsl:stylesheet>
