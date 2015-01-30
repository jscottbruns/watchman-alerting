DROP TABLE IF EXISTS `Settings`;
CREATE TABLE `Settings` (
	`Name` VARCHAR(45) NOT NULL PRIMARY KEY,
	`Descr` VARCHAR(128) NOT NULL,
	`Category` VARCHAR(16) NOT NULL,
	`Setting` VARCHAR(255) NULL DEFAULT NULL,
	INDEX `index_settings_category` ( `Category` )
) ENGINE=InnoDB;

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
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `SystemProperties`;
CREATE TABLE `SystemProperties` (
	`PropName` VARCHAR(45) NOT NULL PRIMARY KEY,
	`PropValue` VARCHAR(64) NULL DEFAULT NULL
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `RF_Alerts`;
CREATE TABLE RF_Alerts (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`IO_Device` VARCHAR(10) NOT NULL, -- SlotIO Model for LinPAC SDK
	`IO_Slot` INT NOT NULL,
	`IO_Channel` INT NOT NULL,
	`ToneDescr` VARCHAR(100) NOT NULL,
	`DisplayLabel` VARCHAR(64) NULL DEFAULT NULL,
	`DisplayColor` VARCHAR(16) NULL DEFAULT NULL,
	`AudioPreamble` VARCHAR(255) NULL DEFAULT NULL,
	`AudioAnnouncement` VARCHAR(64) NULL DEFAULT NULL,
	UNIQUE KEY index_rf_alerts_io_slot_io_channel ( `IO_Slot`, `IO_Channel` )
) ENGINE=InnoDB AUTO_INCREMENT=1;

DROP TABLE IF EXISTS `iCADAlertTrans`;
CREATE TABLE iCADAlertTrans (
	`LocalTransID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`iCADTransID` INT NOT NULL,
	`ReceivedTime` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`SentTime` DATETIME NOT NULL,
	`StreamData` TEXT
) ENGINE=InnoDB AUTO_INCREMENT=1000;

DROP TABLE IF EXISTS `Units`;
CREATE TABLE Units (
	`ID` INT NOT NULL PRIMARY KEY,
	`UnitID` VARCHAR(12) NOT NULL,
	`UnitLabel` VARCHAR(32) NOT NULL,
	`AudioAnnounce` VARCHAR(128) NULL DEFAULT NULL,
	`Inactive` TINYINT(1) NOT NULL DEFAULT 1,
	UNIQUE KEY `index_units_unit_id` (`UnitID`)
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `AlertZones`; -- Physical Alert Zones
CREATE TABLE AlertZones (
	`ZoneID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
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
	INDEX index_alert_zones_host_addr ( `HostAddr` ),
	INDEX index_alert_zones_iface_host_addr ( `iFaceHostAddr` )
) ENGINE=InnoDB AUTO_INCREMENT=100;

DROP TABLE IF EXISTS `AlertGroups`; -- Static unit alert groups
CREATE TABLE AlertGroups (
	`GroupID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`GroupAddr` VARCHAR(16) NOT NULL,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`GroupName` VARCHAR(24) NOT NULL,
	`GroupType` TINYINT(1) NOT NULL DEFAULT 0, -- 0=DEFAULT; 1=UNIT/TONE
	`UnitID` INT NULL DEFAULT NULL,
	`AlertQueue_PriAlert` TINYINT(1) NOT NULL DEFAULT 0, -- 1=>Enabled Priority AlertQueue Alerts 0=>Disable AlertQueue Priority Alerts
	`AlertQueue_LocalAlert` TINYINT(1) NOT NULL DEFAULT 0, -- 1=>Enabled First Due AlertQueue Alerts 0=>Disable First Due Priority Alerts
	`AlertQueue_Filter` VARCHAR(255) NULL DEFAULT NULL, -- Box area filter for AlertQueue alerts
	`AudioPreamble` VARCHAR(255) NULL DEFAULT NULL, -- Preamble alert tone, otherwise will use DefaultAutioPreamble in audioalerting settings
	`ToneID` INT NULL DEFAULT NULL, -- Regexp filter for association w/RF tone channels
	UNIQUE KEY `index_alert_groups_group_addr` ( `GroupAddr` ),
	INDEX `index_alert_groups_group_type` ( `GroupType`, `UnitId` ),
	CONSTRAINT `alert_groups_unit_id_fk` FOREIGN KEY (`UnitID`) REFERENCES `Units` (`ID`) ON DELETE CASCADE,
	CONSTRAINT `alert_groups_tone_id_fk` FOREIGN KEY (`ToneID`) REFERENCES `RF_Alerts` (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1;

DROP TABLE IF EXISTS `AlertGroupMembers`; -- Dynamic alert zone group membership - Used for displaying groups on zone interface
CREATE TABLE AlertGroupMembers (
	`MemberID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	`GroupID` INT NOT NULL,
	`ZoneID` INT NOT NULL,
	CONSTRAINT `alert_group_members_group_id_fk` FOREIGN KEY (`GroupID`) REFERENCES `AlertGroups` (`GroupID`) ON DELETE CASCADE,
	CONSTRAINT `alert_group_members_zone_id_fk` FOREIGN KEY (`ZoneID`) REFERENCES `AlertZones` (`ZoneID`) ON DELETE CASCADE
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `IO_Devices`;
CREATE TABLE `IO_Devices` (
	`TriggerID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`EventType` TINYINT(1) NOT NULL DEFAULT 0, -- (0) Generic Alert Triggered Event (1) Unit Alert Triggered Event (2) Tone Alert Triggered Event
	`UnitID` INT NULL DEFAULT NULL,
	`ToneID` INT NULL DEFAULT NULL,
	`TriggerType` VARCHAR(10) NOT NULL DEFAULT 'MB_TCP', -- (1) MB_TCP => ModbusTCP Event (2) MB_RTU => ModbusRTU IO Event (3) MB_RTUIP => ModbusRTU/IP Event (4) TCPIP => TCP/IP Command Event (5) SLOT_IO => LinPAC Slot IO
	`DeviceAddr` VARCHAR(32) NOT NULL, -- (1) MB_TCP => IPv4 Addr (2) MB_RTU => TTY Addr (3) SLOT_IO => Slot No (1-x)
	`DevicePort` VARCHAR(5) NULL DEFAULT NULL, -- (1) MbusTCP => IPv4 Remote Port [502] (2) SLOT_IO => COM Port [Dflt: 1] (3) All Others => Not Used
	`DeviceID` VARCHAR(12) NOT NULL, -- Modbus Device (i.e. Net ID)
	`DeviceChan` VARCHAR(255) NOT NULL, -- Modbus Channel to Set - Only used for Modbus & SLOT_IO clients - Format: [ ChAddr1_On,ChAddr2_On,ChAddr3_On|ChAddr1_On,ChAddr3_On ] i.e [ 10001,10002,10003|10001,10003 ]
	`DeviceOnVal` VARCHAR(8) NULL DEFAULT NULL, -- Value to represent "ON" (if needed)
	`DeviceOffVal` VARCHAR(8) NULL DEFAULT NULL, -- Value to represent "OFF" (if needed)
	`ResetTime` VARCHAR(3) NULL DEFAULT NULL, -- Seconds to send reset signal (if needed)
	`Inactive` TINYINT(1) NOT NULL DEFAULT 0,
	CONSTRAINT `io_devices_unit_id_fk` FOREIGN KEY (`UnitID`) REFERENCES `Units` (`ID`) ON DELETE SET NULL,
	CONSTRAINT `io_devices_tone_id_fk` FOREIGN KEY (`ToneID`) REFERENCES `RF_Alerts` (`ID`) ON DELETE SET NULL
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `CallGroups`;
CREATE TABLE CallGroups (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`CallGroup` VARCHAR(24) NOT NULL,
	`AlertDisplayColor` VARCHAR(16) NULL DEFAULT NULL,
	`AlertAudioAnnounce` VARCHAR(64) NULL DEFAULT NULL,
	`AudioPreAmble` VARCHAR(64) NULL DEFAULT NULL,
	`AudioPostAmble` VARCHAR(64) NULL DEFAULT NULL,
	`Priority` TINYINT(1) NOT NULL DEFAULT 0,
	UNIQUE KEY `index_call_groups_call_group` (`CallGroup`)
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `CallTypes`;
CREATE TABLE CallTypes (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`TypeCode` VARCHAR(12) NOT NULL,
	`CallGroup` INT NOT NULL,
	`Label` VARCHAR(64) NOT NULL,
	`Priority` TINYINT(1) NOT NULL DEFAULT 0,
	`AlertDisplayColor` VARCHAR(16) NULL DEFAULT NULL,
	`AlertAudioAnnounce` VARCHAR(64) NULL DEFAULT NULL,
	UNIQUE KEY `index_call_types_type_code` (`TypeCode`),
	CONSTRAINT `call_types_call_group_fk` FOREIGN KEY (`CallGroup`) REFERENCES CallGroups (`ID`)
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `Incidents`;
CREATE TABLE Incidents (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`EventNo` VARCHAR(32) NULL DEFAULT NULL,
	`IncidentNo` VARCHAR(25) NULL DEFAULT NULL,
	`ReportNo` VARCHAR(25) NULL DEFAULT NULL,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`EntryTime` DATETIME NULL DEFAULT NULL,
	`OpenTime` DATETIME NULL DEFAULT NULL,
	`DispatchTime` DATETIME NULL DEFAULT NULL,
	`EnrouteTime` DATETIME NULL DEFAULT NULL,
	`OnsceneTime` DATETIME NULL DEFAULT NULL,
	`CloseTime` DATETIME NULL DEFAULT NULL,
	`CallType` INT NOT NULL,
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
	UNIQUE KEY `index_incidents_event_no` ( `EventNo` ),
	UNIQUE KEY `index_incidents_incident_no` ( `IncidentNo` ),
	KEY `index_incidents_open_time` ( `OpenTime` ),
	CONSTRAINT `Incidents_call_type_fk` FOREIGN KEY (`CallType`) REFERENCES CallTypes (`ID`)
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `IncidentUnits`;
CREATE TABLE IncidentUnits (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`IncidentID` INT NOT NULL,
	`UnitID` INT NOT NULL,
	`DispatchTime` DATETIME NULL DEFAULT NULL,
	`Cancelled` TINYINT(1) NOT NULL DEFAULT 0,
	UNIQUE KEY `index_incident_no_unit_id` ( `IncidentID`, `UnitID` ),
	CONSTRAINT `incident_units_incident_id_fk` FOREIGN KEY (`IncidentID`) REFERENCES `Incidents` (`ID`) ON DELETE CASCADE,
	CONSTRAINT `incident_units_unit_id_fk` FOREIGN KEY (`UnitID`) REFERENCES `Units` (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `RF_Incidents`;
CREATE TABLE RF_Incidents (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`ToneId` INT NOT NULL,
	CONSTRAINT `rf_incidents_tone_id_fk` FOREIGN KEY ( `ToneId` ) REFERENCES `RF_Alerts` ( `ID` ) ON DELETE CASCADE
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `AlertQueueFilters`;
CREATE TABLE AlertQueueFilters (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Agency` VARCHAR(20) NULL DEFAULT NULL,
	`BoxArea` VARCHAR(12) NOT NULL COMMENT 'One box area per row, regex matching permitted',
	`CallType` TEXT NOT NULL COMMENT 'Formatting options: (1) Pipe delimited types (i.e. TYPE1|TYPE2|TYPE3 -- regex/wildcard matching: TYPE1.*|TYPE2[0-3] -- prefix optional: type:TYPE1.* ) (2) Pipe delimited groups (i.e. group:LOCAL|group:FIRE -- wildcard matching: group:*)',
	`AreaAnnounce` VARCHAR(255) NULL DEFAULT NULL,
	KEY `index_alert_queue_filters_box_area` ( `BoxArea` )
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `AlertQueue_Rules`;
CREATE TABLE AlertQueue_Rules (
	`RuleId` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Agency` VARCHAR(20) NULL DEFAULT NULL COMMENT 'Optional agency filter, one per row',
	`BoxArea` VARCHAR(100) NOT NULL COMMENT 'Regex formatting box area, one per row',
	`Ruleset` TEXT NOT NULL COMMENT 'Regex formatted call type and/or call group rules to apply to corresponding box area and agency (if not null)',
	`AreaAnnounce` VARCHAR(255) NULL DEFAULT NULL,
	KEY `index_alert_queue_rules_box_area` ( `BoxArea` )
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `ActiveIncidents`;
CREATE TABLE ActiveIncidents (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`EventNo` VARCHAR(24) NOT NULL,
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
	KEY `index_active_incidents_event_no` ( `EventNo` ),
	KEY `index_active_incidents_incident_no` ( `IncidentNo` )
) ENGINE=InnoDB;

#
#DROP TABLE IF EXISTS `SMSCarriers`;
#CREATE TABLE SMSCarriers (
#	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
#	`Carrier` VARCHAR(16) NOT NULL,
#	`GatewayAddr` VARCHAR(64) NULL DEFAULT NULL,
#	UNIQUE KEY index_sms_carriers_carrier ( `Carrier` )
#) ENGINE=InnoDB;

#DROP TABLE IF EXISTS `SMSAlerts`;
#CREATE TABLE SMSAlerts (
#	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
#	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
#	`MemberName` VARCHAR(128) NOT NULL,
#	`PhoneNo` VARCHAR(12) NOT NULL,
#	`AlertQueueNotify` TINYINT(1) NOT NULL DEFAULT 0, # 1 => Receive alertqueue alerts, 0 => Don't
#	`Inactive` TINYINT(1) NOT NULL DEFAULT 0,
#	`ScheduleTimeOfDay` VARCHAR(9) NULL DEFAULT NULL,
#	`ScheduleDayOfWeek` VARCHAR(7) NULL DEFAULT NULL,
#	`Carrier` INT NOT NULL,
#	KEY `MemberName` ( `MemberName` ),
#	CONSTRAINT `sms_alerts_carrier_fk` FOREIGN KEY (`Carrier`) REFERENCES SMSCarriers (`ID`)
#) ENGINE=InnoDB;

#DROP TABLE IF EXISTS `SMSAlertPrefs`;
#CREATE TABLE SMSAlertPrefs (
#	`AlertID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
#	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
#	`SMSID` INT NOT NULL,
#	`AlertPref` VARCHAR(12) NOT NULL DEFAULT 'CALLTYPE', # [ CALLTYPE | UNIT ]
#	`AlertContext` VARCHAR(12) NOT NULL,
#	KEY `SMSID` ( `SMSID`, `AlertPref` ),
#	CONSTRAINT `sms_alert_prefs_sms_id_fk` FOREIGN KEY (`SMSID`) REFERENCES SMSAlerts (`ID`) ON DELETE CASCADE
#) ENGINE=InnoDB;

#DROP TABLE IF EXISTS `SMSNotifications`;
#CREATE TABLE SMSNotifications (
#	`NotificationID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
#	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
#	`IncidentNo` VARCHAR(24) NOT NULL,
#	`CallType` VARCHAR(12) NOT NULL,
#	`UnitList` VARCHAR(255) NULL DEFAULT NULL,
#	`SMSRecipient` INT NOT NULL,
#	CONSTRAINT `sms_notifications_incident_no_fk` FOREIGN KEY (`IncidentNo`) REFERENCES Incidents (`ID`) ON DELETE CASCADE,
#	CONSTRAINT `sms_notifications_call_type_fk` FOREIGN KEY (`CallType`) REFERENCES CallTypes (`ID`) ON DELETE CASCADE,
#	CONSTRAINT `sms_notifications_sms_recipient_fk` FOREIGN KEY (`SMSRecipient`) REFERENCES SMSAlerts (`ID`) ON DELETE CASCADE
#) ENGINE=InnoDB;

DROP TABLE IF EXISTS `RSSFeeds`;
CREATE TABLE RSSFeeds (
	`ID` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`IncidentNo` INT NOT NULL,
	CONSTRAINT `rss_feeds_incident_no_fk` FOREIGN KEY (`IncidentNo`) REFERENCES Incidents (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `ErrorLogs`;
CREATE TABLE ErrorLogs (
	`RowId` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP,
	`LogFile` VARCHAR(128) NOT NULL,
	`Level` VARCHAR(16) NOT NULL DEFAULT 'ERROR',
	`ErrorText` VARCHAR(255) NOT NULL,
	KEY `index_error_logs_log_file` ( `LogFile` )
) ENGINE=InnoDB;

DROP TABLE IF EXISTS `CommLogs`;
CREATE TABLE CommLogs (
	`ObjId` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	`Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`Type` INT NOT NULL DEFAULT 1,
	`ActivateZone` TINYINT(1) NOT NULL DEFAULT 1,
	`Ack` TINYINT(1) NOT NULL DEFAULT 0,
	`Cleared` TINYINT(1) NOT NULL DEFAULT 0,
	KEY `index_comm_logs_active_zone_ack` ( `ActivateZone`, `Ack` )
) ENGINE=InnoDB;