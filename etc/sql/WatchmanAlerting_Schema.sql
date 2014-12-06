DROP TABLE IF EXISTS `Settings`;
CREATE TABLE `Settings` (
	`Name` VARCHAR(45) NOT NULL PRIMARY KEY,
	`Descr` VARCHAR(128) NOT NULL,
	`Category` VARCHAR(16) NOT NULL,
	`Setting` VARCHAR(255) NULL DEFAULT NULL,
	INDEX `Category` ( `Category` )
) ENGINE=MyISAM;

-- PGCo XSLT_PreProcessCommand => [ pcl6 -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=- %source_file ]
-- PGCo DefaultXSLT_Command => [ pdftotext -layout -nopgbrk -x 11 -y 22 -W 600 -H 724 %source_file - | pgfd-tibparse -units '%myunits' ]

INSERT INTO Settings VALUES
('DefaultStationID', 'WatchmanAlerting station identifier', 'system', ''),
('RelayProxyAddr', 'Default URL proxy address for WatchmanAlerting relay web service', 'system', 'http://soap.firehousewatchman.com'),
('RelayURI_Addr', 'Default URI schema address for WatchmanAlerting relay web service', 'system', 'Watchman/SOAP'),
('RelayHTTP_Timeout', 'Default HTTP timeout when waiting for response from relay web service', 'system', '10'),
('RegionCity', 'If applicable, city is used for GPS and mapping queries', 'system', ''),
('RegionCounty', 'If applicable, county is used for GPS and mapping queries', 'system', ''),
('RegionState', 'If applicable, state is used for GPS and mapping queries', 'system', ''),
('DefaultXSLT_Command', 'System command to use for processing incoming dispatch data', 'system', ''),
('DefaultXSL_Path', 'Default XSL stylesheet to use for processing incoming dispatch data', 'system', 'xsl/iCADDispatch_NIEM2.xsl'),
('XSLT_PreProcessCommand', 'If dispatch data requires custom pre-processing before running XSLT command enter pre-processing instruction here', 'system', ''),
('XSLT_PreProcessTimeout', 'Enforce system timeout (in seconds) when executing stream data pre-processing commands', 'system', '5'),
('DefaultTemplatePath', 'File path to system templates', 'system', 'templates/printout.tmpl'),
('LocationFormatting', 'Pattern modifiers used for applying formatting to location text', 'system', ''),
('UnitDispNarrativeInsert', 'Enable/Disable insertion of dispatched units into narrative according to timestamp position w/in narrative', 'system', ''),

('AlertQueueEnabled', 'Enable/Disable the WatchmanAlerting AlertQueue Active Incident Polling System', 'alertqueue', '0'),
('HostAddr', 'Host URL for AlertQueue polling', 'alertqueue', 'http://icad'),
('IncidentNoPrefix', 'Prefix identifier found in incident numbers', 'alertqueue', ''),
('EventNoPrefix', 'Prefix identifier found in event numbers', 'alertqueue', ''),
('PollInterval', 'Wait time in seconds between polling intervals', 'alertqueue', '5'),
('PollErrInterval', 'Wait time in seconds between polling intervals when an error occurs', 'alertqueue', '5'),
('HTTP_CookieSaveFile', 'File location to save HTTP cookies received from the AlertQueue polling server', 'alertqueue', ''),
('HTTP_CookieName', 'Name of HTTP cookie issued by AlertQueue polling server', 'alertqueue', 'PHPSESSID'),
('LoginPath', 'AlertQueue URL path for authentication', 'alertqueue', '/login.php'),
('LoginPathQueryString', 'If required, enter query string to be affixed to end of login URL address (not needed in most cases)', 'alertqueue', ''),
('LoginUsername', 'AlertQueue login username', 'alertqueue', ''),
('LoginPassword', 'AlertQueue login password', 'alertqueue', ''),
('LoginHTTP_Post', 'Default HTTP POST string for AlertQueue login', 'alertqueue', 'login_button_x=1&rememberme=1&r=poll&user_name=%username&password=%password'),
('LoginFailureRegex', 'Regular expression comparison string signifying a failed login', 'alertqueue', 'ERROR'),
('PollingPath', 'AlertQueue URL path for incident polling', 'alertqueue', '/poll.php'),
('PollingHTTP_Post', 'Default HTTP POST string for AlertQueue incident polling', 'alertqueue', 'AlertQueue=1&QueryInterval=%queryinterval%'),
('HTTP_AcceptStatus', 'Regular expression formatted string representing permitted HTTP accepted status', 'alertqueue', '200 OK'),
('HTTP_Agent', 'If required, use specified HTTP agent for browser identification', 'alertqueue', ''),
('HTTP_Timeout', 'Default timeout when waiting for response from polling server', 'alertqueue', ''),
('SleepTime', 'Disable AlertQueue polling between specified times (i.e. 0600-1500)', 'alertqueue', ''),
('SleepExceptionDayOfWeek', 'If system sleep is enable, skip sleeping on specified days of the week', 'alertqueue', ''),
('SleepExceptionDate', 'If system sleep is enabled, skip sleep cycle on specified dates (i.e. 12/25, 07/04)', 'alertqueue', ''),
('PollingLookbackTime', 'Poll for active incidents created or updated within the specified timeframe (in seconds)', 'alertqueue', '300'),
('AlertQueueLED_DisplayReset', 'Length of time (in seconds) to wait before resetting LED signboards to default display', 'alertqueue', '180'),
('ActiveIncAudioPreamble', 'Audio preamble to use for new priority active incidents', 'alertqueue', 'audio/AlertTones/PendingPriCall-wAlertTone.wav'),
('LocalIncAudioPreamble', 'Audio preamble to use for new first due active incidents', 'alertqueue', 'audio/AlertTones/PendingFirstDueCallWithAlertTone.wav'),
('ActiveIncUpdateAudioPreamble', 'Audio preamble to use for updates and cancellations to current active incidents', 'alertqueue', 'audio/AlertTones/IncidentUpdateWithAlertTone.wav'),
('ActiveIncAudioPreamble_FirstDue', 'Audio preamble to use for new (non-priority) first due active incidents', 'alertqueue', 'audio/AlertTones/PendingPriCallFirstDue-wAlertTone.wav'),
('BoxAreaSubstr', 'Box area pattern filter for specifying custom box area syntax (X|Y). For example, 4| applied to 231-25 results in box area 25',  'alertqueue', ''),
('SilentSleepMode', 'If daily sleep mode is enabled, continue polling in silent alert mode during sleep periods', 'alertqueue', ''),
('FirstDueIgnoreTypes', 'Regular expression formatted list of first due call types that should be ignored during AlertQueue active incident polling', 'alertqueue', ''),
('RegexLocationPrefix', 'If the location needs to be reformatted in any way, enter regular expression format', 'alertqueue', ''),
('LogHTTP_Failures', 'Enable/Disable logging of HTTP request failures', 'alertqueue', ''),
('AlertQueueDebug', 'Enable/Disable debug-style logging of AlertQueue related events ', 'alertqueue', ''),
('FirstDueBoxListing', 'List of box area patterns that fall within the first due response criteria', 'alertqueue', ''),
('AlertQueueFormatting_XSL', 'Use the specified XSL file to apply any formatting rules to AlertQueue incidents', 'alertqueue', 'xsl/AlertQueue_Formatting.xsl'),

('INET_Enabled', 'Enable/Disable the WatchmanAlerting network server for incoming data', 'inet', '1'),
('LocalAddr', 'Network server local IP address or interface', 'inet', '172.XX.XX.XX'),
('LocalPort', 'Network server local port', 'inet', '8888'),
('Protocol', 'Network server default connection protocol', 'inet', 'tcp'),
('Blocking', 'Enable/Disable network server connection blocking', 'inet', '0'),
('ClientType', 'Type of connection WatchmanAlerting network server will be receiving data from', 'inet', 'ICAD-XML'), -- ENUM VALS: ICAD-XML|LPR-STREAM

('TotalSerialLED_Device', 'Total number of serial LED signboard devices connected to WatchmanAlerting main control unit', 'serial', '1'),
('LED_DefaultXSL', 'Default XSL stylesheet to use for generating LED signboard commands', 'serial', 'xsl/alphasign.xsl'),
('LED_DefaultProtocol', 'If applicable, specify serial protocol to be used when converting LED display message to the native signboard language (depreciated)', 'serial', ''),
('LED_DefaultType', 'Communication type for LED display [tty|net]', 'serial', ''),
('LED_DefaultAddr', 'Network address (IPv4) for communicating with network LED display (unused w/serial display type)', 'serial', ''),
('LED_DefaultNetProto', 'Network communications protocol for communicating with network LED display [tcp|udp] (unused w/serial display type)', 'serial', ''),
('LED_DefaultLocalUDP_Port', 'Local UDP broadcast port (only used w/udp broadcast display type)', 'serial', '5051'),
('LED_DefaultTTY_Port', 'Default port address to use for communicating with LED signboards [TTY displays: /dev/ttyX] [Network displays: [8080]', 'serial', ''),
('LED_DefaultPortSpeed', 'Default baud rate speed setting for serial port (unused with network displays)', 'serial', '9600'),
('LED_DefaultParity', 'Default parity setting for serial port (unused with network displays)', 'serial', 'even'),
('LED_DefaultDatabits', 'Default databits setting for serial port (unused with network displays)', 'serial', '7'),
('LED_DefaultStopbits', 'Default stopbits setting for serial port (unused with network displays)', 'serial', ''),
('LED_Dev1XSL', 'LED Device #1 XSL stylesheet to use for generating LED signboard commands', 'serial', ''),
('LED_Dev1Protocol', 'If applicable, specify serial protocol to be used when converting Device #1 LED display message to the native signboard language', 'serial', ''),
('LED_Dev1Type', 'Communication type for LED display X', 'serial', ''),
('LED_Dev1Addr', 'Network address (IPv4) for communicating with network LED display (unused w/serial display type)', 'serial', ''),
('LED_Dev1NetProto', 'Communication type for LED display X', 'serial', ''),
('LED_Dev1TTY_Port', 'Default port address to use for communicating with LED signboards [/dev/ttyX|8080]', 'serial', ''),
('LED_Dev1PortSpeed', 'LED Device #1 baud rate speed setting for serial port', 'serial', ''),
('LED_Dev1Parity', 'LED Device #1 parity setting for serial port', 'serial', ''),
('LED_Dev1Databits', 'LED Device #1 databits setting for serial port', 'serial', ''),
('LED_Dev1Stopbits', 'LED Device #1 stopbits setting for serial port', 'serial', ''),
('Netcat_Params', 'Netcat params', 'serial', ''),

('LED_DisplayEnabled', 'Enable/Disable LED signboards during incident alerting', 'ledalerting', '1'),
('LED_DefaultDisplay', 'Default display preference for LED signs', 'ledalerting', 'time'), -- Value can be either text-based message, 'date' for current date, 'time' for current time or 'datetime' for current date/time
('LED_DateFormat', 'Default format for displaying current date on LED signs', 'ledalerting', 'MM.DD.YY'), -- [ MM/DD/YY|DD/MM/YY|MM-DD-YY|DD-MM-YY|MM.DD.YY|DD.MM.YY|MM.DD.YY|DD.MM.YY|MM.DD, YYYY|day ]
('LED_TimeFormat', 'Default format for displaying time of day on LED signs', 'ledalerting', '24-hour'), -- [ 24-hour|am-pm(default)]
('LED_DefaultDisplayColor', 'Default LED display color to use if call type or call group display color is not set', 'ledalerting', 'red'),
('LED_PreambleDisplayMode', 'Default mode for displaying initial incident alerting message on LED signboards', 'ledalerting', 'flash'),
('LED_PreambleDisplaySpeed', 'Default message display speed for showing initial incident alerting message on LED signboards', 'ledalerting', ''),
('LED_StandardDisplayMode', 'Default mode for displaying incident data on LED signboards', 'ledalerting', 'rotate'),
('LED_StandardDisplaySpeed', 'Default message display speed for showing incident data on LED signboards', 'ledalerting', ''),
('LED_PreambleLength', 'Length of time (in seconds) for initial incident alerting message to be displayed on LED signboards', 'ledalerting', '3'),
('LED_PreambleMessage', 'Preample (flashing) message format for displaying incident alerting information on LED displays', 'ledalerting', '[%BoxArea] %CallNature' ),
('LED_ScrollingMessage', 'Scrolling message format for displaying incident alerting information on LED displays', 'ledalerting', '%UnitAssign - %Location %CrossStreet - %CallNature [%BoxArea] %UnitList' ),
('LED_MessageResetTime', 'Length of time (in seconds) to wait before resetting LED signboard to default display', 'ledalerting', '180'),
('LED_PriorityDisplayColor', 'LED display color to use for priority incidents', 'ledalerting', 'rainbow1'),
('LED_PriorityPreambleDisplay', 'Initial alert message to display on LED signboards for priority incidents', 'ledalerting', '--FIRST DUE JOB--'),
('LED_DisplayNarrative', 'Whether to display incident narrative on LED signboards', 'ledalerting', '0'),
('LED_DisplayNarrativeLength', 'If incident narrative display is enabled, limit narrative display character length', 'ledalerting', ''),
('LED_BeepEnabled', 'Enable/Disable audible signal beep on capable LED signboards', 'ledalerting', '1'),
('LED_BeepType', 'If enabled, specify type of audible signal beep to use with capable LED signboards', 'ledalerting', 'long'),
('LED_SkipPreambleDisplay', 'Skip the LED preamble message', 'ledalerting', ''),
('LED_DefaultTextPage', 'Default LED text page identifier to use for sending text files to signboard', 'ledalerting', ''),
('LED_DefaultTextSize', 'Set LED memory size for text message files', 'ledalerting', ''),

('NetAudioEnabled', 'Enable/Disable audible alerting during incident alerting', 'audioalerting', '1'),
('NetAudioPriorityPort', 'RTP destination port to use for sending network audio to remote alert zones during periods of incident alerting (reserved for future use)', 'audioalerting', '4446'),
('NetAudioDefaultPort', 'RTP destination port to use for sending network audio to remote alert zones during idle, non-alerting periods (reserved for future use)', 'audioalerting', '4444'),
('NetAudioTTL', 'Default TTL for streaming RTP across multicast VLANs (reserved for future use)', 'audioalerting', '10'),
('NetAudioSleepBetween', 'Amount of time (microsecs) to sleep in between audio clips', 'audioalerting', ''),
('PriorityAudioPreamble', 'Audible preamble to use for priority incidents (will use default audio preamble if not set)', 'audioalerting', ''),
('PriorityAudioPostamble', 'Audible postamble to use for priority incidents', 'audioalerting', ''),
('DefaultAudioPreamble', 'Default audio preamble to use for preceeding any audible alerting announcements', 'audioalerting', 'audio/AlertTones/FastBeep-3sec.wav'),
('DefaultAudioPostamble', 'Default audio postamble to use following any audible alerting announcements (used if no audio postamble has been set for incident call group)', 'audioalerting', ''),
('PriorityPort', 'RTP destination port to use for sending network audio to remote alert zones during periods of incident alerting', 'audioalerting', '4446'),
('PrioritySkipUnitAnnounce', 'Enable/Disable unit announcements for first-due priority incidents', 'audioalerting', '1'),
('PrioritySkipTypeAnnounce', 'Enable/Disable call type announcements for first-due priority incidents', 'audioalerting', '1'),
('PrioritySkipVoiceAlert', 'Enable/Disable VoiceAlert TTS announcements for first-due priority incidents', 'audioalerting', ''),
('PrioritySkipPreAmble', 'Do not use default audio preamble for first-due priority incidents (if priority preamble is not set)', 'audioalerting', '1'),
('PrioritySkipPostAmble', 'Do not use default audio postamble for first-due priority incidents (if priority postamble is not set)', 'audioalerting', '1'),

('TTS_Enabled', 'Enable/Disable WatchmanAlerting VoiceAlert Text-to-Speech audio announcements', 'voicealert', ''),
('TTS_S3_Bucket', 'Location of VoiceAlert TTS storage (reserved for future use)', 'voicealert', ''),
('TTS_S3_AccessKey', 'Access key for accessing S3 storage repository (reserved for future use)', 'voicealert', ''),
('TTS_S3_SecretKey', 'Secret Access Key', 'voicealert (reserved for future use)', ''),
('TTS_URI', 'URI for checking TTS converstion status', 'voicealert', 'https://tts.neospeech.com/rest_1_1.php?method=GetConversionStatus&email=jeff.bruns@firehouseautomation.com&accountId=ef5674a4c1&conversionNumber=%conversionid'),
('TTS_URI_Timeout', 'HTTP timeout for accessing voicealert audio content', 'voicealert', '5'),

('SMS_Enabled', 'Enable/Disable system-wide SMS incident notifications', 'sms', ''),
('SMS_RelayProxyAddr', 'URL proxy address for WatchmanAlerting SMS web service (if not set, system uses default relay proxy address under system settings)', 'sms', 'http://soap.firehousewatchman.com'),
('SMS_RelayURI_Addr', 'URI schema address for WatchmanAlerting SMS web service (if not set, system uses default relay URI address under system settings)', 'sms', 'Watchman/SOAP'),
('SMS_IncludeNarrative', 'Whether to include incident narrative in SMS notifications', 'sms', ''),
('SMS_NarrativeLength', 'If enabled, limit incident narrative character length in SMS notification messages', 'sms', ''),
('SMS_HTTP_Timeout', 'HTTP timeout when waiting for response from SMS Notification server', 'sms', ''),

('RSS_Enabled', 'Enable/Disable RSS feed postings & updates', 'rss', ''),
('RSS_RelayProxyAddr', 'URL proxy address for WatchmanAlerting RSS web service (if not set, system uses default relay proxy address under system settings)', 'rss', ''),
('RSS_RelayURI_Addr', 'URI schema address for WatchmanAlerting RSS web service (if not set, system uses default relay URI address under system settings)', 'rss', ''),
('RSS_HTTP_Timeout', 'HTTP timeout when waiting for response from RSS Notification server (if not set, system uses default relay timeout under system settings)', 'rss', ''),

('HostSyncEnabled', 'Enable/Disable AlertQueue HostSync service', 'hostsync', ''),
('HostSyncProxyAddr', 'HostSync URL proxy address for connecting to the AlertQueue HostSync server', 'hostsync', ''),
('HostSyncURI_Addr', 'URI schema address for connecting to the AlertQueue HostSync server', 'hostsync', ''),
('HostSyncHTTP_Timeout', 'Default HTTP timeout when waiting for a server response', 'hostsync', ''),
('HostSyncDB_Driver', 'HostSync client database driver', 'hostsync', ''),
('HostSyncDB_Name', 'HostSync client database name', 'hostsync', ''),
('HostSyncDB_User', 'HostSync database username', 'hostsync', ''),
('HostSyncDB_Pass', 'HostSync database password', 'hostsync', ''),
('HostSyncDB_Port', 'HostSync database connection port', 'hostsync', '3306'),
('HostSyncInterval', 'Time to wait between HostSync connection intervals', 'hostsync', ''),

('RemotePrintingEnabled', 'Enable/Disable incident printing to remote fire station printer(s)', 'printing', ''),
('PrintBin', 'System command for executing remote print jobs', 'printing', ''),
('TotalPrintDevices', 'Total number of remote printers to send incident print jobs', 'printing', ''),
('PrintPassThru', 'Enable/Disable printing of unmodified dispatch data rather than dynamic construction of incident printout', 'printing', ''),
('PrintTimeout', 'Default wait timeout when sending print job to remote print device', 'printing', '5'),
('Printer_DefaultHostAddr', 'Default host address of remote printer', 'printing', ''),
('Printer_DefaultName', 'Default name of remote printer (not needed if Host Address is set)', 'printing', ''),
('Printer_DefaultAddr', 'Default IP address or URL of remote printer (not needed if Host Address is set)', 'printing', ''),
('Printer_DefaultPort', 'TCP port to use when sending print jobs to default printer', 'printing', ''),
('Printer_DefaultMethod', 'Print method to use when sending print jobs to default printer (defaults to LPR)', 'printing', ''), -- LPR|JETDIRECT|IPP

('IO_Enabled', 'Enable/Disable device input/output triggers', 'io', ''),
('IO_SlotDO_BinPath', 'Path to IO binary for writing to discrete outputs on onboard LinPAC Slot IO modules', 'io', '/usr/bin/setexdo'),
('IO_SlotAO_BinPath', 'Path to IO binary for writing to analog outputs on onboard LinPAC IO Slot modules', 'io', '/usr/bin/setexao'),

('RF_Alert_Enabled', 'Enable/Disable RF two-tone alerting', 'rfalert', ''),
('Poll_Interval', 'Polling interval time (default 500ms)', 'rfalert', '500'),
('ActiveWait', 'Time to wait before resuming polling after receiving tone activation (in ms)', 'rfalert', '1000'),
('RF_LEDResetTime', 'Time before resetting LED displays to default message', 'rfalert', '180'),
('RF_LED_Enabled', 'Enable/disable led displays during alerting', 'rfalert', '');

DROP TABLE IF EXISTS `LED_Displays`;
CREATE TABLE `LED_Displays` (
	`DisplayId` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`DisplayType` VARCHAR(20) NOT NULL DEFAULT 'BETABRITE',
	`DisplayName` VARCHAR(100) NOT NULL,
	`Protocol` VARCHAR(3) NOT NULL, -- Communications Proto: tcp|udp|tty
	`Address` VARCHAR(30) NOT NULL, -- TCP/UDP Address: 10.100.1.X:1234 | TTY Address: /dev/ttyS0:9600,8N1
 	`DefaultMsg` VARCHAR(255) NULL DEFAULT NULL,
 	`TimeFormat` VARCHAR(10) NOT NULL DEFAULT '24-hour', -- Enum: 24-hour|am-pm
 	`Stylesheet` VARCHAR(200) NOT NULL DEFAULT 'xsl/alphasign.xsl'
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `SystemProperties`;
CREATE TABLE `SystemProperties` (
	`PropName` VARCHAR(45) NOT NULL PRIMARY KEY,
	`PropValue` VARCHAR(64) NULL DEFAULT NULL
) ENGINE=MyISAM;

INSERT INTO SystemProperties VALUES
( 'SystemVersion', '5.1026' ),
( 'SchemaVersion', '5.1021' );

DROP TABLE IF EXISTS `iCADAlertTrans`;
CREATE TABLE iCADAlertTrans (
	`LocalTransID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`iCADTransID` INT NOT NULL,
	`ReceivedTime` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`SentTime` DATETIME NOT NULL,
	`StreamData` TEXT
) ENGINE=MyISAM AUTO_INCREMENT=1000;

DROP TABLE IF EXISTS `AlertZones`; -- Physical Alert Zones
CREATE TABLE AlertZones (
	`ZoneID` VARCHAR(12) NOT NULL PRIMARY KEY,
	`ZoneName` VARCHAR(64) NOT NULL,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`HostAddr` VARCHAR(16) NOT NULL,
	`iFaceHostAddr` VARCHAR(16) NOT NULL,
	`iFaceTcpControlPort` VARCHAR(5) NOT NULL,
	`iFaceUdpControlPort` VARCHAR(5) NOT NULL,
	`iFaceSerialPort` VARCHAR(5) NOT NULL,
	`IoResetAddr` VARCHAR(5) NOT NULL DEFAULT 1,
	`DefaultVolLevel` VARCHAR(3) NOT NULL,
	`AlertVolLevel` VARCHAR(3) NOT NULL,
	`TimeoutMins` INT NOT NULL DEFAULT 3,
	`AutoUpdate` TINYINT(1) NOT NULL DEFAULT 0,
	`SilentWatch` TINYINT(1) NOT NULL DEFAULT 0,
	`SilentStartTime` VARCHAR(4) NOT NULL,
	`SilentEndTime` VARCHAR(4) NOT NULL,
	`SilentVolLevel` VARCHAR(3) NOT NULL,
	`TroubleTimeout` INT NOT NULL DEFAULT 3,
	`CurrVolLevel` VARCHAR(3) NOT NULL DEFAULT 5,
	`CurrStreamNo` VARCHAR(12) NULL DEFAULT NULL,
	`CurrUrl` VARCHAR(255) NULL DEFAULT NULL,
	`iFaceAlarm` TINYINT(1) NOT NULL DEFAULT 0,
	`iFaceError` INT NOT NULL DEFAULT 0,
	`iFaceUptime` INT NOT NULL DEFAULT 0,
	INDEX HostAddr ( `HostAddr` ),
	INDEX iFaceHostAddr ( `iFaceHostAddr` )
) ENGINE=MyISAM AUTO_INCREMENT=100;

INSERT INTO AlertZones ( `ZoneID`, `ZoneName`, `HostAddr`, `iFaceHostAddr`, `DefaultVolLevel`, `AlertVolLevel`, `TimeoutMins` ) VALUES ( 'DEFAULT-1', 'STATION COMMON', '', '10.100.1.3', '0', '75', 3);

DROP TABLE IF EXISTS `AlertGroups`; -- Static unit alert groups
CREATE TABLE AlertGroups (
	`GroupID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`GroupAddr` VARCHAR(16) NOT NULL,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`GroupName` VARCHAR(24) NOT NULL,
	`GroupType` TINYINT(1) NOT NULL DEFAULT 0, -- 0=DEFAULT; 1=UNIT/TONE
	`UnitID` VARCHAR(12) NULL DEFAULT NULL,
	`ToneID` INT NULL DEFAULT NULL,
	`AlertQueue_PriAlert` TINYINT(1) NOT NULL DEFAULT 0, -- 1=>Enabled Priority AlertQueue Alerts 0=>Disable AlertQueue Priority Alerts
	`AlertQueue_LocalAlert` TINYINT(1) NOT NULL DEFAULT 0, -- 1=>Enabled First Due AlertQueue Alerts 0=>Disable First Due Priority Alerts
	`AlertQueue_Filter` VARCHAR(255) NULL DEFAULT NULL, -- Box area filter for AlertQueue alerts
	`AudioPreamble` VARCHAR(255) NULL DEFAULT NULL, -- Preamble alert tone, otherwise will use DefaultAutioPreamble in audioalerting settings
	`ToneID` INT NULL DEFAULT NULL, -- Regexp filter for association w/RF tone channels
	UNIQUE KEY `GroupAddr` ( `GroupAddr` )
	INDEX `GroupType` ( `GroupType`, `UnitId` )
) ENGINE=MyISAM AUTO_INCREMENT=1;

-- Generic broadcast alert groups for non-alert zone alerting
INSERT INTO AlertGroups ( `GroupAddr`, `GroupName`, `GroupType` )VALUES ( '239.255.0.5', 'DEFAULT', 0 );

DROP TABLE IF EXISTS `AlertGroupMember`; -- Dynamic alert zone group membership - Used for displaying groups on zone interface
CREATE TABLE AlertGroupMember (
	`MemberID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	`GroupAddr` VARCHAR(16) NOT NULL,
	`ZoneID` VARCHAR(12) NOT NULL,
	KEY `GroupAddr` ( `GroupAddr` ),
	KEY `ZoneID` ( `ZoneID` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `IO_Device`;
CREATE TABLE `IO_Device` (
	`TriggerID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`EventType` TINYINT(1) NOT NULL DEFAULT 0, -- (0) Generic Alert Triggered Event (1) Unit Alert Triggered Event (2) Tone Alert Triggered Event
	`UnitID` VARCHAR(12) NULL DEFAULT NULL,
	`ToneID` INT NULL DEFAULT NULL,
	`TriggerType` VARCHAR(10) NOT NULL DEFAULT 'MB_TCP', -- (1) MB_TCP => ModbusTCP Event (2) MB_RTU => ModbusRTU IO Event (3) MB_RTUIP => ModbusRTU/IP Event (4) TCPIP => TCP/IP Command Event (5) SLOT_IO => LinPAC Slot IO
	`DeviceAddr` VARCHAR(32) NOT NULL, -- (1) MB_TCP => IPv4 Addr (2) MB_RTU => TTY Addr (3) SLOT_IO => Slot No (1-x)
	`DevicePort` VARCHAR(5) NULL DEFAULT NULL, -- (1) MbusTCP => IPv4 Remote Port [502] (2) SLOT_IO => COM Port [Dflt: 1] (3) All Others => Not Used
	`DeviceID` VARCHAR(12) NOT NULL, -- Modbus Device (i.e. Net ID)
	`DeviceChan` VARCHAR(255) NOT NULL, -- Modbus Channel to Set - Only used for Modbus & SLOT_IO clients - Format: [ ChAddr1_On,ChAddr2_On,ChAddr3_On|ChAddr1_On,ChAddr3_On ] i.e [ 10001,10002,10003|10001,10003 ]
	`DeviceOnVal` VARCHAR(8) NULL DEFAULT NULL, -- Value to represent "ON" (if needed)
	`DeviceOffVal` VARCHAR(8) NULL DEFAULT NULL, -- Value to represent "OFF" (if needed)
	`ResetTime` VARCHAR(3) NULL DEFAULT NULL, -- Seconds to send reset signal (if needed)
	`Inactive` TINYINT(1) NOT NULL DEFAULT 0
);

DROP TABLE IF EXISTS `Units`;
CREATE TABLE Units (
	`UnitID` VARCHAR(12) NOT NULL PRIMARY KEY,
	`UnitLabel` VARCHAR(32) NOT NULL,
	`AudioAnnounce` VARCHAR(128) NULL DEFAULT NULL,
	`Inactive` TINYINT(1) NOT NULL DEFAULT 1
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `Incident`;
CREATE TABLE Incident (
	`EventNo` VARCHAR(32) NOT NULL PRIMARY KEY,
	`IncidentNo` VARCHAR(25) NOT NULL,
	`ReportNo` VARCHAR(25) NULL DEFAULT NULL,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`EntryTime` DATETIME NULL DEFAULT NULL,
	`OpenTime` DATETIME NULL DEFAULT NULL,
	`DispatchTime` DATETIME NULL DEFAULT NULL,
	`EnrouteTime` DATETIME NULL DEFAULT NULL,
	`OnsceneTime` DATETIME NULL DEFAULT NULL,
	`CloseTime` DATETIME NULL DEFAULT NULL,
	`CallType` VARCHAR(12) NOT NULL,
	`Nature` VARCHAR(100) NULL DEFAULT NULL,
	`Station` VARCHAR(6) NULL DEFAULT NULL,
	`BoxArea` VARCHAR(6) NULL DEFAULT NULL,
	`Location` VARCHAR(255) NOT NULL,
	`LocationNote` VARCHAR(255) NULL DEFAULT NULL,
	`CrossSt1` VARCHAR(128) NOT NULL,
	`CrossSt2` VARCHAR(128) NOT NULL,
	`GPSLatitude` VARCHAR(64) NULL DEFAULT NULL,
	`GPSLongitude` VARCHAR(64) NULL DEFAULT NULL,
	`UnitList` VARCHAR(255) NULL DEFAULT NULL,
	`Comments` TEXT,
	KEY `IncidentNo` ( `IncidentNo` ),
	KEY `OpenTime` ( `OpenTime` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `IncidentUnit`;
CREATE TABLE IncidentUnit (
	ObjID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	IncidentNo VARCHAR(24) NOT NULL,
	UnitID VARCHAR(12) NOT NULL,
	DispatchTime DATETIME NULL DEFAULT NULL,
	Cancelled TINYINT(1) NOT NULL DEFAULT 0,
	Key `IncidentNo` ( `IncidentNo`, `UnitID` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `RF_Incident`;
CREATE TABLE RF_Incident (
	ObjID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`ToneId` INT NOT NULL,
	FOREIGN KEY ( ToneId ) REFERENCES RF_Alert ( ToneId )
) ENGINE=MYISAM;

DROP TABLE IF EXISTS `CallGroup`;
CREATE TABLE CallGroup (
	`CallGroup` VARCHAR(24) NOT NULL PRIMARY KEY,
	`AlertDisplayColor` VARCHAR(16) NULL DEFAULT NULL,
	`AlertAudioAnnounce` VARCHAR(64) NULL DEFAULT NULL,
	`AudioPreAmble` VARCHAR(64) NULL DEFAULT NULL,
	`AudioPostAmble` VARCHAR(64) NULL DEFAULT NULL,
	`Priority` TINYINT(1) NOT NULL DEFAULT 0
) ENGINE=MyISAM;

INSERT INTO CallGroup VALUES
( 'FIRE', 'rainbow1', 'audio/Box.mp3', '', '', 1),
( 'LOCAL', 'red', 'audio/Local.mp3', '', '', 0),
( 'RESCUE', 'yellow', 'audio/Rescue_Local.mp3', '', '', 0),
( 'EMS', 'green', 'audio/Medical_Local.mp3', '', '', 0),
( 'MISC', 'green', 'audio/tts/default/misc/GeneralAnnouncement.wav', '', '', 0);

DROP TABLE IF EXISTS `CallType`;
CREATE TABLE CallType (
	`TypeCode` VARCHAR(12) NOT NULL PRIMARY KEY,
	`CallGroup` VARCHAR(64) NOT NULL,
	`Label` VARCHAR(64) NOT NULL,
	`Priority` TINYINT(1) NOT NULL DEFAULT 0,
	`AlertDisplayColor` VARCHAR(16) NULL DEFAULT NULL,
	`AlertAudioAnnounce` VARCHAR(64) NULL DEFAULT NULL
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `AlertQueueFilter`;
CREATE TABLE AlertQueueFilter (
	`ObjID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Agency` VARCHAR(20) NULL DEFAULT NULL,
	`BoxArea` VARCHAR(12) NOT NULL COMMENT 'One box area per row, regex matching permitted',
	`CallType` TEXT NOT NULL COMMENT 'Formatting options: (1) Pipe delimited types (i.e. TYPE1|TYPE2|TYPE3 -- regex/wildcard matching: TYPE1.*|TYPE2[0-3] -- prefix optional: type:TYPE1.* ) (2) Pipe delimited groups (i.e. group:LOCAL|group:FIRE -- wildcard matching: group:*)',
	`AreaAnnounce` VARCHAR(255) NULL DEFAULT NULL,
	KEY `BoxArea` ( `BoxArea` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `AlertQueue_Rules`;
CREATE TABLE AlertQueue_Rules (
	`RuleId` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Agency` VARCHAR(20) NULL DEFAULT NULL COMMENT 'Optional agency filter, one per row',
	`BoxArea` VARCHAR(100) NOT NULL COMMENT 'Regex formatting box area, one per row',
	`Ruleset` TEXT NOT NULL COMMENT 'Regex formatted call type and/or call group rules to apply to corresponding box area and agency (if not null)',
	`AreaAnnounce` VARCHAR(255) NULL DEFAULT NULL,
	KEY `BoxArea` ( `BoxArea` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `ActiveIncident`;
CREATE TABLE ActiveIncident (
	`EventNo` VARCHAR(24) NOT NULL PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`IncidentNo` VARCHAR(24) NOT NULL,
	`OpenTime` DATETIME NULL DEFAULT NULL,
	`CloseTime` DATETIME NULL DEFAULT NULL,
	`CallType` VARCHAR(12) NOT NULL,
	`Nature` VARCHAR(18) NULL DEFAULT NULL,
	`BoxArea` VARCHAR(12) NOT NULL,
	`Location` VARCHAR(255) NOT NULL,
	`Priority` TINYINT(1) NOT NULL DEFAULT 0,
	`Update` TINYINT(1) NOT NULL DEFAULT 0,
	`Cancel` TINYINT(1) NOT NULL DEFAULT 0,
	KEY `IncidentNo` ( `IncidentNo` )
) ENGINE=MyISAM;


DROP TABLE IF EXISTS `SMSAlerts`;
CREATE TABLE SMSAlerts (
	`SMSID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`MemberName` VARCHAR(128) NOT NULL,
	`PhoneNo` VARCHAR(12) NOT NULL,
	`AlertQueueNotify` TINYINT(1) NOT NULL DEFAULT 0, -- 1 => Receive alertqueue alerts, 0 => Don't
	`Inactive` TINYINT(1) NOT NULL DEFAULT 0,
	`ScheduleTimeOfDay` VARCHAR(9) NULL DEFAULT NULL,
	`ScheduleDayOfWeek` VARCHAR(7) NULL DEFAULT NULL,
	`Carrier` VARCHAR(24) NOT NULL,
	KEY `MemberName` ( `MemberName` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `SMSCarrier`;
CREATE TABLE SMSCarrier (
	`Carrier` VARCHAR(16) NOT NULL PRIMARY KEY,
	`GatewayAddr` VARCHAR(64) NULL DEFAULT NULL
) ENGINE=MyISAM;

INSERT INTO SMSCarrier
VALUES
('Alltel', NULL),
('ATT_Wireless_WCTP', NULL),
('ATT_Wireless_SMTP', NULL),
('Boost', NULL),
('Cingular', NULL),
('Sprint_PCS', NULL),
('T-Mobile', NULL),
('Nextel', NULL),
('Verizon', NULL),
('Cricket', NULL);

DROP TABLE IF EXISTS `SMSAlertPrefs`;
CREATE TABLE SMSAlertPrefs (
	`AlertID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`SMSID` INT NOT NULL,
	`AlertPref` VARCHAR(12) NOT NULL DEFAULT 'CALLTYPE', -- [ CALLTYPE | UNIT ]
	`AlertContext` VARCHAR(12) NOT NULL,
	KEY `SMSID` ( `SMSID`, `AlertPref` )
) ENGINE=MyISAM;


DROP TABLE IF EXISTS `SMSNotifications`;
CREATE TABLE SMSNotifications (
	`NotificationID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`EventNo` VARCHAR(24) NOT NULL,
	`IncidentNo` VARCHAR(24) NOT NULL,
	`CallType` VARCHAR(12) NOT NULL,
	`UnitList` VARCHAR(255) NULL DEFAULT NULL,
	`SMSRecipients` TEXT,
	KEY `EventNo` ( `EventNo` ),
	KEY `IncidentNo` ( `IncidentNo` )
) ENGINE=MyISAM;


DROP TABLE IF EXISTS `RSSFeed`;
CREATE TABLE RSSFeed (
	`RSSID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`IncidentNo` VARCHAR(24) NOT NULL,
	`CallType` VARCHAR(12) NOT NULL,
	`Nature` VARCHAR(18) NOT NULL,
	`UnitList` VARCHAR(255) NULL DEFAULT NULL,
	KEY `IncidentNo` ( `IncidentNo` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `ErrorLog`;
CREATE TABLE ErrorLog (
	`RowId` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP,
	`LogFile` VARCHAR(128) NOT NULL,
	`Level` VARCHAR(16) NOT NULL DEFAULT 'ERROR',
	`ErrorText` VARCHAR(255) NOT NULL,
	KEY `LogFile` ( `LogFile` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `CommLog`;
CREATE TABLE CommLog (
	`ObjId` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`Type` INT NOT NULL DEFAULT 1,
	`ActivateZone` TINYINT(1) NOT NULL DEFAULT 1,
	`Ack` TINYINT(1) NOT NULL DEFAULT 0,
	`Cleared` TINYINT(1) NOT NULL DEFAULT 0,
	KEY `ZoneKey` ( `ActivateZone`, `Ack` )
) ENGINE=MyISAM;

DROP TABLE IF EXISTS `RF_Alert`;
CREATE TABLE RF_Alert (
	`ToneId` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`IO_Device` VARCHAR(10) NOT NULL, -- SlotIO Model for LinPAC SDK
	`IO_Slot` INT NOT NULL,
	`IO_Channel` INT NOT NULL,
	`ToneDescr` VARCHAR(100) NOT NULL,
	`DisplayLabel` VARCHAR(64) NULL DEFAULT NULL,
	`DisplayColor` VARCHAR(16) NULL DEFAULT NULL,
	`AudioPreamble` VARCHAR(255) NULL DEFAULT NULL,
	`AudioAnnouncement` VARCHAR(64) NULL DEFAULT NULL,
	UNIQUE KEY IO_Slot_IO_Channel ( `IO_Slot`, `IO_Channel` )
) ENGINE=MyISAM AUTO_INCREMENT=1;