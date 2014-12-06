HTTP/1.1 200 OK
Server: Apache/2.2.14 (Ubuntu)
ETag: W/"12169-1297447410000"
Last-Modified: Fri, 11 Feb 2011 18:03:30 GMT
Vary: Accept-Encoding
Content-Length: 12169
Content-Type: text/javascript
Date: Fri, 25 Mar 2011 14:00:38 GMT
Connection: close

var usbTabNumOfPointsSend = 0;

/* TODO will be enable in v3.0.x release part 2
//function usbTabLoginByEnter(e) {
//
//	
//	if (isEnterPressed(e)) {
//		saveUsb._clickWriteData();
//	} // end if
//}
*/

var SaveUsbClass = Class.create();
SaveUsbClass.prototype = {

	initialize: function() {
	
		this.status = $("usbStatusText");
	
		// auto-install related
		this.latestVersion = [2,9,2,0]; // TODO update GCP version
		this.msgPluginNotInstalled = $("msgPluginNotInstalled");
		this.msgPluginOutOfDate = $("msgPluginOutOfDate");
		this.msgPluginVersion = $("msgPluginVersion");
		this.downloadInstallPlugin = $("downloadInstallPlugin");
		
		// HTML controls
		this.usbFindDevicesButton = $("usbFindDevicesButton");
		this.usbFindDevicesLink = $("usbFindDevicesLink");
		this.usbWriteDataButton = $("usbWriteDataButton");
		/* TODO will be enable in v3.0.x release part 2
//		this.usbMgLoginCheckbox = $("usbMgLoginCheckbox");
		*/
		this.usbSendMoreDevicesButton = $("usbSendMoreDevicesButton");
		
		// div blocks
		this.divUsbNoDeviceFound = $("divUsbNoDeviceFound");
		this.divUsbSelectDevice = $("divUsbSelectDevice");
		this.deviceInfo = $("usbDeviceInfoList");
		this.divUsbSaveDeviceComplete = $("divUsbSaveDeviceComplete");
		/* TODO will be enable in v3.0.x release part 2
//		this.divUsbSaveMgNotes = $("divUsbSaveMgNotes");
		*/
		
		this.updateDevices = new Array();

		this.garminController = null;
		
		try {	
			this.garminController = new Garmin.DeviceControl();
			this.garminController.register(this);
			
			// validate plugin
			this.garminController.garminPlugin.setPluginLatestVersion(this.latestVersion);
			this.garminController.garminPlugin.setPluginRequiredVersion(this.latestVersion);
			this.garminController.validatePlugin();
	
			// unlock controller
			if (this.garminController.unlock(garminSiteKeys)) {
			
	        	this.setStatus(i18nPluginInitialized, false);
	        	
	        	// bind HTML controls
	        	this.usbFindDevicesButton.onclick = this._clickFindDevices.bind(this);
	        	this.usbFindDevicesLink.onclick = this._clickFindDevices.bind(this);
	        	this.usbWriteDataButton.onclick = this._clickWriteData.bind(this);
	        	this.usbSendMoreDevicesButton.onclick = this._clickFindDevices.bind(this);
	        	
				// find device
				this.garminController.findDevices();
			}
			else {
	        	this.setStatus(i18nPluginUnlockFailed, true);
	        	
	        	// GATC
				try {
					pageTracker._trackEvent(gatcEC_err, gatcEA_err_gcpUnlockFailed);
				} catch(err) {}
			} // end if

		} catch (e) { this.handleException(e); }
	},
	
	_clickFindDevices: function() {
		
		this.divUsbNoDeviceFound.style.display = "none";
		this.divUsbSelectDevice.style.display = "none";
		this.divUsbSaveDeviceComplete.style.display = "none";
    	this.deviceInfo.innerHTML = "";
    	
    	try {
    		this.garminController.findDevices();
    	} catch (e) { this.handleException(e); }
	},
	
	_clickWriteData: function() {
		
        try {
        	// write into devices
	        var devices = this.garminController.getDevices();
	        var numDevices = devices.length;
	        var isAnyDeviceChecked = false;
	        
	        this.updateDevices.clear();
	        for (var i = 0; i < numDevices; i = i + 1) {
	        	var elementChecked = this.deviceInfo.elements[i].checked;
	        	this.updateDevices.push(elementChecked);
	        	
	        	if (elementChecked === true) {
	        		isAnyDeviceChecked = true;
	        	} // end if
			} // end for
			
			if (isAnyDeviceChecked === false) {
        		this.divUsbSelectDevice.style.display = "inline";
        		alert(i18nNoDeviceSelected);
			}
			else {
				/* TODO will be enable in v3.0.x release part 2
//				if (this.usbMgLoginCheckbox.checked) {
//					usbTabSavePoiToMyGarmin();
//				}
//				else {
				*/
				
					this._writeToDevice();
				/* TODO will be enable in v3.0.x release part 2
//				} // end if
				*/
			} // end if
			
		} catch (e) { this.handleException(e); }
	},
	
	_getDeviceName: function(device) {
		var displayName = device.getDisplayName();
		var description = device.getDescription();
		var devId = device.getId();
		
		if (displayName.indexOf(description, 0) != -1 &&
			displayName.indexOf(devId, 0) != -1) {
			return description;
		}
		else {
			return displayName;
		} // end if
	},
	
	_writeToDevice: function() {
		
		this.divUsbSelectDevice.style.display = "none";
		
		var arrayLength = this.updateDevices.length;
		var toBeUpdated = false;
		
		for (var i = 0; i < arrayLength; i = i + 1) {
		
			if (this.updateDevices[i] === true) {
				toBeUpdated = true;
				
				this.updateDevices[i] = false;
				
				var d = new Date();
			    var fname = "gpxfile" + d.getYear() + d.getMonth() +
					d.getDate() + d.getHours() + d.getMinutes() +
					d.getSeconds();
			    	
				this.garminController.setDeviceNumber(i);
				this.garminController.writeToDevice(gpxString, fname);
				break;
			} // end if
			
		} // end for
		
		return toBeUpdated;
	},
	
	onStartFindDevices: function(json) {
        this.setStatus(i18nSearchingDevice, false);
    },
    
    onFinishFindDevices: function(json) {
    	try {
    		
	        if (json.controller.numDevices > 0) {
	        	
	        	this.divUsbSelectDevice.style.display = "inline";
	        	
	        	var devices = json.controller.getDevices();
				var numDevices = devices.length;
				
				// list devices
				this.deviceInfo.innerHTML = "";
		
				for (var i = 0; i < numDevices; i = i + 1) {
					this.deviceInfo.innerHTML += 
						'\n<h3><input name="foundDevices_' + i +
						'" type="checkbox">' + this._getDeviceName(devices[i]) +
						'</input></h3>';
				} // end for
	        }
	        else {
				this.divUsbNoDeviceFound.style.display = "inline";
	        } // end if
	        
    		this.status.style.display = "none";	        
	        
    	} catch (e) { this.handleException(e); }
    },
    
    onStartWriteToDevice: function(json) { 
    	try {
    		var device = this.garminController.getDevices()[this.garminController.deviceNumber];
    		this.setStatus(i18nSaveToDevice + "[" + this._getDeviceName(device) + "]...", false);
    	} catch (e) { this.handleException(e); }
    },
    
    onProgressWriteToDevice: function(json) {
    	this.setStatus(json.progress, false);
    },
    
    onFinishWriteToDevice: function(json) {
    	
    	try {
	    	if (this._writeToDevice() === false) {  	
		       	// GATC
		       	try {
		       		pageTracker._trackEvent(gatcEC_gcp, gatcEA_gcp_saveComplete);
		       	} catch(err) {}
		       	
		       	this.status.style.display = "none";
	    		this.divUsbSaveDeviceComplete.style.display = "inline";
	    		
	    		/* TODO will be enable in v3.0.x release part 2
//	    		if (this.usbMgLoginCheckbox.checked) {
//		    		this.divUsbSaveMgNotes.style.display = "inline";
//	    		}
//	    		else {
//	    			this.divUsbSaveMgNotes.style.display = "none";
//	    		} // end if
	    		*/
	       	} // end if
		} catch (e) { this.handleException(e); }
    },
    
    onException: function(json) {
   		this.handleException(json.msg);
  	},
    
    handleException: function(error) {
    	
		if (error.name == "OutOfDatePluginException") {
			var versionString = "[" + this.garminController.garminPlugin.getPluginVersionString() + "]";

			this.msgPluginVersion.innerHTML = versionString;
			this.msgPluginOutOfDate.style.display = "inline";
			this.downloadInstallPlugin.style.display = "inline";
			
			// GATC
	       	try {
	       		pageTracker._trackEvent(gatcEC_gcp, gatcEA_gcp_outOfDate);
	       	} catch(err) {}
		}
		else if (error.name == "PluginNotInstalledException") { 

			this.msgPluginNotInstalled.style.display = "inline";
			this.downloadInstallPlugin.style.display = "inline";
			
			// GATC
	       	try {
	       		pageTracker._trackEvent(gatcEC_gcp, gatcEA_gcp_notInstalled);
	       	} catch(err) {}
		}
		else if (error.name == "BrowserNotSupportedException") { 

			this.setStatus(i18nBrowserNotSupported, true);
			
			// GATC
	       	try {
	       		pageTracker._trackEvent(gatcEC_gcp, gatcEA_gcp_browserNotSupported);
	       	} catch(err) {}
		}
		else {
			
			var msg = error.name + " : " + error.message;
			
			if (Garmin.PluginUtils.isDeviceErrorXml(error)) {
				msg = Garmin.PluginUtils.getDeviceErrorMessage(error);	
			} // end if
			
			if (msg.indexOf("Windows Error Code: 3", 0) != -1) {
				// Windows Error Code: 3
				// The system cannot find the path specified.
				this.divUsbNoDeviceFound.style.display = "inline";
				this.status.style.display = "none";
			}
			else {
				this.setStatus(
						i18nActionAborted + " " + i18nUnexpectedMessage + 
				    	"<br /><br /><b>" + msg + "</b>", true);
			} // end if
		} // end if
	},

	setStatus: function(statusText, isError) {
		
		this.status.style.display = "inline";
		
		if (isError === true) {
			this.status.style.color = "red";
	    }
	    else {
	    	this.status.style.color = "blue";
	    } // end if
	    
	    this.status.innerHTML = statusText;
	}
};

function clickCbUsbMgLogin() {
	
	if (document.getElementById("usbMgLoginCheckbox").checked) {
		MygarminAuth.isAuthenticated({
			callback: function(data) {
				if (data == true) {
					document.getElementById("divUsbMgLogin").style.display = "none";
				}
				else {
					document.getElementById("divUsbMgLogin").style.display = "inline";
				} // end if
			},
			errorHandler: dwrCbError,
			exceptionHandler: dwrCbException,
			textHtmlHandler: dwrCbTextHtml
		});
	}
	else {
		document.getElementById("divUsbMgLogin").style.display = "none";
	} // end if	
}

function usbTabSavePoiToMyGarmin() {
	
	// block UI
	jqBlockUi();
	
	MygarminAuth.isAuthenticated({
		callback: function(data) {
			if (data == true) {
				cbUsbTabSaveToMyGarmin(true);
			}
			else {
				var username = document.getElementById("usbMgUsername");
				var password = document.getElementById("usbMgPassword");
				
				if (username.value.length == 0 || password.value.length == 0) {
					logout(true);
				}
				else {
					MygarminAuth.login(
							username.value,
							password.value,
							{
								callback: cbUsbTabSaveToMyGarmin,
								errorHandler: dwrCbError,
								exceptionHandler: dwrCbException,
								textHtmlHandler: dwrCbTextHtml
							});
				} // end if
			} // end if
		},
		errorHandler: dwrCbError,
		exceptionHandler: dwrCbException,
		textHtmlHandler: dwrCbTextHtml
	});
	
}

function cbUsbTabSaveToMyGarmin(data) {

	if (data == true) {
		
		saveUsb.setStatus(i18nSavingLocToMg, false);
		
		usbTabNumOfPointsSend = 0;
		
		for (var i = 0; i < mgPoiArray.length; i = i + 1) {
		
			ManageLocations.saveSingleWaypointToMyGarmin(
					mgPoiArray[i],
					{
						callback: function(data) {
							
							usbTabNumOfPointsSend = usbTabNumOfPointsSend + 1;
						
							if (usbTabNumOfPointsSend == mgPoiArray.length) {
								loginUiHandler();
								
								// GATC
								try {
									pageTracker._trackEvent(gatcEC_funcTabs,
											gatcEA_funcTabs_saveMgComplete);
								} catch(err) {}
								
								// unblock UI
								jQuery.unblockUI();
								
								// write into device
								saveUsb._writeToDevice();
							} // end if
						},
						errorHandler: dwrCbError,
						exceptionHandler: dwrCbException,
						textHtmlHandler: dwrCbTextHtml
					});
		} // end for
	}
	else {
		logout(true);
	} // end if
}

function loginUsbTab() {
	document.getElementById("divUsbMgLogin").style.display = "none";
}

function logoutUsbTab() {
	
	if (document.getElementById("usbMgLoginCheckbox").checked) {
		document.getElementById("divUsbMgLogin").style.display = "inline";
	}
	else {
		document.getElementById("divUsbMgLogin").style.display = "none";
	} // end if
}
