var net = require('net');
var server = net.createServer( function(c) 
{	
	console.log('Connected to client ' + c.remoteAddress + ':' + c.remotePort);	

	//c.setEncoding('hex');
	//c.setTimeout(30000, function() { c.end(); } );
		
	c.on('data', function(buf) 
	{
		handleTcpReq(buf, c);					
	});
	
	c.on('end', function() {
		console.log('Server Disconnect');
	});	
	
});

server.listen(49950, '0.0.0.0', function ()
{
    addr = server.address();
    console.log('Alert Zone Server Listening on %j:%j \n', addr.address, addr.port);
});

function handleDisconnect(connection, clientAddr) {
	connection.on('error', function(err) {
    if (!err.fatal) {
		return;
    }

    if (err.code !== 'PROTOCOL_CONNECTION_LOST') {
		throw err;
    }

    console.log('Re-connecting lost connection: ' + err.stack);

    connection = mysql.createConnection(connection.config);
    handleDisconnect(connection);
    connection.connect();
  });
}

var mysql      = require('mysql');
var connection = mysql.createConnection({
	host     : 'localhost',
	user     : 'root',
	password : '',
	database : 'WatchmanAlerting'
});

handleDisconnect(connection);

connection.on('error', function(err) {
	console.log('Database error: ' + error.code);
	if ( ! err.fatal ) {
		return;
	}
	
	if ( err.fatal ) {
		console.log('Error is fatal');
	} 			
});

function handleTcpReq(buf, conn)
{	
	var m;
	var data = buf.toString('ascii', 0, buf.length-1);
	//var data = buf.toString('ascii').replace(/\r\n/, ''); // Debug Only
    console.log("--" + data.toUpperCase() + "--");
	
	// Init
	if ( data.toUpperCase() == 'INI' ) {  
		console.log('[' + conn.remoteAddress + '] INI');
		connection.query('SELECT ZoneID AS ZID, ZoneName AS ZLO, iFaceHostAddr AS ZIP, DefaultVolLevel AS DVL, AlertVolLevel AS AVL, SilentWatch AS SIL, SilentStartTime AS SST, SilentEndTime AS SEN, SilentVolLevel AS SVO FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }	
			var str = 'ZID=' + rows[0].ZID + '&ZLO=' + rows[0].ZLO;		
			console.log('[' + conn.remoteAddress + '] ' + str);
			conn.write(str);			
		});		
	}	
	
	// Zone ID
	else if ( data.toUpperCase() == 'ZID' ) {  
		console.log('[' + conn.remoteAddress + '] ZID');
		connection.query('SELECT ZoneID FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }			
			console.log('[' + conn.remoteAddress + '] ZID=' + rows[0].ZoneID);
			conn.write(rows[0].ZoneID);			
		});		
	}
	
	// Zone Location
	else if ( data.toUpperCase() == 'ZLO' ) {  
		console.log('[' + conn.remoteAddress + '] ZLO');
		connection.query('SELECT ZoneName FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] ZLO=' + rows[0].ZoneName);
			conn.write(rows[0].ZoneName);			
		});		
	}
	
	// Zone Interface IP
	else if ( data.toUpperCase() == 'ZIP' ) {  
		console.log('[' + conn.remoteAddress + '] ZIP'); 
		connection.query('SELECT iFaceHostAddr FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] ZIP=' + rows[0].iFaceHostAddr);
			conn.write(rows[0].iFaceHostAddr);			
		});		
	}
	
	// Zone Interface TCP Control Port
	else if ( data.toUpperCase() == 'PRT' ) {
		console.log('[' + conn.remoteAddress + '] PRT');  
		connection.query('SELECT iFaceTcpControlPort FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] PRT=' + rows[0].iFaceTcpControlPort);
			conn.write(rows[0].iFaceTcpControlPort);			
		});		
	}
	
	// Default Volumne Level
	else if ( data.toUpperCase() == 'DVL' ) {  
		console.log('[' + conn.remoteAddress + '] DVL');	
		connection.query('SELECT DefaultVolLevel FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] DVL=' + rows[0].DefaultVolLevel);
			conn.write(rows[0].DefaultVolLevel);			
		});		
	}
		
	// Alert Volume Level
	else if ( data.toUpperCase() == 'AVL' ) {  
		console.log('[' + conn.remoteAddress + '] AVL');
		connection.query('SELECT AlertVolLevel FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] AVL=' + rows[0].AlertVolLevel);
			conn.write(rows[0].AlertVolLevel);			
		});		
	}
		
	// Silent Watch Enabled/Disabled	
	else if ( data.toUpperCase() == 'SIL' ) {  
		console.log('[' + conn.remoteAddress + '] SIL');
		connection.query('SELECT SilentWatch FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] SIL=' + rows[0].SilentWatch);
			conn.write( ( rows[0].SilentWatch == 1 ? "1" : "0" ) );			
		});		
	}	
	
	// Silent Watch Start Time	
	else if ( data.toUpperCase() == 'SST' ) {  
		console.log('[' + conn.remoteAddress + '] SST');
		connection.query('SELECT SilentStartTime FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] SST=' + rows[0].SilentStartTime);
			conn.write(rows[0].SilentStartTime);			
		});		
	}
		
	// Silent Watch End Time	
	else if ( data.toUpperCase() == 'SEN' ) {  
		console.log('[' + conn.remoteAddress + '] SEN');
		connection.query('SELECT SilentEndTime FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] SEN=' + rows[0].SilentEndTime);
			conn.write(rows[0].SilentEndTime);			
		});		
	}
		
	// Silent Watch Volume Level	
	else if ( data.toUpperCase() == 'SVO' ) {  
		console.log('[' + conn.remoteAddress + '] SVO');
		connection.query('SELECT SilentVolLevel FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			if ( ! rows.length ) { console.log('[' + conn.remoteAddress + '] Empty Result Set'); return; }
			console.log('[' + conn.remoteAddress + '] SVO=' + rows[0].SilentVolLevel);
			conn.write(rows[0].SilentVolLevel);			
		});		
	}	
		
	// Unit Alerting Groups	
	else if ( data.toUpperCase() == 'UNT' ) {  
		console.log('[' + conn.remoteAddress + '] UNT');
		connection.query('SELECT GroupName, UnitID FROM AlertGroups', conn.remoteAddress, function(err, rows) {
			if (err) throw err;							
			var resp = '';
			for ( var i=0; i<rows.length; i++ ) {
				resp += rows[i].GroupName + '=' + ( rows[i].UnitID ? rows[i].UnitID : 'DEF' ) + ( i < rows.length - 1 ? '&' : '' );
			}
			console.log('[' + conn.remoteAddress + '] UNT=' + resp);
			conn.write(resp);			
		});		
	}	
	
	// Unit Alerting Group Membership	
	else if ( data.toUpperCase() == 'UME' ) {  
		console.log('[' + conn.remoteAddress + '] UME');
		connection.query('SELECT t2.GroupName FROM AlertGroupMember t1 LEFT JOIN AlertGroups t2 ON t2.GroupAddr = t1.GroupAddr LEFT JOIN AlertZones t3 ON t3.ZoneID = t1.ZoneID WHERE t3.HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			var resp = '';
			for ( var i=0; i<rows.length; i++ ) {
				resp  += rows[i].GroupName + ( i < rows.length - 1 ? '&' : '' );
			}			
			console.log('[' + conn.remoteAddress + '] UME=' + resp);
			conn.write(resp);			
		});		
	}			
	
	// Unit Alerting Group Membership ID (Same as above but return AlertGroup ID)
	else if ( data.toUpperCase() == 'UMI' ) {  
		console.log('[' + conn.remoteAddress + '] UME');
		connection.query('SELECT t2.UnitID FROM AlertGroupMember t1 LEFT JOIN AlertGroups t2 ON t2.GroupAddr = t1.GroupAddr LEFT JOIN AlertZones t3 ON t3.ZoneID = t1.ZoneID WHERE t3.HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;			
			var resp = '';
			for ( var i=0; i<rows.length; i++ ) {
				resp  += rows[i].UnitID + ( i < rows.length - 1 ? '&' : '' );
			}			
			console.log('[' + conn.remoteAddress + '] UMI=' + resp);
			conn.write(resp);			
		});		
	}
	else if ( m = data.toUpperCase().match(/^RST:(\d*)$/) )
	{
		console.log('[' + conn.remoteAddress + '] ' + data.toUpperCase()); 
		connection.query('SELECT iFaceHostAddr, iFaceTcpControlPort, iFaceSerialPort, IoResetAddr FROM AlertZones WHERE HostAddr = ?', conn.remoteAddress, function(err, rows) {
			if (err) throw err;
			
			var iFaceIp, iFaceTcpPort, iFaceRstCmd, IoResetAddr, AutoUpdate;
			
			if ( rows.length ) 
			{ 
				iFaceIp = '10.100.1.' + rows[0].iFaceHostAddr;
				iFaceTcpPort = rows[0].iFaceTcpControlPort;
				IoResetAddr = rows[0].IoResetAddr;
				iFaceSerialPort = rows[0].iFaceSerialPort;
				iFaceRstCmd = 'V=' + m[1];
				AutoUpdate = rows[0].AutoUpdate;
			}
				
			if ( iFaceIp && iFaceTcpPort )
			{
				console.log('Connecting to AlertZone audio interface [echo ' + iFaceRstCmd + ' | nc -i 1 ' + iFaceIp + ' ' + iFaceTcpPort + ']');
				
				var exec = require('child_process').exec, child;
				
				child = exec(
					'echo ' + iFaceRstCmd + ' | nc -i 1 ' + iFaceIp + ' ' + iFaceTcpPort, 
					{ timeout: 5000 },
					function (error, stdout, stderr) 
					{
						console.log('stdout: ' + stdout);
						console.log('stderr: ' + stderr);
						if ( error !== null ) 
						{ 
							console.log('exec error: ' + error); 
						}
					}
				);	
				
				setTimeout( function() { return; }, 3000 );
			}
			
			if ( iFaceIp && iFaceSerialPort && IoResetAddr )
			{
				var exec = require('child_process').exec, child;
	
				console.log('Calling modbus write utility for zone reset [/usr/local/watchman-alerting/bin/modbus-write -h ' + iFaceIp + ' -p ' + iFaceTcpPort + ' --addr ' + IoResetAddr + '=1]');
				
				child = exec(
					'/usr/local/watchman-alerting/bin/modbus-write -h ' + iFaceIp + ' -p ' + iFaceSerialPort + ' --addr ' + IoResetAddr + '=1', 
					{ timeout: 10000 },
					function (error, stdout, stderr) 
					{
						console.log('stdout: ' + stdout);
						console.log('stderr: ' + stderr);
						if ( error !== null ) 
						{ 
							console.log('exec error: ' + error); 
						}
					}
				);	
				
				setTimeout( function() { return; }, 3000 );
				
				console.log('Resetting AlertZone IO channel [/usr/local/watchman-alerting/bin/modbus-write -h ' + iFaceIp + ' -p ' + iFaceTcpPort + ' --addr ' + IoResetAddr + '=0]');
				
				child = exec(
					'/usr/local/watchman-alerting/bin/modbus-write -h ' + iFaceIp + ' -p ' + iFaceSerialPort + ' --addr ' + IoResetAddr + '=0', 
					{ timeout: 10000 },
					function (error, stdout, stderr) 
					{
						console.log('stdout: ' + stdout);
						console.log('stderr: ' + stderr);
						if ( error !== null ) 
						{ 
							console.log('exec error: ' + error); 
						}
					}
				);				
			}
			
			conn.write('OK\r\n');
			conn.end();
		});
	}
			
	// Exit Request
	else if ( data.toUpperCase() == 'EXT' ) {
		console.log('[' + conn.remoteAddress + '] EXIT');
		conn.end();
	}	
	
	// *DEV-ONLY - Emulate Zone Interface	
	else if ( data.match(/V=.*/) ) {  
		console.log('[' + conn.remoteAddress + '] *DEV* INT_VOL => ' + data);
		conn.write('OK\r\n');			
	}		
	
	else {
		console.log('[' + conn.remoteAddress + '] REQUEST_ERROR=>[' + data.toUpperCase() + ']');
	}
}










