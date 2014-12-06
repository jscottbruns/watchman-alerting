HTTP/1.1 200 OK
Server: Apache/2.2.14 (Ubuntu)
ETag: W/"6113-1297447410000"
Last-Modified: Fri, 11 Feb 2011 18:03:30 GMT
Vary: Accept-Encoding
Content-Length: 6113
Content-Type: text/javascript
Date: Fri, 25 Mar 2011 14:00:38 GMT
Connection: close

var wlsTabNumOfPointsSend = 0;

function wlsTabLoginByEnter(e) {

	if (isEnterPressed(e)) {
		clickWlsTabSignInButton();
	} // end if
}

function clickWlsTabSignInButton() {

	// block UI
	jqBlockUi();
	
	var username = document.getElementById("wlsTabMgUsername");
	var password = document.getElementById("wlsTabMgPassword");
	
	if (username.value.length == 0 || password.value.length == 0) {
		logout(true);
	}
	else {
		MygarminAuth.login(
				username.value,
				password.value,
				{
					callback: function(data) {
						if (data == true) {
							loginUiHandler();
							
							// unblock UI
							jQuery.unblockUI();
						}
						else {
							logout(true);
						} // end if
					},
					errorHandler: dwrCbError,
					exceptionHandler: dwrCbException,
					textHtmlHandler: dwrCbTextHtml
				});
	} // end if
}

function logoutWirelessTab() {
	document.getElementById("divWlsMgSignIn").style.display = "inline";
	
	document.getElementById("wlsTabNoCompatible").style.display = "none";
	document.getElementById("divWlsSelectDevices").style.display = "none";
	document.getElementById("divWlsLocationSent").style.display = "none";
}

function loginWirelessTab() {
	clickWlsTabRefreshList();
}

function clickWlsTabRefreshList() {
	
	// block UI
	jqBlockUi();
	
	MygarminAuth.isAuthenticated({
			callback: cbWlsTabRefreshList,
			errorHandler: dwrCbError,
			exceptionHandler: dwrCbException,
			textHtmlHandler: dwrCbTextHtml
		});
}

function cbWlsTabRefreshList(data) {
	
	if (data == true) {

		ManageLocations.getDeviceList({
			callback: function(devices) {
				if (devices == null) {
					document.getElementById("divWlsMgSignIn").style.display =
						"inline";
					document.getElementById("wlsTabNoCompatible").style.display =
						"inline";
					
					// clear device list
					document.getElementById("divWlsSelectDevices").style.display =
						"none";
					document.getElementById("wlsTabDeviceInfoList").innerHTML =
						"";
					
					// GATC
					try {
						pageTracker._trackEvent(gatcEC_funcTabs,
								gatcEA_funcTabs_noWlsDeviceReg);
					} catch(err) {}
				}
				else {
					var wlsTabDeviceInfoList =
						document.getElementById("wlsTabDeviceInfoList");
					
					wlsTabDeviceInfoList.innerHTML = "";
					
					for (var i = 0; i < devices.length; i = i + 1) {
						
						var _unitId = devices[i].unitId;
						
						wlsTabDeviceInfoList.innerHTML += 
							'<h3><input name="wlsDev_' + _unitId + 
							'" type="checkbox" value="'+ _unitId + '" />' +
							devices[i].model +
							' (' + i18nWlsTabUnitId + ' ' + _unitId + ')' +
							'</h3>';
					} // end for
					
					document.getElementById("divWlsMgSignIn").style.display =
						"none";
					document.getElementById("divWlsLocationSent").style.display =
						"none";
					document.getElementById("divWlsSelectDevices").style.display =
						"inline";
				} // end if
				
				// unblock UI
				jQuery.unblockUI();
			},
			errorHandler: dwrCbError,
			exceptionHandler: dwrCbException,
			textHtmlHandler: dwrCbTextHtml
		});
	}
	else {
		logout(true);
	} // end if
}

function clickWlsTabSendToGpsButton() {
	// block UI
	jqBlockUi();
	
	MygarminAuth.isAuthenticated({
		callback: cbWlsTabSendToGps,
		errorHandler: dwrCbError,
		exceptionHandler: dwrCbException,
		textHtmlHandler: dwrCbTextHtml
	});
}

function cbWlsTabSendToGps(data) {
	
	if (data == true) {
		var devElements = document.getElementById("wlsTabDeviceInfoList").elements;
		
		/* TODO will be enable in v3.0.x release part 2
//		var isSaveToMygarmin = document.getElementById("wlsSaveMgCheckBox").checked;
		*/

		var unitIdArray = new Array();
		var j = 0;
		
		for(var i = 0; i < devElements.length; i = i + 1) {
			
			if (devElements[i].checked) {
				unitIdArray[j] = devElements[i].value;
				j = j + 1;
			} // end if
		} // end for
		
		if (unitIdArray.length == 0) {
			// unblock UI
			jQuery.unblockUI();
			
			alert(i18nNoDeviceSelected);
		}
		else {

			wlsTabNumOfPointsSend = 0;
			
			for (var i = 0; i < mgPoiArray.length; i = i + 1) {
				
				ManageLocations.sendSingleWaypointWirelessly(
						/* TODO will be enable in v3.0.x release part 2
//						isSaveToMygarmin,
						*/
						false,
						unitIdArray,
						mgPoiArray[i],
						{
							callback: function(data) {
								
								wlsTabNumOfPointsSend = wlsTabNumOfPointsSend + 1;
							
								if (wlsTabNumOfPointsSend == mgPoiArray.length) {
									// all points are sent
									
									document.getElementById("divWlsSelectDevices").style.display = 
										"none";
									document.getElementById("divWlsLocationSent").style.display =
										"inline";
									
									/* TODO will be enable in v3.0.x release part 2
//									if (isSaveToMygarmin) {
//										document.getElementById("divWlsSaveMgMsg").style.display =
//											"inline";
//										
//										// GATC
//										try {
//											pageTracker._trackEvent(gatcEC_funcTabs,
//													gatcEA_funcTabs_saveMgComplete);
//										} catch(err) {}
//									}
//									else {
//										document.getElementById("divWlsSaveMgMsg").style.display =
//											"none";
//									} // end if
									*/
									
									// GATC
									try {
										pageTracker._trackEvent(gatcEC_funcTabs,
												gatcEA_funcTabs_sendWlsComplete);
									} catch(err) {}
									
									// unblock UI
									jQuery.unblockUI();
								} // end if
							},
							errorHandler: dwrCbError,
							exceptionHandler: dwrCbException,
							textHtmlHandler: dwrCbTextHtml
						});
			} // end for

		} // end if
	}
	else {
		logout(true);
	} // end if
}

function clickWlsTabSendMoreDevicesButton() {
	
	// block UI
	jqBlockUi();
	
	MygarminAuth.isAuthenticated({
		callback: cbWlsTabRefreshList,
		errorHandler: dwrCbError,
		exceptionHandler: dwrCbException,
		textHtmlHandler: dwrCbTextHtml
	});
}
